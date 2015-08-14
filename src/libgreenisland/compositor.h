/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include "abstractquickcompositor.h"
#include "surfaceitem.h"

#include <greenisland/greenisland_export.h>

namespace GreenIsland {

class CompositorSettings;
class ClientWindow;
class CompositorPrivate;
class HomeApplication;
class Output;
class ScreenManager;
class ShellWindow;
class GtkSurface;
class WlCursorTheme;
class WlCursorThemePrivate;
class WlShellSurfaceMoveGrabber;
class WlShellSurfaceResizeGrabber;
class WlSubSurface;
class XdgSurfaceMoveGrabber;
class XdgSurfaceResizeGrabber;

class GREENISLAND_EXPORT Compositor : public QObject, public AbstractQuickCompositor
{
    Q_OBJECT
    Q_PROPERTY(CompositorSettings *settings READ settings CONSTANT)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(int idleInterval READ idleInterval WRITE setIdleInterval NOTIFY idleIntervalChanged)
    Q_PROPERTY(int idleInhibit READ idleInhibit WRITE setIdleInhibit NOTIFY idleInhibitChanged)
    Q_PRIVATE_PROPERTY(Compositor::d_func(), QQmlListProperty<ClientWindow> windows READ windows NOTIFY windowsChanged)
    Q_PRIVATE_PROPERTY(Compositor::d_func(), QQmlListProperty<ShellWindow> shellWindows READ shellWindows NOTIFY shellWindowsChanged)
    Q_ENUMS(State)
public:
    enum State {
        //! Compositor is active.
        Active,
        //! Compositor is idle.
        Idle
    };

    virtual ~Compositor();

    static Compositor *instance();

    bool isRunning() const;
    bool isNested() const;
    QString shell() const;
    QString fakeScreenConfiguration() const;
    QQmlEngine *engine() const;

    State state() const;
    void setState(State state);

    int idleInterval() const;
    void setIdleInterval(int value);

    int idleInhibit() const;
    void setIdleInhibit(int value);

    CompositorSettings *settings() const;

    void run();

    Q_INVOKABLE SurfaceView *pickView(const QPointF &globalPosition) const Q_DECL_OVERRIDE;
    Q_INVOKABLE SurfaceItem *firstViewOf(Surface *surface);
    Q_INVOKABLE SurfaceItem *subSurfaceForOutput(Surface *surface,
                                                         Output *output) const;

    void surfaceCreated(Surface *surface) Q_DECL_OVERRIDE;
    SurfaceView *createView(Surface *surf) Q_DECL_OVERRIDE;

    Q_INVOKABLE void clearKeyboardFocus();
    Q_INVOKABLE void restoreKeyboardFocus();

    Q_INVOKABLE void unsetMouseCursor();
    Q_INVOKABLE void resetMouseCursor();

    Q_INVOKABLE void abortSession();

public Q_SLOTS:
    void incrementIdleInhibit();
    void decrementIdleInhibit();

Q_SIGNALS:
    void newSurfaceCreated(Surface *surface);
    void surfaceMapped(Surface *surface);
    void surfaceUnmapped(Surface *surface);
    void surfaceDestroyed(Surface *surface);

    void windowMapped(ClientWindow *window);
    void windowUnmapped(ClientWindow *window);
    void windowDestroyed(uint id);

    void shellWindowMapped(ShellWindow *window);
    void shellWindowUnmapped(ShellWindow *window);
    void shellWindowDestroyed(uint id);

    void stateChanged();
    void idleIntervalChanged();
    void idleInhibitChanged();
    void windowsChanged();
    void shellWindowsChanged();

    void lockedChanged();

    void idle();
    void wake();

    void fadeIn();
    void fadeOut();

    void screenConfigurationAcquired();

    void outputAdded(Output *output);
    void outputRemoved(Output *output);

protected:
    Compositor(QObject *parent = 0);

    void setCursorSurface(Surface *surface, int hotspotX, int hotspotY) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(Compositor)
    CompositorPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_createNestedConnection())
    Q_PRIVATE_SLOT(d_func(), void _q_createInternalConnection())

    friend class ClientWindow;
    friend class HomeApplication;
    friend class ScreenManager;
    friend class ShellWindow;
    friend class OutputWindow;
    friend class GtkSurface;
    friend class WlCursorTheme;
    friend class WlCursorThemePrivate;
    friend class WlShellSurfaceMoveGrabber;
    friend class WlShellSurfaceResizeGrabber;
    friend class WlSubSurface;
    friend class XdgSurfaceMoveGrabber;
    friend class XdgSurfaceResizeGrabber;
};

}

#endif // COMPOSITOR_H
