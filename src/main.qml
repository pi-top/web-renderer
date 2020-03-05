import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtWebEngine 1.0

ApplicationWindow {
  visible: true
  id: root

  // Properties to be initialised
  visibility: "Windowed"
  title: ""
  width: -1
  height: -1
  property string url: ""

  property bool initialised: false

  onInitialisedChanged: {
    x = Screen.width / 2 - width / 2
    y = Screen.height / 2 - height / 2
  }

  WebEngineView {
    url: root.url
    anchors.fill: parent
    focus: true
  }
}
