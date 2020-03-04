# pt-web-ui

Qt/QML-based web browser
(modified from: https://github.com/kernelconcepts/qt-mini-browser)

## Run dependencies

``` bash
sudo apt install -y libqt5quick5 libqt5qml5 qml-module-qtquick-window2 qml-module-qtquick2 qml-module-qtquick-controls qml-module-qtwebkit

```

## Development dependencies

``` bash
$ sudo apt install qt5-default libqt5webkit5-dev qtwebengine5-dev
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
