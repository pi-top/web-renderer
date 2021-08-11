#include "oshelper.h"

#ifndef __APPLE__
#include "crypt.h"

#include "shadow.h"
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <unistd.h>

#ifndef SKIP_COMMON_LOGGING
#include "ptlogger.h"
#endif

#ifndef TRACE_METHOD
#define TRACE_METHOD()
#endif

OSHelper::OSHelper(QObject* parent) : QObject(parent)
{
}

QString OSHelper::getOS()
{
#if defined(Q_OS_MAC)
    //PTLogger::info("OS: Q_OS_MAC");
  return QStringLiteral("Mac");
#elif defined(Q_OS_WIN)
    //PTLogger::info("OS: Q_OS_WIN");
  return QStringLiteral("Windows");
#elif defined(PT_SPOOF_MAC)
    //PTLogger::info("OS: PT_SPOOF_MAC");
  return QStringLiteral("Mac");
#elif defined(Q_OS_LINUX)
    //PTLogger::info("OS: Q_OS_LINUX");
  return QStringLiteral("Linux");
#elif defined(Q_OS_UNIX)
   // PTLogger::info("OS: Q_OS_Q_OS_LINUX");
  return QStringLiteral("Unix");
#else
   // PTLogger::info("OS: Unknown");
  return QStringLiteral("Unknown");
#endif
}

bool OSHelper::isPi()
{
  return getOS() != QStringLiteral("Mac");
}

QString OSHelper::nameOfCoderPackage()
{
  return QStringLiteral("pt-coder");
}

