#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>

class IFileIO
{
 public:
  virtual ~IFileIO() = default;
  virtual void createDirectory(const QString& filename) = 0;
  virtual void deleteFile(const QString& filename) = 0;
  virtual void writeFileText(const QString& filename, const QString& text) = 0;
  virtual void writeFileTextRaw(const QString& filename,
                                const QString& text) = 0;
  virtual void writeFileLines(const QString& filename,
                              const QStringList& textLines) = 0;
  virtual void writeFileBytes(const QString& filename,
                              const QByteArray& text) = 0;
  virtual void appendFileText(const QString& filename, const QString& text) = 0;
  virtual void appendFileTextInternal(const QString& filename,
                                      const QString& text) = 0;
  virtual void moveFile(const QString& currentFilename,
                        const QString& newFilename) = 0;
  virtual QStringList readFile(const QString& filename) = 0;
  virtual QByteArray readFileToByteArray(const QString& filename) = 0;
  virtual QString readFileToString(const QString& filename) = 0;
  virtual bool fileExists(const QString& filename) = 0;
  virtual QStringList directoryContents(const QString& directoryPath,
                                        const QString& filter) = 0;
  virtual QStringList directoryContents(const QString& directoryPath,
                                        const QStringList& filterList) = 0;
  virtual void touchFile(const QString& filename) = 0;
  virtual void copyPath(const QString& src, const QString& dst) = 0;
  virtual bool directoryIsEmpty(const QString& directory) = 0;
  virtual bool copyFile(const QString& src, const QString& dst) = 0;
  virtual bool directoryExists(const QString& path) = 0;
};

class FileIO : public QObject, public IFileIO
{
  Q_OBJECT

 public:
  explicit FileIO(QObject* parent = nullptr);

  void createDirectory(const QString& filename) override;
  Q_INVOKABLE void deleteFile(const QString& filename) override;
  Q_INVOKABLE void writeFileText(const QString& filename,
                                 const QString& text) override;
  Q_INVOKABLE void writeFileTextRaw(const QString& filename,
                                    const QString& text) override;
  Q_INVOKABLE void writeFileLines(const QString& filename,
                                  const QStringList& textLines) override;
  Q_INVOKABLE void writeFileBytes(const QString& filename,
                                  const QByteArray& text) override;
  Q_INVOKABLE void appendFileText(const QString& filename,
                                  const QString& text) override;
  void appendFileTextInternal(const QString& filename,
                              const QString& text) override;
  Q_INVOKABLE void moveFile(const QString& currentFilename,
                            const QString& newFilename) override;
  Q_INVOKABLE QStringList readFile(const QString& filename) override;
  Q_INVOKABLE QByteArray readFileToByteArray(const QString& filename) override;
  Q_INVOKABLE QString readFileToString(const QString& filename) override;
  Q_INVOKABLE bool fileExists(const QString& filename) override;
  Q_INVOKABLE QStringList directoryContents(const QString& directoryPath,
                                            const QString& filter) override;
  Q_INVOKABLE QStringList directoryContents(
      const QString& directoryPath, const QStringList& filterList) override;
  Q_INVOKABLE void touchFile(const QString& filename) override;
  Q_INVOKABLE void copyPath(const QString& src, const QString& dst) override;
  Q_INVOKABLE bool directoryIsEmpty(const QString& directory) override;
  Q_INVOKABLE bool copyFile(const QString& src, const QString& dst) override;
  Q_INVOKABLE bool directoryExists(const QString& path) override;
};

#endif  // FILEIO_H