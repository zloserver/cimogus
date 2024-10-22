import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts

import PageEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Config"
import "../Components"

BusyIndicator {
    id: busyIndicator

    Layout.alignment: Qt.AlignHCenter
    Layout.bottomMargin: 32

    visible: true
    running: true

    contentItem: Item {
        implicitWidth: 46
        implicitHeight: 46
        transformOrigin: Item.Center

        Shape {
            id: shape
            width: parent.implicitWidth
            height: parent.implicitHeight
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            layer.enabled: true
            layer.samples: 4

            ShapePath {
                fillColor: AmneziaStyle.color.transparent
                strokeColor: AmneziaStyle.color.charcoalGray
                strokeWidth: 3
                capStyle: ShapePath.RoundCap

                PathAngleArc {
                    centerX: shape.width / 2
                    centerY: shape.height / 2
                    radiusX: 18
                    radiusY: 18
                    startAngle: 225
                    sweepAngle: -90
                }
            }
            RotationAnimator {
                target: shape
                running: busyIndicator.visible && busyIndicator.running
                from: 0
                to: 360
                loops: Animation.Infinite
                duration: 1250
            }
        }
    }
}
