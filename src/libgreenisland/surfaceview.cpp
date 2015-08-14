/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd, author: <giulio.camuffo@jollamobile.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "surfaceview.h"
#include "surface.h"
#include "surface_p.h"
#include "abstractcompositor.h"
#include "inputdevice.h"

namespace GreenIsland {

class SurfaceViewPrivate
{
public:
    Surface *surface;
    QPointF pos;
};

SurfaceView::SurfaceView(Surface *surf)
                   : d(new SurfaceViewPrivate)
{
    d->surface = surf;
    if (surf) {
        surf->d_func()->views << this;
        surf->ref();
        emit surf->viewAdded(this);
    }
}

SurfaceView::~SurfaceView()
{
    if (d->surface) {
        InputDevice *i = d->surface->compositor()->defaultInputDevice();
        if (i->mouseFocus() == this)
            i->setMouseFocus(Q_NULLPTR, QPointF());

        d->surface->destroy();
        d->surface->d_func()->views.removeOne(this);
        emit d->surface->viewRemoved(this);
    }
    delete d;
}

Surface *SurfaceView::surface() const
{
    return d->surface;
}

AbstractCompositor *SurfaceView::compositor() const
{
    return d->surface ? d->surface->compositor() : 0;
}

void SurfaceView::setPos(const QPointF &pos)
{
    d->pos = pos;
}

QPointF SurfaceView::pos() const
{
    return d->pos;
}

}
