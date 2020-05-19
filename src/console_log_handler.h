#ifndef CONSOLELOGHANDLER_H
#define CONSOLELOGHANDLER_H

#include <QObject>
#include <QDebug>


class ConsoleLogHandler : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleLogHandler(QObject *parent = 0);

public slots:
    void handleLog(int level, QString message, int lineNumber, QString sourceID);

};

#endif // CONSOLELOGHANDLER_H
