#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QThread>
#include <QtWebEngine>
#include <QUrl>

#include "config.h"
#include "console_log_handler.h"
#include "fileio.h"
#include "ptlogger.h"
#include "unix_signal_manager.h"

bool isPi()
{
#ifdef __arm__
  return true;
#else
  return false;
#endif
}

int runCommand(const QString &command, const QStringList &args, int timeout,
               QString &response)
{
  qDebug().noquote() << "Executing:" << command;

  if (args.length() > 0)
  {
    qDebug().noquote() << "with" << args;
  }

  int exitCode = -1;

  QProcess process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert(QStringLiteral("DISPLAY"), QStringLiteral(":0"));
  process.setProcessEnvironment(env);

  if (args.length() > 0)
  {
    process.start(command, args);
  }
  else
  {
    process.start(command);
  }

  if (process.waitForStarted(timeout) == false)
  {
    qDebug().noquote().nospace()
        << QStringLiteral("\"") << command << QStringLiteral(" ")
        << args.join(QStringLiteral(" ")) << QStringLiteral("\" err");
  }

  if (process.waitForFinished(timeout) == false)
  {
    qDebug().noquote().nospace()
        << QStringLiteral("\"") << command << QStringLiteral(" ")
        << args.join(QStringLiteral(" ")) << "\" timed out";
  }

  response = process.readAll();
  exitCode = process.exitCode();

  QString stderr = process.readAllStandardError();
  QString stdout = process.readAllStandardOutput();

  if (stderr.isEmpty() == false)
  {
    qDebug().noquote() << "stderr:\n" << stderr;
  }

  if (stdout.isEmpty() == false)
  {
    qDebug().noquote() << "stdout:\n" << stdout;
  }

  process.close();

  return exitCode;
}

void waitForServerResponse(const QUrl url)
{
  qInfo() << "Waiting for backend web server response...";
  bool serverIsUp = false;
  int counter = 0;
  int counterMax = 30;
  while (serverIsUp == false)
  {
    if (counter >= counterMax)
    {
      qFatal("Unable to contact web server!");
      exit(1);
    }

    QString resp;
    int exitCode = runCommand("curl",
                              QStringList() << "--max-time"
                                            << "1"
                                            << "--silent"
                                            << "--fail"
                                            << "--output"
                                            << "/dev/null" << url.toString(),
                              1000, resp);

    qDebug() << exitCode;

    if (exitCode == 0)
    {
      qInfo() << "Backend web server responded!";
      break;
    }

    qInfo() << "Backend web server did not respond - sleeping for 1 second...";
    QThread::sleep(1);
    ++counter;
  }
}

void parseSizeArgument(QString sizeStr, float &width, float &height)
{
  QRegExp rx("x");
  QStringList sizeList = sizeStr.split(rx, QString::SkipEmptyParts);

  if (sizeList.size() != 2)
  {
    qFatal("Error: Invalid size argument. Make sure it's in the format '<width_scalar>x<height_scalar>' where each scalar is a decimal number from 0 to 1 (e.g.: '0.6x0.4').");
    exit(1);
  }

  width = sizeList.at(0).toFloat();
  height = sizeList.at(1).toFloat();
  if (width <= 0.0 || width > 1.0 || height <= 0.0 || height > 1.0)
  {
    qFatal("Error: size scalars must be a decimal number from 0 to 1 (e.g.: '0.6x0.4').");
    exit(1);
  }
}

