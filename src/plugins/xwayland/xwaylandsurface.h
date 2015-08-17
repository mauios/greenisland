/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef XWAYLANDSURFACE_H
#define XWAYLANDSURFACE_H

#include "surfaceinterface.h"

#include "xwaylandwindow.h"

namespace GreenIsland {

class ClientWindow;

class XWaylandSurface : public QObject, public SurfaceInterface
{
public:
    XWaylandSurface(XWaylandWindow *window);
    ~XWaylandSurface();

    inline ClientWindow *clientWindow() const {
        return m_window;
    }

    void setAppId(const QString &id);
    void setTitle(const QString &title);
    void setType(const Surface::WindowType &type);

protected:
    bool runOperation(SurfaceOperation *op) Q_DECL_OVERRIDE;

private:
    ClientWindow *m_window;
};

}

#endif // XWAYLANDSURFACE_H