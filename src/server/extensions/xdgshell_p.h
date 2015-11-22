/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef GREENISLAND_XDGSHELL_P_H
#define GREENISLAND_XDGSHELL_P_H

#include <GreenIsland/Server/XdgShell>
#include <GreenIsland/QtWaylandCompositor/private/qwaylandextension_p.h>

#include <GreenIsland/server/private/qwayland-server-xdg-shell.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace GreenIsland {

namespace Server {

typedef QPointer<XdgSurface> XdgSurfacePtr;

class GREENISLANDSERVER_EXPORT XdgShellPrivate
        : public QWaylandExtensionTemplatePrivate
        , public QtWaylandServer::xdg_shell
{
    Q_DECLARE_PUBLIC(XdgShell)
public:
    XdgShellPrivate();

    bool isPopupInitialUp() const { return m_initialUp; }
    void setPopupInitialUp(bool up) { m_initialUp = up; }

    void ping(XdgSurface *surface);
    void ping(uint32_t serial, XdgSurface *surface);

    void addPopup(XdgPopup *popup, QWaylandInputDevice *inputDevice);
    void removePopup(XdgPopup *popup);

    static XdgShellPrivate *get(XdgShell *shell) { return shell->d_func(); }

protected:
    void shell_destroy(Resource *resource) Q_DECL_OVERRIDE;
    void shell_use_unstable_version(Resource *resource, int32_t version) Q_DECL_OVERRIDE;
    void shell_get_xdg_surface(Resource *resource, uint32_t id,
                               wl_resource *surfaceResource) Q_DECL_OVERRIDE;
    void shell_get_xdg_popup(Resource *resource, uint32_t id, wl_resource *surfaceResource,
                             wl_resource *parentResource, wl_resource *seatResource,
                             uint32_t serial, int32_t x, int32_t y) Q_DECL_OVERRIDE;
    void shell_pong(Resource *resource, uint32_t serial) Q_DECL_OVERRIDE;

private:
    QMap<uint32_t, XdgSurfacePtr> m_pings;

    QVector<XdgPopup *> m_popups;
    wl_client *m_popupClient;
    bool m_initialUp;
};

class GREENISLANDSERVER_EXPORT XdgSurfacePrivate
        : public QWaylandExtensionTemplatePrivate
        , public QtWaylandServer::xdg_surface
{
    Q_DECLARE_PUBLIC(XdgSurface)
public:
    struct PendingChange {
        PendingChange()
            : maximized(false)
            , fullScreen(false)
            , resizing(false)
            , activated(false)
            , size(QSize())
            , output(Q_NULLPTR)
        {}

        PendingChange(const PendingChange &other)
            : maximized(other.maximized)
            , fullScreen(other.fullScreen)
            , resizing(other.resizing)
            , activated(other.activated)
            , size(other.size)
            , output(other.output)
        {}

        bool maximized;
        bool fullScreen;
        bool resizing;
        bool activated;
        QSize size;
        QWaylandOutput *output;
    };

    XdgSurfacePrivate();
    ~XdgSurfacePrivate();

    void sendActivate();
    void sendConfigure(const PendingChange &change);

    static XdgSurfacePrivate *get(XdgSurface *surface) { return surface->d_func(); }

protected:
    void surface_destroy_resource(Resource *resource) Q_DECL_OVERRIDE;
    void surface_destroy(Resource *resource) Q_DECL_OVERRIDE;

    void surface_set_parent(Resource *resource,
                            wl_resource *parentResource) Q_DECL_OVERRIDE;

    void surface_set_title(Resource *resource,
                           const QString &title) Q_DECL_OVERRIDE;
    void surface_set_app_id(Resource *resource,
                            const QString &app_id) Q_DECL_OVERRIDE;

    void surface_show_window_menu(Resource *resource, wl_resource *seat,
                                  uint32_t serial, int32_t x, int32_t y) Q_DECL_OVERRIDE;

    void surface_move(Resource *resource, wl_resource *seat,
                      uint32_t serial) Q_DECL_OVERRIDE;
    void surface_resize(Resource *resource, wl_resource *seat, uint32_t serial,
                        uint32_t edges) Q_DECL_OVERRIDE;

    void surface_ack_configure(Resource *resource,
                               uint32_t serial) Q_DECL_OVERRIDE;
    void surface_set_window_geometry(Resource *resource, int32_t x, int32_t y,
                                     int32_t width, int32_t height) Q_DECL_OVERRIDE;

    void surface_set_maximized(Resource *resource) Q_DECL_OVERRIDE;
    void surface_unset_maximized(Resource *resource) Q_DECL_OVERRIDE;

    void surface_set_fullscreen(Resource *resource,
                                wl_resource *outputResource) Q_DECL_OVERRIDE;
    void surface_unset_fullscreen(Resource *resource) Q_DECL_OVERRIDE;

    void surface_set_minimized(Resource *resource) Q_DECL_OVERRIDE;

private:
    XdgShell *m_shell;
    QWaylandSurface *m_surface;

    QString m_title;
    QString m_appId;

    QSize m_maximizedSize;
    QSize m_fullScreenSize;

    QRect m_windowGeometry;

    bool m_active;
    bool m_transient;
    bool m_maximized;
    bool m_fullScreen;

    QMap<uint32_t, PendingChange> m_pendingChanges;
};

class GREENISLANDSERVER_EXPORT XdgPopupPrivate
        : public QWaylandExtensionTemplatePrivate
        , public QtWaylandServer::xdg_popup
{
    Q_DECLARE_PUBLIC(XdgPopup)
public:
    XdgPopupPrivate();
    ~XdgPopupPrivate();

    XdgShell *getShell() const { return m_shell; }

    static XdgPopupPrivate *get(XdgPopup *popup) { return popup->d_func(); }

protected:
    void popup_destroy_resource(Resource *resource) Q_DECL_OVERRIDE;
    void popup_destroy(Resource *resource) Q_DECL_OVERRIDE;

private:
    XdgShell *m_shell;
    QWaylandSurface *m_surface;
    QWaylandInputDevice *m_inputDevice;
};

} // namespace Server

} // namespace GreenIsland

#endif // GREENISLAND_XDGSHELL_P_H
