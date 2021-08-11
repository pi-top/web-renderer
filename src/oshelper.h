#ifndef OSHELPER_H
#define OSHELPER_H

#include <QObject>

class IOSHelper
{
 public:
  virtual ~IOSHelper() = default;
  virtual bool isPi() = 0;
  virtual int runCommandVerbose(const QString& command, const QStringList& args,
                                QString& response, bool& failedToStart,
                                bool& timedOut, int timeout) = 0;
  virtual int runCommandVerbose(const QString& command, QString& response,
                                int timeout) = 0;
  virtual int runCommandVerbose(const QString& command, int timeout) = 0;
  virtual int runCommand(const QString& command, const QStringList& args,
                         int timeout, QString& response) = 0;
  virtual int runCommand(const QString& command, const QStringList& args,
                         int timeout) = 0;
  virtual int runCommand(const QString& command, int timeout,
                         QString& response) = 0;
  virtual int runCommand(const QString& command, int timeout) = 0;
  virtual void runCommandDetached(const QString& command,
                                  const QStringList& args) = 0;
  virtual void runCommandDetached(const QString& command) = 0;
  virtual QString getUserAnalyticsConfigPath() = 0;
  virtual QString getLogPath() = 0;
  virtual bool isSSHEnabled() = 0;
  virtual bool sshPassAuthIsEnabled() = 0;
};

class OSHelper : public QObject, public IOSHelper
{
  Q_OBJECT
 public:
  explicit OSHelper(QObject* parent = nullptr);

  Q_INVOKABLE bool isPi() override;
  Q_INVOKABLE int runCommandVerbose(const QString& command,
                                    const QStringList& args, QString& response,
                                    bool& failedToStart, bool& timedOut,
                                    int timeout) override;
  Q_INVOKABLE int runCommandVerbose(const QString& command, QString& response,
                                    int timeout) override;
  Q_INVOKABLE int runCommandVerbose(const QString& command,
                                    int timeout) override;
  Q_INVOKABLE int runCommand(const QString& command, const QStringList& args,
                             int timeout, QString& response) override;
  Q_INVOKABLE int runCommand(const QString& command, const QStringList& args,
                             int timeout) override;
  Q_INVOKABLE int runCommand(const QString& command, int timeout,
                             QString& response) override;
  Q_INVOKABLE int runCommand(const QString& command, int timeout) override;
  Q_INVOKABLE void runCommandDetached(const QString& command,

                                      const QStringList& args) override;
  Q_INVOKABLE void runCommandDetached(const QString& command) override;
  Q_INVOKABLE QString getTempDirectory() override;
  Q_INVOKABLE QString getUserAnalyticsConfigPath() override;
  Q_INVOKABLE QString getLogPath() override;
  Q_INVOKABLE QStringList getRunningProcessNames() override;
  Q_INVOKABLE bool isSSHEnabled() override;
  Q_INVOKABLE bool sshPassAuthIsEnabled() override;

  Q_INVOKABLE static QString nameOfCoderPackage();
  Q_INVOKABLE static QString getCoderConfigPath();
  Q_INVOKABLE static QString getCoderSharedResourcesPath();
  Q_INVOKABLE static QString getSystemConfigurationPath();
  Q_INVOKABLE static QString getUserPiTopConfigPath();

 private:
  void touchFile(const QString& filename);
  static QString getOS();
};

#endif  // OSHELPER_H
