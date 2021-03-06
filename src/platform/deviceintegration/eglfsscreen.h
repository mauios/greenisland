/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 * Copyright (C) 2016 The Qt Company Ltd.
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:QTLGPL$
 *
 * GNU Lesser General Public License Usage
 * This file may be used under the terms of the GNU Lesser General
 * Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPLv3 included in the
 * packaging of this file. Please review the following information to
 * ensure the GNU Lesser General Public License version 3 requirements
 * will be met: https://www.gnu.org/licenses/lgpl.html.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 2.0 or (at your option) the GNU General
 * Public license version 3 or any later version approved by the KDE Free
 * Qt Foundation. The licenses are as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 and LICENSE.GPLv3
 * included in the packaging of this file. Please review the following
 * information to ensure the GNU General Public License requirements will
 * be met: https://www.gnu.org/licenses/gpl-2.0.html and
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef GREENISLAND_EGLFSSCREEN_H
#define GREENISLAND_EGLFSSCREEN_H

#include <QtGui/qpa/qplatformscreen.h>

#include <GreenIsland/platform/greenislandplatform_export.h>

#include <EGL/egl.h>

class QOpenGLContext;

namespace GreenIsland {

namespace Platform {

class EglFSWindow;

class GREENISLANDPLATFORM_EXPORT EglFSScreen : public QPlatformScreen
{
public:
    struct Mode {
        QSize size;
        qreal refreshRate;
    };

    EglFSScreen(EGLDisplay display);
    ~EglFSScreen();

    QRect geometry() const Q_DECL_OVERRIDE;
    int depth() const Q_DECL_OVERRIDE;
    QImage::Format format() const Q_DECL_OVERRIDE;

    QSizeF physicalSize() const Q_DECL_OVERRIDE;
    QDpi logicalDpi() const Q_DECL_OVERRIDE;
    qreal pixelDensity() const Q_DECL_OVERRIDE;
    Qt::ScreenOrientation nativeOrientation() const Q_DECL_OVERRIDE;
    Qt::ScreenOrientation orientation() const Q_DECL_OVERRIDE;

    QPlatformCursor *cursor() const Q_DECL_OVERRIDE;

    qreal refreshRate() const Q_DECL_OVERRIDE;

    QPixmap grabWindow(WId wid, int x, int y, int width, int height) const Q_DECL_OVERRIDE;

    EGLSurface primarySurface() const { return m_surface; }

    EGLDisplay display() const { return m_dpy; }

    void setPrimarySurface(EGLSurface surface);

    void handleCursorMove(const QPoint &pos);

    virtual QList<Mode> modes() const;

    virtual int currentMode() const;
    virtual void setCurrentMode(int modeId);

    virtual int preferredMode() const;
    virtual void setPreferredMode(int modeId);

    virtual QString identifier() const;
    virtual QString manufacturer() const;
    virtual QString model() const;
    virtual QString serialNumber() const;

private:
    EGLDisplay m_dpy;
    QWindow *m_pointerWindow;
    EGLSurface m_surface;
    QPlatformCursor *m_cursor;
};

} // namespace Platform

} // namespace GreenIsland

#endif // GREENISLAND_EGLFSSCREEN_H
