#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QThread>
#include <QtWebEngine>
#include <QUrl>

#include "console_log_handler.h"
#include "logger.h"
#include "unix_signal_manager.h"

void parseSizeArgument(QString sizeStr, float &width, float &height)
{
  QRegExp rx("x");
  QStringList sizeList = sizeStr.split(rx, QString::SkipEmptyParts);

  if (sizeList.size() != 2)
  {
    qCritical("Error: Invalid size argument. Make sure it's in the format '<width_scalar>x<height_scalar>' where each scalar is a decimal number from 0 to 1 (e.g.: '0.6x0.4').");
    exit(1);
  }

  width = sizeList.at(0).toFloat();
  height = sizeList.at(1).toFloat();
  if (width <= 0.0 || width > 1.0 || height <= 0.0 || height > 1.0)
  {
    qCritical("Error: size scalars must be a decimal number from 0 to 1 (e.g.: '0.6x0.4').");
    exit(1);
  }
}

int main(int argc, char *argv[])
{

  QGuiApplication app(argc, argv);
  QtWebEngine::initialize();


  /////////////////////////
  // Set up command line //
  /////////////////////////
  QCommandLineParser parser;
  parser.setApplicationDescription("Application to present web content in a native application window style");

  parser.addPositionalArgument("url", "URL to open");

  QCommandLineOption kioskModeOption(QStringList() << "k" << "kiosk", "Start in kiosk mode. In this mode, the application will start in fullscreen with minimal UI. It will prevent the user from quitting or performing any interaction outside of usage of the application");
  parser.addOption(kioskModeOption);
  QCommandLineOption sizeOption(QStringList() << "s" << "size", "Window size relative to screen, expressed as '<width_scalar>x<height_scalar>' (e.g. '0.6x0.4'), where each scalar is a decimal number from 0 to 1.", "size", "0.65x0.55");
  parser.addOption(sizeOption);
  QCommandLineOption titleOption(QStringList() << "t" << "window-title", "Specify a title for the window. This will appear in the title bar.", "title", "web-renderer");
  parser.addOption(titleOption);
  QCommandLineOption iconOption(QStringList() << "i" << "icon", "Specify the path to an icon to be set as the application icon. This will appear in the title bar (unless the '--hide-frame' option is selected), and in the application bar.", "path", "");
  parser.addOption(iconOption);
  QCommandLineOption hideWindowFrameOption(QStringList() << "hide-frame", "Hide the frame (title bar and border) of the window. This means there will be no 'close', 'minimize' and 'maximize' buttons. Without this, the user cannot move or resize the window via the window system.");
  parser.addOption(hideWindowFrameOption);
  QCommandLineOption helpOption = parser.addHelpOption();

  parser.process(app);

  if (parser.isSet(helpOption))
  {
    parser.showHelp(0);
    return 0;
  }

  /////////////////////
  // Parse arguments //
  /////////////////////
  bool kioskModeArg = parser.isSet(kioskModeOption);
  bool hideWindowFrameArg = parser.isSet(hideWindowFrameOption);

  QString sizeStr = parser.value(sizeOption);
  float width, height;
  parseSizeArgument(sizeStr, width, height);

  const QString titleArg = parser.value(titleOption);
  const QString iconArg = parser.value(iconOption);

  const QStringList args = parser.positionalArguments();
  if (args.size() != 1)
  {
    qCritical("Error: missing URL");
    parser.showHelp(1);
  }

  const QUrl url = args.at(0);
  if (!url.isValid() || url.scheme().size() == 0)
  {
    qCritical("Error: invalid URL format.");
    exit(1);
  }

  ////////////////////
  // Handle logging //
  ////////////////////
  int defaultLoggingMode = LoggingMode::Console | LoggingMode::Journal;
#ifdef QT_DEBUG
  int defaultLogLevel = LOG_DEBUG;
#else
  int defaultLogLevel = LOG_INFO;
#endif

  Logger::initialiseLogger(defaultLoggingMode, defaultLogLevel);

  // TODO: add as argument
  //
  // if (logLevel != defaultLogLevel)
  // {
  //   qInfo().noquote() << "Logging level set to" << logLevel;
  //   Logger::setLevel(logLevel);
  // }

  // Suppress "qt5ct: using qt5ct plugin" stdout output
  qputenv("QT_LOGGING_RULES", "qt5ct.debug=false");

  //////////////
  // Frontend //
  //////////////
  qInfo() << "Loading QML";
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/web-renderer.qml")));

  if (engine.rootObjects().isEmpty())
  {
    return -1;
  }
  QObject *rootObject = engine.rootObjects().constFirst();

  ////////////////////
  // Handle logging //
  ////////////////////
  QQuickWindow *window = qobject_cast<QQuickWindow *>(rootObject);
  ConsoleLogHandler consoleLogHandler;
  QObject::connect(window, SIGNAL(logMessage(int, QString, int, QString)),
                   &consoleLogHandler,
                   SLOT(handleLog(int, QString, int, QString)));

  ////////
  // UI //
  ////////
  qInfo() << "Configuring view";

  app.setApplicationName(titleArg);
  rootObject->setProperty("title", titleArg);

  const QSize &screenSize = app.primaryScreen()->size();
  if (kioskModeArg)
  {
    rootObject->setProperty("visibility", "FullScreen");
    rootObject->setProperty("width", screenSize.width());
    rootObject->setProperty("height", screenSize.height());
  }
  else
  {
    rootObject->setProperty("visibility", "Windowed");
    rootObject->setProperty("height", height*screenSize.height());
    rootObject->setProperty("width", width*screenSize.width());
  }
  rootObject->setProperty("url", url);

  window->setFlag(Qt::FramelessWindowHint, hideWindowFrameArg);

  if (!iconArg.isEmpty())
  {
    const QIcon icon(iconArg);
    app.setWindowIcon(icon);
  }

  rootObject->setProperty("initialised", true);

  ////////////////////
  // Handle signals //
  ////////////////////
  UnixSignalManager::catchUnixSignals(
      {SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGTSTP});

  ///////////////
  // MAIN LOOP //
  ///////////////
  qInfo() << "Starting main loop";
  int exitCode = app.exec();

  return exitCode;
}
