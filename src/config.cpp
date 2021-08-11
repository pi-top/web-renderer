#include "config.h"

#include "ptlogger.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Config::Config(FileIO* fileIO, const QString& configFilePath, QObject* parent)
    : QObject(parent), m_fileIO(fileIO), m_configFilePath(configFilePath)
{
}

bool Config::parseLegacyConfig(const QByteArray& contentArr)
{
  TRACE_METHOD();

  qInfo() << "Attempting to parse legacy config file";
  QString contentStr = QString(contentArr);
  QStringList lines = contentStr.split(QStringLiteral("\n"));

  qDebug() << "Config content:" << contentStr;

  QJsonObject convertedConfig;
  bool modified = false;

  foreach (const QString& line, lines)
  {
    if (line.trimmed().isEmpty() == false)
    {
      QStringList lineFields = line.split(QStringLiteral("="));
      if (lineFields.length() == 2)
      {
        if (lineFields.at(1) == QStringLiteral("true") ||
            lineFields.at(1) == QStringLiteral("false"))
        {
          bool boolVal = (lineFields.at(1) == QStringLiteral("true"));
          convertedConfig[lineFields.at(0)] = boolVal;
        }
        else
        {
          convertedConfig[lineFields.at(0)] = lineFields.at(1);
        }
        modified = true;
      }
    }
  }

  if (modified)
  {
    qInfo() << "Successfully parsed legacy config file";
    writeJsonObj(convertedConfig);
  }
  else
  {
    qWarning() << "Failed to parse legacy config file - likely invalid";
  }
  return modified;
}

void Config::setValue(const QString& property, const QJsonValue& value)
{
  TRACE_METHOD();

  QJsonObject content = loadJson();
  content[property] = value;
  writeJsonObj(content);
}

QJsonValue Config::getValue(const QString& property)
{
  TRACE_METHOD();

  QJsonObject content = loadJson();
  return content[property];
}

void Config::removeValue(const QString& property)
{
  TRACE_METHOD();

  QJsonObject content = loadJson();
  content.remove(property);
  writeJsonObj(content);
}

int Config::getInt(const QString& property, int defaultValue)
{
  TRACE_METHOD();

  QJsonValue value = getValue(property);
  if (value.isDouble())
  {
    return value.toInt(defaultValue);
  }
  else
  {
    setValue(property, defaultValue);
  }

  return defaultValue;
}

QJsonObject Config::loadJson()
{
  TRACE_METHOD();

  QByteArray contentArr = loadByteArray();
  QJsonObject content;

  if (contentArr.length() > 0)
  {
    QJsonParseError parseError;

    QJsonDocument doc = QJsonDocument::fromJson(contentArr, &parseError);

    bool valid = true;
    if (parseError.error != QJsonParseError::NoError)
    {
      // Try to convert
      valid = parseLegacyConfig(contentArr);
    }

    if (valid)
    {
      content = doc.object();
    }
  }

  return content;
}

QByteArray Config::loadByteArray()
{
  TRACE_METHOD();
  return m_fileIO->readFileToByteArray(m_configFilePath);
}

void Config::writeJsonObj(const QJsonObject& jsonObj)
{
  TRACE_METHOD();

  m_fileIO->writeFileText(m_configFilePath, QJsonDocument(jsonObj).toJson());
}
