# qt-mini-browser
Simple Qt/QML based webkit browser

## Compile

```sh
$ qmake qt-mini-browser.pro
$ make -j4
$ ./qt-mini-browser http://www.google.com
```

## Usage

The browser can be started showing an initial page given by the first command line argument:
``` bash
  qt-mini-browser https://kernelconcepts.de
```
To visit another page a physical keyboard has to be connected. `ALT+BACKSPACE` shows the url text field, which can be edited and confirmed by pressing `ENTER`.

## Credits

Taken from: https://github.com/kernelconcepts/qt-mini-browser
