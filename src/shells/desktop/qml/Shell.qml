/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (c) 2012 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import GreenIsland 1.0
import FluidCore 1.0

Item {
    id: root

    // Available geometry
    property rect availableGeometry: Qt.rect(x, y, width, height)

    // AppChooser
    property var appChooser: appChooserObject

    // Notifications panel
    property var notifications: notificationsPanel

    // Recalculate geometry when the size changes
    onWidthChanged: changeWidth()
    onHeightChanged: changeHeight()

    // Panel
    Loader {
        id: panelComponent
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        z: 2
        source: "Panel.qml"
        asynchronous: true
        onLoaded: {
            // Recalculate geometry
            calculateGeometry();
        }

        // Animate Panel when it shows up
        Behavior on height {
            NumberAnimation { easing.type: Easing.InQuad; duration: 350 }
        }
    }

    // Launcher
    Loader {
        id: launcherComponent
        z: 2
        source: "Launcher.qml"
        asynchronous: true
        onLoaded: {
            // Recalculate geometry
            calculateGeometry();
        }

        // Animate Launcher when it shows up
        Behavior on width {
            NumberAnimation { easing.type: Easing.InQuad; duration: 350 }
        }
        Behavior on height {
            NumberAnimation { easing.type: Easing.InQuad; duration: 350 }
        }

        states: [
            State {
                name: "left"
                when: launcherComponent.item.alignment === LauncherAlignment.Left

                AnchorChanges {
                    target: launcherComponent
                    anchors.left: root.left
                }
                PropertyChanges {
                    target: launcherComponent
                    y: panelComponent.height
                    width: item.launcherSize
                    height: root.height - y
                }
            },
            State {
                name: "right"
                when: launcherComponent.item.alignment === LauncherAlignment.Right

                AnchorChanges {
                    target: launcherComponent
                    anchors.right: root.right
                }
                PropertyChanges {
                    target: launcherComponent
                    y: panelComponent.height
                    width: item.launcherSize
                    height: root.height - y
                }
            },
            State {
                name: "bottom"
                when: launcherComponent.item.alignment === LauncherAlignment.Bottom

                AnchorChanges {
                    target: launcherComponent
                    anchors.bottom: root.bottom
                    anchors.horizontalCenter: root.horizontalCenter
                }
                PropertyChanges {
                    target: launcherComponent
                    width: root.width
                    height: item.launcherSize
                }
            }
        ]
    }

    // Active top left corner
    MouseArea {
        id: topLeftCorner
        x: root.x
        y: root.y
        width: 16
        height: 16
        hoverEnabled: true
    }

    // Active top right corner
    MouseArea {
        id: topRightCorner
        x: root.width - 16
        y: root.y
        width: 16
        height: 16
        hoverEnabled: true
    }

    // Active bottom left corner
    MouseArea {
        id: bottomLeftCorner
        x: root.x
        y: root.height - 16
        width: 16
        height: 16
        hoverEnabled: true
    }

    // Active bottom right corner
    MouseArea {
        id: bottomRightCorner
        x: root.width - 16
        y: root.y
        width: 16
        height: 16
        hoverEnabled: true
    }

    // Area that contains windows
    MouseArea {
        id: availableArea
        x: availableGeometry.x
        y: availableGeometry.y
        width: availableGeometry.width
        height: availableGeometry.height
        onClicked: {
            // A click outside the shell closes AppChooser
            appChooserObject.close();
        }
    }

    // Application chooser
    AppChooser {
        id: appChooserObject
        width: availableGeometry.width / 1.2
        height: availableGeometry.height / 1.2

        // Animate when it shows up
        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    // Notifications panel
    NotificationsPanel {
        id: notificationsPanel
        x: availableGeometry.width
        y: availableGeometry.y
        z: 3
        width: availableGeometry.width / 10
        height: availableGeometry.height
        visible: false

        // Animation
        Behavior on x {
            NumberAnimation { duration: 200 }
        }

        function show() {
            visible = true;
            x = availableGeometry.width - width;
        }

        function hide() {
            x = availableGeometry.width;
        }
    }

    function calculateGeometry() {
        // Available geometry equals screen geometry
        var geometry = Qt.rect(x, y, width, height);

        // ...unless the panel is loaded
        if (panelComponent.status == Loader.Ready) {
            panelComponent.height = panelComponent.item.panelHeight;
            geometry.y = panelComponent.item.panelHeight;
            geometry.height -= panelComponent.item.panelHeight;
        }

        // ...or the launcher is
        if (launcherComponent.status == Loader.Ready) {
            switch (launcherComponent.item.alignment) {
            case LauncherAlignment.Left:
                geometry.x = launcherComponent.item.launcherSize;
                geometry.width -= launcherComponent.item.launcherSize;
                break;
            case LauncherAlignment.Right:
                geometry.x = geometry.width - launcherComponent.item.launcherSize;
                geometry.width -= launcherComponent.item.launcherSize;
                break;
            default:
                geometry.height -= launcherComponent.item.launcherSize;
                break;
            }
        }

        // Set the available geometry to the result of the above computation
        availableGeometry = geometry;
        console.log("Available geometry is now", availableGeometry);

        // Update the compositor only when both Panel and Launcher are ready
        if (panelComponent.status == Loader.Ready && launcherComponent.status == Loader.Ready) {
            console.log("Sending the available geometry to the compositor...");
            shell.updateAvailableGeometry();
        }
    }
}
