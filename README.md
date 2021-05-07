# web-renderer

A simple Qt/QML-based application with an embedded [Qt WebEngineView](https://doc.qt.io/qt-5/qml-qtwebengine-webengineview.html), offering a convenient way to present web content in a way that looks like a native application window (without styling).

This repository was inspired by [kernelconcepts' `qt-mini-browser`](https://github.com/kernelconcepts/qt-mini-browser).

## Usage

``` bash
Usage: ./web-renderer [options] url
Application that presents web content in a way that looks like a native application window

Options:
  -k, --kiosk                 Start in kiosk mode. In this mode, the
                              application will start in fullscreen with minimal
                              UI. It will prevent the user from quitting or
                              performing any interaction outside of usage of the
                              application
  -s, --size <size>           Window size relative to screen, expressed as
                              '<width_scalar>x<height_scalar>' (e.g. '0.6x0.4'),
                              where each scalar is a decimal number from 0 to 1.
  -t, --window-title <title>  Specify a title for the window. This will appear
                              in the title bar.
  -i, --icon <path>           Specify the path to an icon to be set as the
                              application icon. This will appear in the title
                              bar (unless the '--hide-frame' option is
                              selected), and in the application bar.
  --hide-frame                Hide the frame (title bar and border) of the
                              window. This means there will be no 'close',
                              'minimize' and 'maximize' buttons. Without this,
                              the user cannot move or resize the window via the
                              window system.
  -h, --help                  Displays this help.

Arguments:
  url                         URL to open
```

## Examples

### Open Google
``` bash
  $ web-renderer --window-title "Google" --size 0.7x0.8 http://www.google.com
```

### Fullscreen Local Server
``` bash
  $ web-renderer --window-title "My Local Server" --kiosk http://localhost:3000
```
