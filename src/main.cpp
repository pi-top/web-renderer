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
    else
    {
      qInfo()
          << "Backend web server did not respond - sleeping for 1 second...";
      QThread::sleep(1);
    }
  }
}

int main(int argc, char *argv[])
{

  QGuiApplication app(argc, argv);
  QtWebEngine::initialize();

  QCommandLineParser parser;
  parser.setApplicationDescription("Application that presents web content in a way that looks like a native application window");
  QCommandLineOption windowedModeOption(QStringList() << "wm" << "windowed", "Start application in windowed mode.");
  parser.addOption(windowedModeOption);
  QCommandLineOption widthOption(QStringList() << "ww" << "width", "Window width relative to screen, from 0 to 1.", "width", "0.65");
  parser.addOption(widthOption);
  QCommandLineOption heightOption(QStringList() << "wh" << "height", "Window height relative to screen, from 0 to 1.", "height", "0.55");
  parser.addOption(heightOption);
  QCommandLineOption urlOption(QStringList() << "u" << "url", "URL to open. Defaults to localhost.", "url", "http://localhost:80");
  parser.addOption(urlOption);
  QCommandLineOption titleOption(QStringList() << "t" << "title", "Specify a title for the window.", "title", "web-renderer");
  parser.addOption(titleOption);
  QCommandLineOption iconOption(QStringList() << "i" << "icon", "Specify the path to an icon to be set as the application icon.", "path", "");
  parser.addOption(iconOption);
  QCommandLineOption showWindowFrameOption(QStringList() << "wf" << "show-window-frame", "Show the frame of the window.");
  parser.addOption(showWindowFrameOption);
  QCommandLineOption helpOption = parser.addHelpOption();

  parser.process(app);

  if (parser.isSet(helpOption))
  {
    parser.showHelp();
    return 0;
  }

  bool windowedModeArg = parser.isSet(windowedModeOption);
  bool showWindowFrameArg = parser.isSet(showWindowFrameOption);

  bool widthArg;
  QString widthStr = parser.value(widthOption);
  float width = widthStr.toFloat(&widthArg);

  bool heightArg;
  QString heightStr = parser.value(heightOption);
  float height = heightStr.toFloat(&heightArg);

  const QUrl url(parser.value(urlOption));
  const QString title = parser.value(titleOption);
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

  app.setApplicationName(title);
  rootObject->setProperty("title", title);

  const QSize &screenSize = app.primaryScreen()->size();
  if (windowedModeArg && widthArg)
  {
    rootObject->setProperty("width", width*screenSize.width());
  }
  if (windowedModeArg && heightArg)
  {
    rootObject->setProperty("height", height*screenSize.height());
  }
  if (! windowedModeArg)
  {
    rootObject->setProperty("visibility", "FullScreen");
    rootObject->setProperty("width", screenSize.width());
    rootObject->setProperty("height", screenSize.height());
  }
  rootObject->setProperty("url", url);

  if (showWindowFrameArg)
  {
    window->setFlag(Qt::FramelessWindowHint, false);
  }

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
