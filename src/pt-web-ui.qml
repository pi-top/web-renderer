import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtWebKit 3.0

ApplicationWindow {
    visible: true
    id: root

    // Properties to be updated
    visibility: "Windowed"
    width: -1
    height: -1
    title: ""
    property string url: ""
    // END

    WebView {
        url: root.url
        anchors.fill: parent
        focus: true
    }
}
