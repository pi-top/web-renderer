#include <QWebEnginePage>
#include "console_log_handler.h"
#include "ptlogger.h"

ConsoleLogHandler::ConsoleLogHandler(QObject *parent) :
    QObject(parent)
{
}

void ConsoleLogHandler::handleLog(int level, QString message, int lineNumber, QString sourceID)
{
    QString msg = "[web-renderer CONSOLE][source: " + sourceID + "] [line: " + QString::number(lineNumber) + "]:\n" + message;
    QString txt;
    switch(level)
    {
        case QWebEnginePage::InfoMessageLevel:
            // qInfo().noquote() << msg;
            PTLogger::info(msg);
            break;
        case QWebEnginePage::WarningMessageLevel:
            // qWarning().noquote() << msg;
            PTLogger::warning(msg);
            break;
        case QWebEnginePage::ErrorMessageLevel:
            // qCritical().noquote() << msg;
            PTLogger::error(msg);
            break;
        default:
            // qDebug().noquote() << msg;
            PTLogger::debug(msg);
    }
}
