/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL$
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1 or later as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPLv21 included in the packaging of
 * this file.  Please review the following information to ensure the
 * GNU Lesser General Public License version 2.1 requirements will be
 * met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 2.0 or later as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 included in the
 * packaging of this file.  Please review the following information to ensure
 * the GNU General Public License version 2.0 requirements will be
 * met: http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QSocketNotifier>
#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qcore_unix_p.h>

#include "logging.h"
#include "logind/logind.h"
#include "logind/vthandler.h"

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/major.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
}

#ifndef KDSKBMUTE
#define KDSKBMUTE 0x4B51
#endif

#ifndef DRM_MAJOR
#define DRM_MAJOR 226
#endif

#define VT_HANDLER_LOGIND 1

namespace GreenIsland {

namespace Platform {

static VtHandler *vth;

/*
 * VtHandlerPrivate
 */

class VtHandlerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(VtHandler)
public:
    VtHandlerPrivate(VtHandler *self)
        : logind(Logind::instance())
        , notifier(Q_NULLPTR)
        , vtFd(-1)
        , vtNumber(0)
        , kbMode(K_OFF)
        , active(false)
    {
        vth = self;

        // Disable the cursor
        toggleTtyCursor(false);
    }

    ~VtHandlerPrivate()
    {
        toggleKeyboard(true);
        toggleTtyCursor(true);

        if (notifier) {
            ::close(signalFd[0]);
            ::close(signalFd[1]);
        }

        closeFd();
    }

    void setup(int nr)
    {
        Q_Q(VtHandler);

        // A fd already open means we are good
        if (vtFd != -1)
            return;

        // Need a valid vt number
        if (nr < 0) {
            qCWarning(lcLogind, "Invalid vt number");
            return;
        }

        // Open the tty for this vt
        devName = QStringLiteral("/dev/tty%1").arg(nr);
        vtFd = ::open(qPrintable(devName), O_RDWR | O_CLOEXEC | O_NONBLOCK);
        if (vtFd < 0) {
            qCWarning(lcLogind, "Failed to open vt \"%s\": %s",
                      qPrintable(devName), ::strerror(errno));
            return;
        }

        // Must be a valid vt
        if (!isValidVt(vtFd)) {
            qCWarning(lcLogind, "TTY \"%s\" is not a virtual terminal",
                      qPrintable(devName));
            closeFd();
            return;
        }

        // Avoid input going to the tty
        toggleKeyboard(false);

        // Graphics mode
        if (::ioctl(vtFd, KDSETMODE, KD_GRAPHICS) < 0) {
            qCWarning(lcLogind, "Unable to set KD_GRAPHICS mode on \"%s\": %s",
                      qPrintable(devName), ::strerror(errno));
            toggleKeyboard(true);
            closeFd();
            return;
        }

        // Signal handler
        if (!installSignalHandler()) {
            ::ioctl(vtFd, KDSETMODE, KD_TEXT);
            toggleKeyboard(true);
            closeFd();
            return;
        }

#if !VT_HANDLER_LOGIND
        // Take over VT
        vt_mode mode = { VT_PROCESS, 0, short(SIGRTMIN), short(SIGRTMIN), 0 };
        if (::ioctl(vtFd, VT_SETMODE, &mode) < 0) {
            qCWarning(lcLogind, "Unable to take over VT \"%s\": %s",
                      qPrintable(devName), qPrintable(::strerror(errno)));
            ::ioctl(vtFd, KDSETMODE, KD_TEXT);
            toggleKeyboard(true);
            closeFd();
            return ;
        }
#endif

        vtNumber = nr;
        setActive(true);
        Q_EMIT q->created();
    }

