#ifndef CONFIG_H
#define CONFIG_H

#include "fileio.h"
#include "oshelper.h"

#include <QJsonObject>
#include <QObject>

class IConfig
{
 public:
  virtual ~IConfig() = default;
  virtual void setValue(const QString& property, const QJsonValue& value) = 0;
  virtual QJsonValue getValue(const QString& property) = 0;
  virtual int getInt(const QString& property, int defaultValue) = 0;
};

class Config : public QObject, public IConfig
{
  Q_OBJECT

 public:
  Config(FileIO* fileIO, const QString& configFilePath,
         QObject* parent = nullptr);

  Q_INVOKABLE void setValue(const QString& property,
                            const QJsonValue& value) override;
  Q_INVOKABLE QJsonValue getValue(const QString& property) override;

  int getInt(const QString& property, int defaultValue) override;

 signals:

 public slots:

 private:
  QJsonObject loadJson();
  QByteArray loadByteArray();
  bool parseLegacyConfig(const QByteArray& contentArr);
  void writeJsonObj(const QJsonObject& jsonObj);

  FileIO* m_fileIO;
  QString m_configFilePath;
};

#endif  // CONFIG_H
