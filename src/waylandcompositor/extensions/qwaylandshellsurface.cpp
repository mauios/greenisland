/****************************************************************************
**
** Copyright (C) 2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwaylandshellsurface.h"
#include "qwaylandshellsurface_p.h"

QT_BEGIN_NAMESPACE

QWaylandShellSurfacePrivate::QWaylandShellSurfacePrivate()
    : focusPolicy(QWaylandShellSurface::AutomaticFocus)
{
}

QWaylandShellSurface::QWaylandShellSurface(QWaylandObject *waylandObject)
    : QWaylandCompositorExtension(waylandObject, *new QWaylandShellSurfacePrivate())
{
}

/*!
 * \qmlproperty enum QtWaylandCompositor::ShellSurface::focusPolicy
 *
 * This property holds the focus policy of the ShellSurface.
 */

/*!
 * \enum QWaylandShellSurface::FocusPolicy
 *
 * This enum type is used to specify the focus policy of a shell surface.
 *
 * \value AutomaticFocus The shell surface will automatically get keyboard focus.
 * \value ManualFocus The compositor will decide whether the shell surface should get keyboard focus or not.
 */

/*!
 * \qmlproperty object QtWaylandCompositor::ShellSurface::focusPolicy
 *
 * This property holds the focus policy of the ShellSurface.
 */

/*!
 * \property QWaylandShellSurface::focusPolicy
 *
 * This property holds the focus policy of the QWaylandShellSurface.
 */
QWaylandShellSurface::FocusPolicy QWaylandShellSurface::focusPolicy() const
{
    Q_D(const QWaylandShellSurface);
    return d->focusPolicy;
}

void QWaylandShellSurface::setFocusPolicy(QWaylandShellSurface::FocusPolicy focusPolicy)
{
    Q_D(QWaylandShellSurface);

    if (d->focusPolicy == focusPolicy)
        return;

    d->focusPolicy = focusPolicy;
    emit focusPolicyChanged();
}

QT_END_NAMESPACE