    bool installSignalHandler()
    {
        Q_Q(VtHandler);

#if !VT_HANDLER_LOGIND
        // SIGRTMIN is used as global VT-release signal while SIGRTMIN + 1 is
        // used as VT-acquire signal, these must be checked on runtime because
        // their exact values are unknown at compile time; verify if we
        // exceed the limit (POSIX has 32 of them)
        if (SIGRTMIN > SIGRTMAX) {
            qCWarning(lcLogind, "Not enough RT signals available: %u-%u",
                      SIGRTMIN, SIGRTMAX);
            return false;
        }
#endif

        if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalFd)) {
            qCWarning(lcLogind, "Failed to create socket pair: %s",
                      ::strerror(errno));
        }
        notifier = new QSocketNotifier(signalFd[1], QSocketNotifier::Read, q);
        q->connect(notifier, &QSocketNotifier::activated, q, [this, q] {
            if (vtFd < 0)
                return;

            notifier->setEnabled(false);

            char sigNo;
            if (QT_READ(signalFd[1], &sigNo, sizeof(sigNo)) == sizeof(sigNo)) {
                switch (sigNo) {
                case SIGINT:
                case SIGTERM:
                    Q_EMIT q->interrupted();
                    toggleKeyboard(true);
                    toggleTtyCursor(true);
                    ::_exit(1);
                    break;
                case SIGTSTP:
                    Q_EMIT q->aboutToSuspend();
                    toggleKeyboard(true);
                    toggleTtyCursor(true);
                    ::kill(::getpid(), SIGSTOP);
                    break;
                case SIGCONT:
                    toggleKeyboard(false);
                    toggleTtyCursor(false);
                    Q_EMIT q->resumed();
                    break;
                default:
                    break;
                }

#if !VT_HANDLER_LOGIND
                if (sigNo >= SIGRTMIN && sigNo <= SIGRTMAX) {
                    if (active) {
                        setActive(false);
                        ::ioctl(vtFd, VT_RELDISP, 1);
                    } else {
                        ::ioctl(vtFd, VT_RELDISP, VT_ACKACQ);
                        setActive(true);
                    }
                }
#endif
            }

            notifier->setEnabled(true);
        });

        struct sigaction sa;
        sa.sa_flags = 0;
        sa.sa_handler = signalHandler;
        ::sigemptyset(&sa.sa_mask);
        ::sigaction(SIGINT, &sa, 0);
        ::sigaction(SIGTSTP, &sa, 0);
        ::sigaction(SIGCONT, &sa, 0);
        ::sigaction(SIGTERM, &sa, 0);
#if !VT_HANDLER_LOGIND
        ::sigaction(SIGRTMIN, &sa, 0);
        ::sigaction(SIGRTMIN + 1, &sa, 0);
#endif

