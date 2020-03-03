import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0
import QtWebKit 3.0
/*import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Styles 2.2
import QtQuick.VirtualKeyboard.Settings 2.2
*/

ApplicationWindow {
    id: window
    visible: true
    visibility: "FullScreen"
    width: Screen.width
    height: Screen.height
    title: qsTr("net")

    property bool altPressed: false

    TextField {
        id: urlbar
        text: webview.url
        placeholderText: "Please enter an URL"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: -urlbar.height - 16
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        height: 32
        /*leftPadding: 8
        rightPadding: 8
        topPadding: 8
        bottomPadding: 8*/
        horizontalAlignment: TextField.AlignHCenter
        //color: "white"
        font.bold: true;
        /*background: Item {
            Rectangle {
                id: urlbarBackground
                anchors.fill: parent
                color: "black"
                radius: 6
            }

            DropShadow {
                anchors.fill: urlbarBackground
                horizontalOffset: 0
                verticalOffset: 0
                radius: 12
                samples: 17
                color: "#D0000000"
                source: urlbarBackground
                z: 2
            }
        }*/
        z: 1

        PropertyAnimation {
            id: urlbarSlideIn
            target: urlbar
            property: "anchors.topMargin"
            to: 16
            duration: 60
        }

        PropertyAnimation {
            id: urlbarSlideOut
            target: urlbar
            property: "anchors.topMargin"
            to: -urlbar.height - 16
            duration: 60
        }

        onFocusChanged: {
            if (!focus) {
                urlbarSlideOut.start();
                inputPanel.visible = false;
            } else {
                urlbarSlideIn.start();
                inputPanel.visible = true;
            }
        }

        onAccepted: {
            webview.url = text;
            webview.focus = true;
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Escape) {
                webview.focus = true;
            }
        }
    }

    WebView {
        id: webview
        url: Qt.application.arguments[1]
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_Alt) {
                altPressed = true;
            }

            if (altPressed && event.key === Qt.Key_Backspace) {
                urlbar.focus = true;
            }
        }

        Keys.onReleased: {
            if (altPressed && event.key === Qt.Key_Alt) {
                altPressed = false;
            }
        }

        /*MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true

            onClicked: {
                console.log("clicked");
                mouse.accepted = false
            }
        }*/

        onNavigationRequested: {
            request.action = WebView.AcceptRequest;
            // detect URL scheme prefix, most likely an external link
            /*var schemaRE = /^\w+:/;
            if (schemaRE.test(request.url)) {
                request.action = WebView.AcceptRequest;
            } else {
                request.action = WebView.IgnoreRequest;
                // delegate request.url here
            }*/
        }
    }

    /*InputPanel {
        id: inputPanel
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
        anchors.left: parent.left
        anchors.right: parent.right
    }*/
}