int OSHelper::runCommand(const QString& command, const QStringList& args,
                         int timeout, QString& response)
{
  TRACE_METHOD();

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

int OSHelper::runCommandVerbose(const QString& command, const QStringList& args,
                                QString& response, bool& failedToStart,
                                bool& timedOut, int timeout)
{
  TRACE_METHOD();

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

  failedToStart = (process.waitForStarted(timeout) == false);
  if (failedToStart)
  {
    qDebug().noquote().nospace()
        << QStringLiteral("\"") << command << QStringLiteral(" ")
        << args.join(QStringLiteral(" ")) << QStringLiteral("\" err");
  }

  timedOut = (process.waitForFinished(timeout) == false);
  if (timedOut)
  {
    qDebug().noquote().nospace()
        << QStringLiteral("\"") << command << QStringLiteral(" ")
        << args.join(QStringLiteral(" ")) << QStringLiteral("\" timed out");
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

int OSHelper::runCommandVerbose(const QString& command, QString& response,
                                int timeout)
{
  QStringList args;
  bool failedToStart;
  bool timedOut;
  return runCommandVerbose(command, args, response, failedToStart, timedOut,
                           timeout);
}

int OSHelper::runCommandVerbose(const QString& command, int timeout)
{
  QStringList args;
  QString response;
  bool failedToStart;
  bool timedOut;
  return runCommandVerbose(command, args, response, failedToStart, timedOut,
                           timeout);
}

int OSHelper::runCommand(const QString& command, const QStringList& args,
                         int timeout)
{
  TRACE_METHOD();

  QString unusedResponse;
  return runCommand(command, args, timeout, unusedResponse);
}

int OSHelper::runCommand(const QString& command, int timeout, QString& response)
{
  TRACE_METHOD();

  return runCommand(command, {}, timeout, response);
}

int OSHelper::runCommand(const QString& command, int timeout)
{
  TRACE_METHOD();

  QString unusedResponse;
  return runCommand(command, {}, timeout, unusedResponse);
}

void OSHelper::runCommandDetached(const QString& command,
                                  const QStringList& args)
{
  TRACE_METHOD();

  qDebug().noquote() << "Executing detached:" << command;

  if (args.length() > 0)
  {
    qDebug().noquote() << "with" << args;
  }

  QProcess process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert(QStringLiteral("DISPLAY"), QStringLiteral(":0"));
  process.setProcessEnvironment(env);

  bool success;

  if (args.length() > 0)
  {
    success = process.startDetached(command, args);
  }
  else
  {
    success = process.startDetached(command);
  }

  process.close();

  if (success == false)
  {
    qCritical() << "There was an error trying to run" << command << args << ":"
                << process.error();
  }
}

void OSHelper::runCommandDetached(const QString& command)
{
  runCommandDetached(command, {});
}

QString OSHelper::getCoderSharedResourcesPath()
{
  return QString("/usr/share/" + nameOfCoderPackage() + "/");
}

QString OSHelper::getSystemConfigurationPath()
{
  if (getOS() == QStringLiteral("Mac"))
  {
    return QCoreApplication::applicationDirPath() + "/";
  }
  else
  {
    return QStringLiteral("/etc/pi-top/");
  }
}

QString OSHelper::getUserPiTopConfigPath()
{
  QString configDirStr = QStringLiteral("/home/pi/.config/pi-top/");
  if (getOS() == QStringLiteral("Mac"))
  {
    configDirStr = QCoreApplication::applicationDirPath() + "/";
  }

  QDir configDir(configDirStr);
  if (configDir.exists() == false)
  {
    configDir.mkpath(QStringLiteral("."));
  }
  return configDirStr;
}

QString OSHelper::getCoderConfigPath()
{
  QString configDirStr = QStringLiteral("");
  if (getOS() == QStringLiteral("Mac"))
  {
    configDirStr = QCoreApplication::applicationDirPath() + "/";
  }
  else
  {
    configDirStr = getUserPiTopConfigPath() + nameOfCoderPackage() + "/";
  }

  QDir configDir(configDirStr);
  if (configDir.exists() == false)
  {
    configDir.mkpath(QStringLiteral("."));
  }
  return configDirStr;
}

QString OSHelper::getUserAnalyticsConfigPath()
{
  QString configDirStr = QStringLiteral("");
  if (getOS() == QStringLiteral("Mac"))
  {
    configDirStr = QCoreApplication::applicationDirPath() + "/";
  }
  else
  {
    configDirStr = getUserPiTopConfigPath() + "pt-analytics/";
  }

  QDir configDir(configDirStr);
  if (configDir.exists() == false)
  {
    configDir.mkpath(QStringLiteral("."));
  }
  return configDirStr;
}

void OSHelper::touchFile(const QString& filename)
{
  QFile file(filename);
  if (file.exists() == false)
  {
    if (file.open(QIODevice::ReadWrite))
    {
      file.write("");
      file.close();
    }
  }
}

QString OSHelper::getLogPath()
{
  QString cacheDirStr = QStringLiteral("");
  if (isPi())
  {
    cacheDirStr = getCoderConfigPath() + "logs/";
  }
  else
  {
    cacheDirStr = QCoreApplication::applicationDirPath() + "/logs/";
  }

  QDir cacheDir(cacheDirStr);
  if (cacheDir.exists() == false)
  {
    cacheDir.mkpath(QStringLiteral("."));
  }
  return cacheDirStr;
}

bool OSHelper::isSSHEnabled()
{
  if (isPi())
  {
    QString result;
    runCommand(QStringLiteral("systemctl is-enabled ssh"), 1000, result);
    return result.contains(QStringLiteral("enabled"));
  }
  else
  {
    return true;
  }
}

bool OSHelper::sshPassAuthIsEnabled()
{
  if (isPi())
  {
    QFile inputFile(QStringLiteral("/etc/ssh/sshd_config"));
    if (inputFile.open(QIODevice::ReadOnly))
    {
      QTextStream in(&inputFile);
      while (!in.atEnd())
      {
        QString line = in.readLine();
        if (line.contains(QRegExp("^PasswordAuthentication\\s*no")))
        {
          inputFile.close();
          return false;
        }
      }
      inputFile.close();
    }
  }
  return true;
}
