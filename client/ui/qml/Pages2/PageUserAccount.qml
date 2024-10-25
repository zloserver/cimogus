import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import PageEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Config"

import UserInfo 1.0

PageType {
    id: root

    Popup {
        property real monthsToAdd: 2

        id: blockingPopup
        anchors.centerIn: Overlay.overlay
        background: Rectangle {
            radius: 16
            color: Qt.rgba(14/255, 14/255, 17/255, 0.8)
            border.color: "transparent"
        }

        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 100 }
        }
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
        }

        modal: true
        focus: true
        closePolicy: Popup.CloseOnPressOutside

        padding: 20
        property int margin: 32
        property int maxWidth: 380
        width: Math.min(parent.width - margin, maxWidth)

        ColumnLayout {
            id: popupContent
            width: parent.width
            spacing: 16

            Header2Type {
                headerText: qsTr("Add Balance")
            }

            ColumnLayout {
                Layout.fillWidth: true
                visible: AuthController.userInfo.monthsAvailableToAdd() > 1

                SliderType {
                    id: monthsSlider

                    minValue: 1
                    maxValue: AuthController.userInfo.monthsAvailableToAdd()
                    value: blockingPopup.monthsToAdd
                    slider.onValueChanged: {
                        blockingPopup.monthsToAdd = monthsSlider.value
                    }
                }

                LabelTextType {
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 16

                    text: qsTr("Months")
                    color: AmneziaStyle.color.mutedGray
                }
            }

            function localizeMonths(count) {
                if (count == 1) return qsTr("Buy %1 month").arg(count);
                const lastDigit = count % 10;
                if (lastDigit == 2 || lastDigit == 3 || lastDigit == 4) return qsTr("Buy %1 months", "2,3,4").arg(count);
                return qsTr("Buy %1 months", ">=5").arg(count);
            }

            BasicButtonType {
                id: buyButton

                Layout.fillWidth: true
                text: popupContent.localizeMonths(blockingPopup.monthsToAdd)

                onClicked: {
                    blockingPopup.close()
                    PageController.showBusyIndicator(true)
                    AuthController.addBalance(blockingPopup.monthsToAdd)
                }
            }
        }
    }

    RowLayout {
        id: topStrip
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left

        HeaderType {
            Layout.fillWidth: true
            Layout.topMargin: 24
            Layout.rightMargin: 16
            Layout.leftMargin: 16

            headerText: qsTr("Profile")
        }

        ImageButtonType {
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.topMargin: 24
            Layout.rightMargin: 16

            image: "qrc:/images/controls/logout.svg"
            imageColor: AmneziaStyle.color.paleGray

            implicitWidth: 40
            implicitHeight: 40

            onClicked: {
                AuthController.logout()
                PageController.goToPage(PageEnum.PageSetupWizardStart)
            }
        }
    }

    FlickableType {
        id: fl
        anchors.top: topStrip.bottom
        anchors.bottom: bottomStrip.top
        // contentItem: content
        // contentHeight: content.height

        ColumnLayout {
            id: content

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left

            spacing: 0

            LabelWithButtonType {
                Layout.fillWidth: true

                text: qsTr("Username")
                descriptionText: AuthController.userInfo.username

                leftImageSource: "qrc:/images/controls/user.svg"
                shouldBeWide: true
            }

            RowLayout {
                Layout.fillWidth: true

                LabelWithButtonType {
                    Layout.fillWidth: true

                    text: qsTr("Email")
                    descriptionText: AuthController.userInfo.isValid ? AuthController.userInfo.email : qsTr("...")

                    leftImageSource: "qrc:/images/controls/mail.svg"
                    shouldBeWide: true
                }

                BasicButtonType {
                    Layout.rightMargin: 16
                    Layout.leftMargin: 16
                    Layout.minimumWidth: 48
                    text: qsTr("Edit")

                    defaultColor: AmneziaStyle.color.transparent
                    hoveredColor: AmneziaStyle.color.translucentWhite
                    pressedColor: AmneziaStyle.color.sheerWhite
                    disabledColor: AmneziaStyle.color.mutedGray
                    textColor: AmneziaStyle.color.mutedGray

                    clickedFunc: function() {
                        PageController.goToPage(PageEnum.PageChangeEmail)
                    }
                }
            }

            LabelWithButtonType {
                Layout.fillWidth: true

                text: qsTr("Change password")
                leftImageSource: "qrc:/images/controls/password.svg"
                rightImageSource: "qrc:/images/controls/chevron-right.svg"

                clickedFunction: function() {
                    PageController.goToPage(PageEnum.PageChangePassword)
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Rectangle { anchors.fill: parent; color: "#ffaaaa" }
            }
        }
    }

    ColumnLayout {
        id: bottomStrip
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.left: parent.left

        DividerType {}

        RowLayout {
            Layout.fillWidth: true

            LabelWithButtonType {
                function uppercaseFirst(x) {
                    return x.charAt(0).toUpperCase() + x.slice(1);
                }

                Layout.fillWidth: true

                text: qsTr("Balance")
                descriptionText: AuthController.userInfo.isValid ? uppercaseFirst(AuthController.userInfo.localizedTimeLeft) : qsTr("...")

                leftImageSource: "qrc:/images/controls/balance.svg"
                shouldBeWide: true
            }

            BasicButtonType {
                Layout.rightMargin: 16
                Layout.leftMargin: 16
                Layout.minimumWidth: 90
                text: qsTr("Add")

                onClicked: {
                    if (AuthController.userInfo.monthsAvailableToAdd() == 0) {
                        PageController.showErrorMessage(qsTr("You can't add more balance yet."))
                        return;
                    }

                    blockingPopup.open()
                }
            }
        }
    }

    Connections {
        target: AuthController

        function onErrorOccurred(error) {
            PageController.showBusyIndicator(false)
            PageController.showErrorMessage(error)
        }

        function onAddBalanceOpened() {
            PageController.showBusyIndicator(false)
        }
    }
    Component.onCompleted: {
        AuthController.refreshUserInfo()
    }
}