        return true;
    }

    void toggleTtyCursor(bool enable)
    {
        int fd = -1;

        const char *const devices[] = { "/dev/tty0", "/dev/tty", "/dev/console", Q_NULLPTR };
        for (const char *const *device = devices; *device; ++device) {
            fd = ::open(*device, O_RDWR);
            if (fd >= 0) {
                const char *escape = enable
                        ? "\033[9;15]\033[?33h\033[?25h\033[?0c"
                        : "\033[9;0]\033[?33l\033[?25l\033[?1c";
                (void)::write(fd, escape, ::strlen(escape) + 1);
                ::close(fd);
                return;
            }
        }
    }

    void toggleKeyboard(bool enable)
    {
        if (vtFd < 0)
            return;

        if (enable) {
            if (::ioctl(vtFd, KDSKBMUTE, 0) < 0)
                qCWarning(lcLogind, "Unable to unmute keyboard on \"%s\": %s",
                          qPrintable(devName), ::strerror(errno));
            if (::ioctl(vtFd, KDSKBMODE, kbMode) < 0)
                qCWarning(lcLogind, "Unable to restore keyboard mode on \"%s\": %s",
                          qPrintable(devName), ::strerror(errno));
        } else {
            // Read keyboard mode
            if (::ioctl(vtFd, KDGKBMODE, &kbMode) < 0) {
                qCWarning(lcLogind, "Unable to read keyboard mode on \"%s\": %s",
                          qPrintable(devName), ::strerror(errno));
                kbMode = K_UNICODE;
            } else if (kbMode == K_OFF) {
                kbMode = K_UNICODE;
            }

            if (!qEnvironmentVariableIntValue("GREENISLAND_QPA_ENABLE_TERMINAL_KEYBOARD")) {
                // Avoid input going to the tty
                if (::ioctl(vtFd, KDSKBMUTE, 1) < 0 &&
                        ::ioctl(vtFd, KDSKBMODE, K_OFF) < 0) {
                    qCWarning(lcLogind, "Unable to set K_OFF keyboard mode on \"%s\": %s",
                              qPrintable(devName), ::strerror(errno));
                    closeFd();
                }
            }
        }
    }

    void closeFd()
    {
        if (vtFd < 0)
            return;

        if (notifier) {
            delete notifier;
            notifier = Q_NULLPTR;

            ::close(signalFd[0]);
            ::close(signalFd[1]);
        }

        ::close(vtFd);
        vtFd = -1;
    }

    void setActive(bool v)
    {
        Q_Q(VtHandler);

        if (active == v)
            return;

        active = v;
        Q_EMIT q->activeChanged(v);
    }

    static bool isValidVt(int fd)
    {
        if (fd < 0)
            return false;

        struct stat st;
        if (::fstat(fd, &st) == -1) {
            qCWarning(lcLogind) << "Failed to fstat tty";
            return false;
        }

        if (major(st.st_rdev) != TTY_MAJOR || minor(st.st_rdev) <= 0 ||
                minor(st.st_rdev) >= 64)
            return false;

        return true;
    }

    static void signalHandler(int sigNo)
    {
        char a = sigNo;
        QT_WRITE(vth->d_func()->signalFd[0], &a, sizeof(a));
    }

    Logind *logind;

    int signalFd[2];
    QSocketNotifier *notifier;

    int vtFd;
    int vtNumber;
    QString devName;

    int kbMode;

    bool active;
};

/*
 * VtHandler
 */

VtHandler::VtHandler(QObject *parent)
    : QObject(*new VtHandlerPrivate(this), parent)
{
    Q_D(VtHandler);

    // Setup vt if we already know the number
    if (d->logind->vtNumber() != -1)
        d->setup(d->logind->vtNumber());

    // Setup vt when the number will be assigned
    connect(d->logind, &Logind::vtNumberChanged, this, [this, d](int nr) {
        d->setup(nr);
    });

#if VT_HANDLER_LOGIND
    // Vt is active when the session is
    connect(d->logind, &Logind::sessionActiveChanged, this, [this, d](bool active) {
        d->setActive(active);
    });

    // Handle pause and resume of DRM devices
    connect(d->logind, &Logind::devicePaused, this, [this, d]
            (quint32 devMajor, quint32 devMinor, const QString &type) {
        qCDebug(lcLogind, "Device with major %d minor %d paused with %s",
                devMajor, devMinor, qPrintable(type));

        if (type == QLatin1String("pause"))
            d->logind->pauseDeviceComplete(devMajor, devMinor);
    });
    connect(d->logind, &Logind::deviceResumed, this, [this, d]
            (quint32 devMajor, quint32 devMinor, int) {
        qCDebug(lcLogind, "Device with major %d minor %d resumed",
                devMajor, devMinor);
    });
#endif

    // Take control upon connection
    if (d->logind->isConnected())
        d->logind->takeControl();
    else
        connect(d->logind, &Logind::connectedChanged,
                d->logind, &Logind::takeControl);
}

bool VtHandler::isActive() const
{
    Q_D(const VtHandler);
    return d->active;
}

void VtHandler::activate(quint32 nr)
{
    Q_D(VtHandler);

    if (d->vtFd < 0)
        return;

    if (d->vtNumber == int(nr))
        return;

    qCDebug(lcLogind, "Switching to vt %d", nr);
    d->logind->switchTo(nr);
    d->setActive(false);
}

} // namespace Platform

} // namespace GreenIsland

#include "moc_vthandler.cpp"
