#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QCommandLineParser>
#include <QScreen>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    //parser.setApplicationDescription("Test helper");
    //parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption urlOption(QStringList() << "u" << "url", "url to open", "url", "");
    parser.addOption(urlOption);
    QCommandLineOption fullScreenOption(QStringList() << "f" << "fullscreen", "starts the app in fullscreen mode");
    parser.addOption(fullScreenOption);
    QCommandLineOption widthOption(QStringList() << "w" << "window-width", "window width relative to screen", "width", "1");
    parser.addOption(widthOption);
    QCommandLineOption heightOption(QStringList() << "h" << "window-height", "window height relative to screen", "height", "1");
    parser.addOption(heightOption);
    parser.process(app);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *rootObject = engine.rootObjects().constFirst();
    if (parser.isSet(fullScreenOption))
        rootObject->setProperty("visibility", "FullScreen");
    else
        rootObject->setProperty("visibility", "Windowed");

    QString url = parser.value(urlOption);
    rootObject->setProperty("url", url);

    QScreen* screen = app.primaryScreen();
    QSize screenSize = screen->size();

    bool ok;
    QString widthStr = parser.value(widthOption);
    float width = widthStr.toFloat(&ok);
    if (ok) {
        rootObject->setProperty("width", width*screenSize.width());
    } else {
        qFatal("No width specifed");
    }

    QString heightStr = parser.value(heightOption);
    float height = heightStr.toFloat(&ok);
    if (ok) {
        rootObject->setProperty("height", height*screenSize.height());
    } else {
        qFatal("No height specifed");
    }

    return app.exec();
}