int main(int argc, char *argv[])
{

  QGuiApplication app(argc, argv);
  QtWebEngine::initialize();

  QCommandLineParser parser;
  parser.setApplicationDescription("Application that presents web content in a way that looks like a native application window");
  QCommandLineOption kioskModeOption(QStringList() << "k" << "kiosk", "Start in kiosk mode. In this mode, the application will start in fullscreen with minimal UI. It will prevent the user from quitting or performing any interaction outside of usage of the application");
  parser.addOption(kioskModeOption);
  QCommandLineOption sizeOption(QStringList() << "s" << "size", "Window size relative to screen, expressed as '<width_scalar>x<height_scalar>' (e.g. '0.6x0.4'), where each scalar is a decimal number from 0 to 1.", "size", "0.65x0.55");
  parser.addOption(sizeOption);
  QCommandLineOption urlOption(QStringList() << "u" << "url", "URL to open. Defaults to localhost.", "url", "http://localhost:80");
  parser.addOption(urlOption);
  QCommandLineOption titleOption(QStringList() << "t" << "window-title", "Specify a title for the window. This will appear in the title bar.", "title", "web-renderer");
  parser.addOption(titleOption);
  QCommandLineOption iconOption(QStringList() << "i" << "icon", "Specify the path to an icon to be set as the application icon. This will appear in the title bar (unless the '--hide-frame' option is selected), and in the application bar.", "path", "");
  parser.addOption(iconOption);
  QCommandLineOption hideWindowFrameOption(QStringList() << "hide-window-frame", "Hide the frame (title bar and border) of the window. This means there will be no 'close', 'minimize' and 'maximize' buttons. Without this, the user cannot move or resize the window via the window system.");
  parser.addOption(hideWindowFrameOption);
  QCommandLineOption helpOption = parser.addHelpOption();

  parser.process(app);

  if (parser.isSet(helpOption))
  {
    parser.showHelp();
    return 0;
  }

  bool kioskModeArg = parser.isSet(kioskModeOption);
  bool hideWindowFrameArg = parser.isSet(hideWindowFrameOption);

  QString sizeStr = parser.value(sizeOption);
  float width, height;
  parseSizeArgument(sizeStr, width, height);

  const QUrl url(parser.value(urlOption));
  const QString titleArg = parser.value(titleOption);
  const QString iconArg = parser.value(iconOption);

  int defaultLoggingMode = LoggingMode::Console | LoggingMode::Journal;
#ifdef QT_DEBUG
  int defaultLogLevel = LOG_DEBUG;
#else
  int defaultLogLevel = LOG_INFO;
#endif

  PTLogger::initialiseLogger(defaultLoggingMode, defaultLogLevel);

  QStringList args = app.arguments();
  qDebug() << args;

  QString configFilePath;
  if (isPi())
  {
    configFilePath = "/usr/lib/web-renderer/web-renderer.json";
  }
  else
  {
    configFilePath = "web-renderer.json";
  }

  qInfo().noquote() << "Config file path:" << configFilePath;
  QFile cfgFile(configFilePath);
  if (!cfgFile.exists())
  {
    qInfo().noquote() << "Couldnt find config file. Using default parameters";
  }
  FileIO *fileIO = new FileIO();
  Config *config;
  config = new Config(fileIO, configFilePath);

  int logLevel = config->getInt("logOutputLevel", defaultLogLevel);
  if (logLevel != defaultLogLevel)
  {
    qInfo().noquote() << "Logging level overridden to" << logLevel
                      << "from config file";
    PTLogger::setLevel(logLevel);
  }

  // Suppress "qt5ct: using qt5ct plugin" stdout output
  qputenv("QT_LOGGING_RULES", "qt5ct.debug=false");

  UnixSignalManager::catchUnixSignals(
      {SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGTSTP});

  qInfo() << "Loading QML";
  QQmlApplicationEngine engine;
  if (isPi())
  {
    engine.load(QUrl(QStringLiteral("/usr/lib/web-renderer/web-renderer.qml")));
  }
  else
  {
    engine.load(QUrl(QStringLiteral("qrc:/web-renderer.qml")));
  }

  if (engine.rootObjects().isEmpty())
  {
    return -1;
  }
  QObject *rootObject = engine.rootObjects().constFirst();

  QQuickWindow *window = qobject_cast<QQuickWindow *>(rootObject);
  ConsoleLogHandler consoleLogHandler;
  QObject::connect(window, SIGNAL(logMessage(int, QString, int, QString)),
                   &consoleLogHandler,
                   SLOT(handleLog(int, QString, int, QString)));

  ////////////////
  // SETUP VIEW //
  ////////////////
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

  waitForServerResponse(url);

  ///////////////
  // MAIN LOOP //
  ///////////////
  qInfo() << "Starting main loop";
  int exitCode = app.exec();

  return exitCode;
}
