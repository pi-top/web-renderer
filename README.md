# pt-web-ui

A simple Qt/QML-based application with an embedded [Qt WebEngineView](https://doc.qt.io/qt-5/qml-qtwebengine-webengineview.html), offering a convenient way to present web content in a way that looks like a native application window (without styling).

This repository was inspired by [kernelconcepts' `qt-mini-browser`](https://github.com/kernelconcepts/qt-mini-browser).

## Usage

#### Test with Google
``` bash
  ./pt-web-ui --title "Google" --url https://google.com --width 0.7 --height 0.8
```

#### Fullscreen Local Server
``` bash
  ./pt-web-ui --title "My Local Server" --url http://localhost --fullscreen
```
