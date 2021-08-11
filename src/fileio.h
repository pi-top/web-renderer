#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>

class IFileIO
{
 public:
  virtual ~IFileIO() = default;
  virtual void deleteFile(const QString& filename) = 0;
  virtual void writeFileText(const QString& filename, const QString& text) = 0;
  virtual QStringList readFile(const QString& filename) = 0;
  virtual QByteArray readFileToByteArray(const QString& filename) = 0;
  virtual bool fileExists(const QString& filename) = 0;
  virtual QStringList directoryContents(const QString& directoryPath,
                                        const QString& filter) = 0;
  virtual QStringList directoryContents(const QString& directoryPath,
                                        const QStringList& filterList) = 0;
  virtual void touchFile(const QString& filename) = 0;
  virtual void copyPath(const QString& src, const QString& dst) = 0;
};

class FileIO : public QObject, public IFileIO
{
  Q_OBJECT

 public:
  explicit FileIO(QObject* parent = nullptr);

  Q_INVOKABLE void deleteFile(const QString& filename) override;
  Q_INVOKABLE void writeFileText(const QString& filename,
                                 const QString& text) override;
  Q_INVOKABLE QStringList readFile(const QString& filename) override;
  Q_INVOKABLE QByteArray readFileToByteArray(const QString& filename) override;
  Q_INVOKABLE bool fileExists(const QString& filename) override;
  Q_INVOKABLE QStringList directoryContents(const QString& directoryPath,
                                            const QString& filter) override;
  Q_INVOKABLE QStringList directoryContents(
      const QString& directoryPath, const QStringList& filterList) override;
  Q_INVOKABLE void touchFile(const QString& filename) override;
  Q_INVOKABLE void copyPath(const QString& src, const QString& dst) override;
};

#endif  // FILEIO_H
