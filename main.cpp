#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QScreen>


#include <QDebug>

QString APP_NAME = "pi-top Web UI Viewer";
float APP_VERSION = 1.0;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION.toString());

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("pt-web-ui.qml")));
    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }
    QObject *rootObject = engine.rootObjects().constFirst();


    ////////////////////////////////
    // SET UP COMMAND LINE PARSER //
    ////////////////////////////////

    QCommandLineParser parser;
    parser.setApplicationDescription(APP_NAME);
    //parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption titleOption(QStringList() << "t" << "title", "Title of window", "title", "");
    parser.addOption(titleOption);

    QCommandLineOption urlOption(QStringList() << "u" << "url", "URL to point to", "url", "");
    parser.addOption(urlOption);

    QCommandLineOption fullScreenOption(QStringList() << "f" << "fullscreen", "Start the app in fullscreen mode - ignores height and width");
    parser.addOption(fullScreenOption);

    QCommandLineOption widthOption(QStringList() << "w" << "width", "Window width (relative to screen, 0-1). Defaults to 1.0.", "width", "");
    parser.addOption(widthOption);

    QCommandLineOption heightOption(QStringList() << "h" << "height", "Window height (relative to screen, 0-1). Defaults to 1.0.", "height", "");
    parser.addOption(heightOption);

    parser.process(app);


    ///////////////////////////////
    // PROCESS COMMAND LINE ARGS //
    ///////////////////////////////

    rootObject->setProperty("title", parser.value(titleOption));


    rootObject->setProperty("url", parser.value(urlOption));


    if (parser.isSet(fullScreenOption))
    {
        rootObject->setProperty("visibility", "FullScreen");
    }
    else
    {
        rootObject->setProperty("visibility", "Windowed");
    }


    QSize screenSize = app.primaryScreen()->size();

    bool ok;
    QString widthScaleStr = parser.value(widthOption);
    float widthScalingFactor;
    if (widthScaleStr == "")
    {
        widthScalingFactor = 1.0;
    }
    else
    {
        widthScalingFactor = widthScaleStr.toFloat(&ok);
    }
    
    if (ok) {
        rootObject->setProperty("width", widthScalingFactor * screenSize.width());
    } else {
        qFatal("Invalid width specified");
    }

    QString heightScaleStr = parser.value(heightOption);
    float heightScalingFactor;
    if (heightScaleStr == "")
    {
        heightScalingFactor = 1.0;
    }
    else
    {
        heightScalingFactor = heightScaleStr.toFloat(&ok);
    }
    
    if (ok) {
        rootObject->setProperty("height", heightScalingFactor * screenSize.height());
    } else {
        qFatal("Invalid height specified");
    }

    ///////////
    // START //
    ///////////

    return app.exec();
}
