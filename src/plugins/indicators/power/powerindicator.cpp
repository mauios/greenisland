/****************************************************************************
 * This file is part of Indicators.
 *
 * Copyright (c) Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QIcon>
#include <QTimer>
#include <QWindow>
#include <QDebug>

#include "powerindicator.h"
#include "powerview.h"

PowerIndicator::PowerIndicator()
    : VIndicator("power")
{
    // Update the label now
    m_index = 0;
    m_list << "weather-storm" << "weather-snow" << "weather-showers";
    updateLabel();

    // Update the label every second
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLabel()));
    timer->start(2000);

    // Create the view
    m_window = new QWindow();
    m_view = new PowerView(m_window);
}

PowerIndicator::~PowerIndicator()
{
    delete m_view;
    delete m_window;
}

void PowerIndicator::showView()
{
    m_view->show();
}

void PowerIndicator::hideView()
{
    m_view->hide();
}

void PowerIndicator::updateLabel()
{
    setLabel("ciao");
    if (m_index >= m_list.size()) m_index = 0;
    setIconName(m_list.at(m_index));
    m_index++;
}

#include "moc_powerindicator.cpp"
