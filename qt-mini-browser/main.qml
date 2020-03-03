import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtWebKit 3.0
ApplicationWindow {
    id: root
    visibility: "Windowed"
    visible: true
    width: -1
    height: -1
    title: ""
    property string url: ""
    WebView {
        url: root.url
        anchors.fill: parent
        focus: true
    }
}
