/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd, author: <giulio.camuffo@jollamobile.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Compositor.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDSURFACEINTERFACE_H
#define QWAYLANDSURFACEINTERFACE_H

#include <QWindow>

#include "surface.h"
#include <greenisland/greenisland_export.h>

namespace GreenIsland {

class Surface;

class GREENISLAND_EXPORT SurfaceOperation
{
public:
    enum Type {
        Close,
        SetVisibility,
        Resize,
        Ping,
        UserType = 1000
    };

    SurfaceOperation(int t);
    virtual ~SurfaceOperation();

    int type() const;

private:
    class Private;
    Private *const d;
};

class GREENISLAND_EXPORT SurfaceSetVisibilityOperation : public SurfaceOperation
{
public:
    SurfaceSetVisibilityOperation(QWindow::Visibility visibility);
    QWindow::Visibility visibility() const;

private:
    QWindow::Visibility m_visibility;
};

class GREENISLAND_EXPORT SurfaceResizeOperation : public SurfaceOperation
{
public:
    SurfaceResizeOperation(const QSize &size);
    QSize size() const;

private:
    QSize m_size;
};

class GREENISLAND_EXPORT SurfacePingOperation : public SurfaceOperation
{
public:
    SurfacePingOperation(quint32 serial);
    quint32 serial() const;

private:
    quint32 m_serial;
};

class GREENISLAND_EXPORT SurfaceInterface
{
public:
    SurfaceInterface(Surface *surf);
    virtual ~SurfaceInterface();

    Surface *surface() const;

protected:
    virtual bool runOperation(SurfaceOperation *op) = 0;

    void setSurfaceType(Surface::WindowType type);
    void setSurfaceClassName(const QString &name);
    void setSurfaceTitle(const QString &title);

private:
    class Private;
    Private *const d;
    friend class Surface;
};

}

#endif
