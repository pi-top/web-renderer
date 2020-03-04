# pt-web-ui

Qt/QML-based web browser
(modified from: https://github.com/kernelconcepts/qt-mini-browser)

## Dependencies

Follow instructions [here](https://packages.debian.org/buster/armhf/qtwebengine5-dev/download) for adding Debian as a source before installing deps, then remove after.

### To Run
``` bash
sudo apt install -y libqt5quick5 libqt5qml5 qml-module-qtquick-window2 qml-module-qtquick2 qml-module-qtquick-controls qml-module-qtwebkit
```

### To Build

``` bash
$ sudo apt install -y qt5-default qtwebengine5-dev
```


## Compile

``` bash
$ qmake pt-web-ui.pro
$ make -j4
```

## Usage

``` bash
  ./pt-web-ui --title "pi-topOS Onboarding" --url localhost/onboarding --fullscreen
```

``` bash
  ./pt-web-ui --title "pi-topOS Settings" --url localhost/settings --width 0.7 --height 0.8
```
