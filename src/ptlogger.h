#ifndef PTLOGGER_H
#define PTLOGGER_H

#include <syslog.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QProcess>
#include <QThread>

// Supplementary to those defined in syslog.h
#define LOG_TRACE 8

enum LoggingMode : int
{
  None = 0,
  Auto = 1,
  Console = 2,
  Journal = 4
};

enum TerminalColor : int
{
  TC_Default = -1,
  TC_Black = 1,
  TC_Red = 2,
  TC_Green = 3,
  TC_Yellow = 4,
  TC_Blue = 5,
  TC_Magenta = 6,
  TC_Cyan = 7,
  TC_White = 8
};

// Defined in ptlogger.cpp in the ptcommon lib. Externed here
// to make sure the same instance is used in all callees in the
// process

extern QList<int64_t> g_threadIdTable;
extern int g_loggingMode;
extern int g_loggingLevel;

class PTLogger
{
 public:
  static inline void initialiseLogger(int mode, int level = LOG_DEBUG)
  {
    qInstallMessageHandler(messageOutput);

    if (mode == LoggingMode::Auto)
    {
      if (isSystemd())
      {
        g_loggingMode = LoggingMode::Journal;
      }
      else
      {
        g_loggingMode = LoggingMode::Console;
      }
    }
    else
    {
      g_loggingMode = mode;
    }

    QString logLevelVariable = QString(getenv("PT_LOG_LEVEL"));

    bool result;
    int loggingOverride = logLevelVariable.toInt(&result);
    if (result)
    {
      g_loggingLevel = loggingOverride;
    }
    else
    {
      g_loggingLevel = level;
    }

    info(QString(QStringLiteral("Logging mode: ") +
                 QString::number(g_loggingMode) + QStringLiteral(". Level: ") +
                 QString::number(g_loggingLevel)));
  }

  static inline void setLevel(int level)
  {
    g_loggingLevel = level;
  }

  static inline void setMode(int mode)
  {
    g_loggingMode = mode;
  }

  static inline void messageOutput(QtMsgType type,
                                   const QMessageLogContext& msgLogContext,
                                   const QString& msg)
  {
    switch (type)
    {
      case QtDebugMsg:
      {
        debug(msg);
        break;
      }
      case QtInfoMsg:
      {
        info(msg);
        break;
      }
      case QtWarningMsg:
      {
        warning(msg);
        break;
      }
      case QtCriticalMsg:
      {
        error(msg);
        break;
      }
      case QtFatalMsg:
      {
        critical(msg);
        break;
      }
    }
  }

  static inline void debug(const QString& message)
  {
    logMessage(LOG_DEBUG, message);
  }

  static inline void info(const QString& message)
  {
    logMessage(LOG_INFO, message);
  }

  static inline void warning(const QString& message)
  {
    logMessage(LOG_WARNING, message);
  }

  static inline void error(const QString& message)
  {
    logMessage(LOG_ERR, message);
  }

  static inline void critical(const QString& message)
  {
    logMessage(LOG_CRIT, message);
  }

