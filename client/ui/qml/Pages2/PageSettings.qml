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

PageType {
    id: root

    defaultActiveFocusItem: header

    FlickableType {
        id: fl
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        contentHeight: content.height

        ColumnLayout {
            id: content

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 0

            HeaderType {
                id: header
                Layout.fillWidth: true
                Layout.topMargin: 24
                Layout.rightMargin: 16
                Layout.leftMargin: 16

                headerText: qsTr("Settings")

                KeyNavigation.tab: connection.rightButton
            }


            LabelWithButtonType {
                id: autoUpdate
                visible: AutoUpdateController.supportsAutoUpdates
                Layout.fillWidth: true

                text: qsTr("Check for updates")
                rightImageSource: "qrc:/images/controls/chevron-right.svg"
                leftImageSource: "qrc:/images/controls/download.svg"

                clickedFunction: function() {
                    AutoUpdateController.checkForUpdates();
                }

                KeyNavigation.tab: application.rightButton
            }

            DividerType {}

            LabelWithButtonType {
                id: connection
                Layout.fillWidth: true

                text: qsTr("Connection")
                rightImageSource: "qrc:/images/controls/chevron-right.svg"
                leftImageSource: "qrc:/images/controls/radio.svg"

                clickedFunction: function() {
                    PageController.goToPage(PageEnum.PageSettingsConnection)
                }

                KeyNavigation.tab: application.rightButton
            }

            DividerType {}

            LabelWithButtonType {
                id: application
                Layout.fillWidth: true

                text: qsTr("Application")
                rightImageSource: "qrc:/images/controls/chevron-right.svg"
                leftImageSource: "qrc:/images/controls/app.svg"

                clickedFunction: function() {
                    PageController.goToPage(PageEnum.PageSettingsApplication)
                }

                KeyNavigation.tab: about.rightButton
            }

            DividerType {}

            LabelWithButtonType {
                id: about
                Layout.fillWidth: true

                text: qsTr("About ZloVPN")
                rightImageSource: "qrc:/images/controls/chevron-right.svg"
                leftImageSource: "qrc:/images/controls/info.svg"

                clickedFunction: function() {
                    PageController.goToPage(PageEnum.PageSettingsAbout)
                }
                KeyNavigation.tab: close

            }

            DividerType {}

            LabelWithButtonType {
                id: devConsole
                visible: SettingsController.isDevModeEnabled
                Layout.fillWidth: true

                text: qsTr("Dev console")
                rightImageSource: "qrc:/images/controls/chevron-right.svg"
                leftImageSource: "qrc:/images/controls/bug.svg"

                // Keys.onTabPressed: lastItemTabClicked(header)

                clickedFunction: function() {
                    PageController.goToPage(PageEnum.PageDevMenu)
                }
            }

            DividerType {
                visible: SettingsController.isDevModeEnabled
            }

            LabelWithButtonType {
                id: close
                visible: GC.isDesktop()
                Layout.fillWidth: true
                Layout.preferredHeight: about.height

                text: qsTr("Close application")
                leftImageSource: "qrc:/images/controls/x-circle.svg"
                isLeftImageHoverEnabled: false                

                Keys.onTabPressed: lastItemTabClicked(header)

                clickedFunction: function() {
                    PageController.closeApplication()
                }
            }

            DividerType {
                visible: GC.isDesktop()
            }
        }
    }
}
