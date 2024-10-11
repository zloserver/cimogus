import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SortFilterProxyModel 0.2

import PageEnum 1.0
import ProtocolEnum 1.0
import ContainerProps 1.0
import ContainersModelFilters 1.0
import Style 1.0
import Errors 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Config"
import "../Components"

PageType {
    id: root

    defaultActiveFocusItem: emailField.textField

    property string email
    property string username
    property string password
    property string passwordConfirmation

    property bool emailErrorVisible: false
    property string emailError

    property bool usernameErrorVisible: false
    property string usernameError

    property bool passwordErrorVisible: false
    property string passwordError

    Item {
        id: loginPage
        anchors.fill: parent

        ColumnLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            spacing: 16

            RowLayout {
                BackButtonType {
                    id: backButton
                    Layout.topMargin: 20

                    KeyNavigation.tab: emailField.textField

                    backButtonFunction: {
                        PageController.closePage()
                    }
                }

                HeaderType {
                    Layout.fillWidth: true
                    Layout.topMargin: 24
                    Layout.rightMargin: 16
                    Layout.leftMargin: 16

                    headerText: qsTr("Register")
                }
            }

            TextFieldWithHeaderType {
                id: emailField

                Layout.fillWidth: true
                headerText: qsTr("Email")

                textFieldText: root.email

                KeyNavigation.tab: usernameField.textField
            }
            Binding { root.email: emailField.textField.text }
            Header2TextType {
                id: emailFieldError

                color: AmneziaStyle.color.vibrantRed
                font.pixelSize: 14
                font.weight: 500
                visible: root.emailErrorVisible

                text: root.emailError
            }

            TextFieldWithHeaderType {
                id: usernameField

                Layout.fillWidth: true
                headerText: qsTr("Username")

                textFieldText: root.username

                KeyNavigation.tab: passwordField.textField
            }
            Binding { root.username: usernameField.textField.text }
            Header2TextType {
                id: usernameFieldError

                color: AmneziaStyle.color.vibrantRed
                font.pixelSize: 14
                font.weight: 500
                visible: root.usernameErrorVisible

                text: root.usernameError
            }

            TextFieldWithHeaderType {
                id: passwordField

                Layout.fillWidth: true
                headerText: qsTr("Password")

                textFieldText: root.password
                echoMode: TextInput.Password

                KeyNavigation.tab: passwordConfirmationField.textField
            }
            Binding { root.password: passwordField.textField.text }
            Header2TextType {
                id: passwordFieldError

                color: AmneziaStyle.color.vibrantRed
                font.pixelSize: 14
                font.weight: 500
                visible: root.passwordErrorVisible

                text: root.passwordError
            }

            TextFieldWithHeaderType {
                id: passwordConfirmationField

                Layout.fillWidth: true
                headerText: qsTr("Password confirmation")

                textFieldText: root.passwordConfirmation
                echoMode: TextInput.Password


                textField.onTextEdited: {
                    if (textField.text !== root.password) {
                        passwordMatchError.opacity = 1
                        registerButton.enabled = false
                    } else {
                        passwordMatchError.opacity = 0
                        registerButton.enabled = true
                    }
                }

                KeyNavigation.tab: registerButton
            }
            Binding { root.passwordConfirmation: passwordConfirmationField.textField.text }

            Header2TextType {
                id: passwordMatchError

                color: AmneziaStyle.color.vibrantRed
                font.pixelSize: 14
                font.weight: 500
                opacity: 0

                text: qsTr("Passwords do not match")

                Behavior on opacity {
                    PropertyAnimation { duration: 200 }
                }
            }

            BasicButtonType {
                id: registerButton

                Layout.fillWidth: true
                text: qsTr("Register")

                clickedFunc: function() {
                    PageController.showBusyIndicator(true)
                    AuthController.registerUser(root.email, root.username, root.password)
                }
            }
        }

        PageStart {
            id: startPage
            width: root.width
            height: root.height
            visible: false
        }
    }

    Connections {
        target: AuthController

        function onErrorOccurredQml(errorString, errors) {
            PageController.showBusyIndicator(false)
            root.emailErrorVisible = errors["email"] != null
            if (root.emailErrorVisible) root.emailError = errors["email"]

            root.usernameErrorVisible = errors["username"] != null
            if (root.usernameErrorVisible) root.usernameError = errors["username"]

            root.passwordErrorVisible = errors["password"] != null
            if (root.passwordErrorVisible) root.passwordError = errors["password"]
        }

        function onRegisterSuccessfull() {
            PageController.showBusyIndicator(false)
            PageController.goToPageHome()
        }
    }
}
