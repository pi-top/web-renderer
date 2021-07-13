import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import QtWebEngine 1.7

ApplicationWindow {
  visible: true
  id: root
  flags: Qt.WindowStaysOnTopHint | Qt.Window | Qt.FramelessWindowHint

  // Properties to be initialised
  visibility: "Windowed"
  title: ""
  width: -1
  height: -1
  property string url: ""

  property bool initialised: false
  signal logMessage(int level, string message, int lineNumber, string sourceID)

  onInitialisedChanged: {
    x = Screen.width / 2 - width / 2
    y = Screen.height / 2 - height / 2
  }

  WebEngineView {
    url: root.url
    anchors.fill: parent
    focus: true

    onContextMenuRequested: {
      request.accepted = true
    }

    onLoadingChanged: {
      if (loadRequest.status != WebEngineView.LoadFailedStatus) {
        reload();
      }
    }

    profile: WebEngineProfile {
      httpCacheType: WebEngineProfile.NoCache
      httpUserAgent: "web-renderer"
      offTheRecord: true
    }

    onJavaScriptConsoleMessage: function (level, message, lineNumber, sourceID) {
      logMessage(level, message, lineNumber, sourceID)
    }
  }
}
