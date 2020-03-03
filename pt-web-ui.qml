import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtWebEngine 1.0

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

    WebEngineView {
        url: root.url
        anchors.fill: parent
        focus: true
    }
}
