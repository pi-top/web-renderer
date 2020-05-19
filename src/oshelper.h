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

  /* Desc:
   *   This function is intended to run shell commands only, through bash, as
   *   opposed to forking and executing a binary. This enables bash builtin
   *   commands to be run, pipes to be used, and the existance of environment
   *   variables that are sourced in .bashrc to be used.
   *
   *   The function will group all failure modes together under one return code
   *   and will trim the standard output returned in result.
   */
  virtual int runShellCommand(const QString& command,
                              QString& result) = 0;
  virtual bool processIsRunning(const QString& processName) = 0;
  virtual QString getRootDir() = 0;
  virtual QString getTempDirectory() = 0;
  virtual QString getUserAnalyticsConfigPath() = 0;
  virtual QString getUserAnalyticsFilePath() = 0;
  virtual QString getCoderUserConfigFilePath() = 0;
  virtual QString getLogPath() = 0;
  virtual QString getLogFilePath() = 0;
  virtual QString getTranslationsPath() = 0;
  virtual QStringList getProcessIds(const QString&) = 0;
  virtual QStringList getRunningProcessNames() = 0;
  virtual bool isUserPasswordSafe(const QString& username,
                                  const QString& passwordTest) = 0;
  virtual bool isSSHEnabled() = 0;
  virtual bool sshPassAuthIsEnabled() = 0;
  virtual bool enableSSHPassAuthAsRoot() = 0;
  virtual bool isSSHEnabledWithPassAuth() = 0;
  virtual bool isRootUser() = 0;
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
  Q_INVOKABLE int runShellCommand(const QString& command,
                                  QString& result) override;
  Q_INVOKABLE bool processIsRunning(const QString& processName) override;
  Q_INVOKABLE QString getRootDir() override;
  Q_INVOKABLE QString getTempDirectory() override;
  Q_INVOKABLE QString getUserAnalyticsConfigPath() override;
  Q_INVOKABLE QString getUserAnalyticsFilePath() override;
  Q_INVOKABLE QString getCoderUserConfigFilePath() override;
  Q_INVOKABLE QString getLogPath() override;
  Q_INVOKABLE QString getLogFilePath() override;
  Q_INVOKABLE QString getTranslationsPath() override;
  Q_INVOKABLE QStringList getProcessIds(const QString& processName) override;
  Q_INVOKABLE QStringList getRunningProcessNames() override;
  Q_INVOKABLE bool isUserPasswordSafe(const QString& username,
                                      const QString& passwordTest) override;
  Q_INVOKABLE bool isSSHEnabled() override;
  Q_INVOKABLE bool sshPassAuthIsEnabled() override;
  Q_INVOKABLE bool enableSSHPassAuthAsRoot() override;
  Q_INVOKABLE bool isSSHEnabledWithPassAuth() override;
  Q_INVOKABLE bool isRootUser() override;

  Q_INVOKABLE static QString nameOfCoderPackage();
  Q_INVOKABLE static QString getCoderConfigPath();
  Q_INVOKABLE static QString getCachedWorksheetsPath();
  Q_INVOKABLE static QString getCachedCoursesPath();
  Q_INVOKABLE static QString getCoderSharedResourcesPath();
  Q_INVOKABLE static QString getSystemConfigurationPath();
  Q_INVOKABLE static QString getCoderHelperFilesPath();
  Q_INVOKABLE static QString getSplashscreenHelperFilesPath();
  Q_INVOKABLE static QString getUserPiTopConfigPath();
  Q_INVOKABLE static QString getWorksheetsWorkingDirectory();

 private:
  void touchFile(const QString& filename);
  static QString getOS();
};

#endif  // OSHELPER_H
