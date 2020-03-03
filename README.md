# pt-web-ui

Qt/QML-based web browser
(modified from: https://github.com/kernelconcepts/qt-mini-browser)

## Suggested dependencies

``` bash
$ sudo apt install qt5-default qt5-qmake qt5-qmake-bin qtdeclarative5-dev libqt5webkit5-dev qml-module-qtquick2 qml-module-qtwebkit qml-module-qtquick-*
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
