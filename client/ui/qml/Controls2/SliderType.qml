import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Style 1.0

import "../Controls2/TextTypes"

RowLayout {
    id: root
    property real minValue: 1
    property real maxValue: 6

    property alias value: slider.value
    property alias slider: slider

    spacing: 12
    LabelTextType {
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: 16

        text: root.minValue
        color: AmneziaStyle.color.lightGray
    }

    Slider {
        id: slider
        Layout.topMargin: -4
        Layout.bottomMargin: -4
        Layout.leftMargin: -8
        Layout.rightMargin: -8
        Layout.fillWidth: true

        handle.implicitWidth: 20
        handle.implicitHeight: 20

        from: root.minValue
        to: root.maxValue
        value: root.value
        stepSize: 1
        snapMode: Slider.SnapAlways
    }

    LabelTextType {
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: 16

        text: root.maxValue
        color: AmneziaStyle.color.lightGray
    }
}