  static inline void logJson(const QString& json)
  {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseError);
    if (parseError.error == QJsonParseError::NoError)
    {
      QString formattedJsonString = doc.toJson(QJsonDocument::Indented);

      debug(formattedJsonString);
    }
    else
    {
      debug(json);
    }
  }

  static inline void logJson(const QJsonObject& jsonObject)
  {
    QJsonDocument doc(jsonObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    debug(jsonString);
  }

  static inline void traceMethod(const QString& prettyFunction)
  {
    if (g_loggingLevel >= LOG_TRACE)
    {
      int colons = prettyFunction.indexOf(QStringLiteral("::"));
      if (colons == -1)
      {
        colons = 0;
      }

      int begin =
          prettyFunction.leftRef(colons).lastIndexOf(QStringLiteral(" ")) + 1;
      int end = prettyFunction.lastIndexOf(QStringLiteral("("));

      if (begin != -1 && end != -1)
      {
        QString logLine = prettyFunction.mid(begin, end - begin);
        logMessage(LOG_TRACE, logLine);
      }
    }
  }

  static inline void writeToTerminal(
      const QString& message, TerminalColor color = TerminalColor::TC_Default)
  {
    if (color == TerminalColor::TC_Default)
    {
      fputs(message.toLatin1(), stderr);
    }
    else
    {
      QString formattedMessage = QString(getColorAnsiCode(color) + message +
                                         QStringLiteral("\u001b[0m"));
      fputs(formattedMessage.toLatin1(), stderr);
    }
  }

 private:
  static inline int getSimplifiedThreadId()
  {
    int64_t systemThreadId = (int64_t)QThread::currentThreadId();
    int simplifiedThreadId = g_threadIdTable.indexOf(systemThreadId);

    if (simplifiedThreadId == -1)
    {
      g_threadIdTable.push_back(systemThreadId);
      simplifiedThreadId = g_threadIdTable.count() - 1;
    }

    return simplifiedThreadId;
  }

  static inline bool isSystemd()
  {
    QProcess p;
    p.start("ps -o ppid= -p " +
            QString::number(QCoreApplication::applicationPid()));
    p.waitForFinished();

    if (p.exitCode() == 0)
    {
      QString result = QString(p.readAllStandardOutput());
      result = result.replace(QStringLiteral("\n"), QStringLiteral(""));
      result = result.trimmed();

      if (result == QStringLiteral("1"))
      {
        return true;
      }
    }

    return false;
  }

  static inline QString getTimestamp()
  {
    QDateTime dt = QDateTime::currentDateTimeUtc();
    return dt.toString(Qt::ISODate)
               .replace(QStringLiteral("Z"), QStringLiteral(""))
               .replace(QStringLiteral("T"), QStringLiteral(" ")) +
           QStringLiteral(".") +
           QString("%1").arg(dt.time().msec(), 3, 10, QChar('0'));
  }

  static inline void logMessage(int logLevel, QString message)
  {
    if (g_loggingLevel < logLevel)
    {
      return;
    }

    if (message.endsWith(QStringLiteral("\n")) == false)
    {
      message += QStringLiteral("\n");
    }

    int simplifiedThreadId = getSimplifiedThreadId();
    QString logMessage = QString("(%1)[%2] %3")
                             .arg(QString::number(simplifiedThreadId),
                                  QString::number(logLevel), message);

    if ((g_loggingMode & LoggingMode::Console) != 0)
    {
      QString timestamp = getTimestamp();
      QString outputMessage = QString("%1 %2").arg(timestamp, logMessage);
      writeToTerminal(outputMessage);
    }

    if ((g_loggingMode & LoggingMode::Journal) != 0)
    {
      if (logLevel > LOG_DEBUG)
      {
        // Don't broadcast messages
        logLevel = LOG_DEBUG;
      }

      syslog(logLevel, "%s", logMessage.toStdString().c_str());
    }
  }

  static inline QString getColorAnsiCode(TerminalColor color)
  {
    bool assertTest =
        ((color == TerminalColor::TC_Black) | (color == TerminalColor::TC_Red) |
         (color == TerminalColor::TC_Green) |
         (color == TerminalColor::TC_Yellow) |
         (color == TerminalColor::TC_Blue) |
         (color == TerminalColor::TC_Magenta) |
         (color == TerminalColor::TC_Cyan) |
         (color == TerminalColor::TC_White) |
         (color == TerminalColor::TC_Default));

    Q_ASSERT_X(assertTest, "PTLogger::getColorAnsiCode",
               QStringLiteral("Invalid value - colour not found")
                   .toStdString()
                   .c_str());

    switch (color)
    {
      case TerminalColor::TC_Black:
        return QStringLiteral("\u001b[30m");
      case TerminalColor::TC_Red:
        return QStringLiteral("\u001b[31m");
      case TerminalColor::TC_Green:
        return QStringLiteral("\u001b[32m");
      case TerminalColor::TC_Yellow:
        return QStringLiteral("\u001b[33m");
      case TerminalColor::TC_Blue:
        return QStringLiteral("\u001b[34m");
      case TerminalColor::TC_Magenta:
        return QStringLiteral("\u001b[35m");
      case TerminalColor::TC_Cyan:
        return QStringLiteral("\u001b[36m");
      case TerminalColor::TC_White:
        return QStringLiteral("\u001b[37m");
      case TerminalColor::TC_Default:
        return QStringLiteral("");
    }
    return QStringLiteral("");
  }
};

#define TRACE_METHOD() PTLogger::traceMethod(__PRETTY_FUNCTION__)

#endif  // PTLOGGER_H
