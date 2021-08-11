#include "fileio.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

#ifndef SKIP_COMMON_LOGGING
#include "ptlogger.h"
#endif

#ifndef TRACE_METHOD
#define TRACE_METHOD()
#endif

FileIO::FileIO(QObject* parent) : QObject(parent)
{
}

void FileIO::createDirectory(const QString& filename)
{
  TRACE_METHOD();

  QDir dir;

  if (dir.exists(filename) == false)
  {
    dir.mkpath(filename);
  }
}

void FileIO::deleteFile(const QString& filename)
{
  TRACE_METHOD();

  QFile file(filename);

  if (file.exists())
  {
    file.remove();
  }

  if (fileExists(filename))
  {
    qWarning() << "An attempt to delete" << filename << "failed.";
  }
}

void FileIO::writeFileText(const QString& filename, const QString& text)
{
  TRACE_METHOD();

  deleteFile(filename);
  QFile file(filename);

  if (file.open(QIODevice::ReadWrite))
  {
    QTextStream stream(&file);
    stream << text << endl;
    file.close();
  }
}

void FileIO::writeFileTextRaw(const QString& filename, const QString& text)
{
  TRACE_METHOD();

  deleteFile(filename);
  QFile file(filename);

  if (file.open(QIODevice::ReadWrite))
  {
    QTextStream stream(&file);
    stream << text;
    file.close();
  }
}

void FileIO::writeFileLines(const QString& filename,
                            const QStringList& textLines)
{
  TRACE_METHOD();

  deleteFile(filename);
  QFile file(filename);

  if (file.open(QIODevice::ReadWrite))
  {
    QTextStream stream(&file);
    foreach (const QString& line, textLines)
    {
      stream << line << endl;
    }

    file.close();
  }
}

void FileIO::writeFileBytes(const QString& filename, const QByteArray& text)
{
  TRACE_METHOD();

  deleteFile(filename);
  QFile file(filename);

  if (file.open(QIODevice::ReadWrite))
  {
    file.write(text);
    file.close();
  }
}

void FileIO::moveFile(const QString& currentFilename,
                      const QString& newFilename)
{
  TRACE_METHOD();

  if (QFile::copy(currentFilename, newFilename))
  {
    deleteFile(currentFilename);
  }
}

bool FileIO::fileExists(const QString& filename)
{
  TRACE_METHOD();

  QFile file(filename);
  return file.exists();
}

QStringList FileIO::directoryContents(const QString& directoryPath,
                                      const QString& filter)
{
  QStringList strList = {filter};
  return directoryContents(directoryPath, strList);
}

QStringList FileIO::directoryContents(const QString& directoryPath,
                                      const QStringList& filterList)
{
  QDir dir(directoryPath);
  return dir.entryList(filterList);
}

void FileIO::touchFile(const QString& filename)
{
  TRACE_METHOD();

  QFile file(filename);
  if (file.exists() == false)
  {
    // Create empty file
    if (file.open(QIODevice::ReadWrite))
    {
      file.write("");
      file.close();
    }
  }
}

QStringList FileIO::readFile(const QString& filename)
{
  QFile file(filename);
  QStringList strings;
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    while (in.atEnd() == false)
    {
      strings += in.readLine();
    }
    file.close();
  }
  return strings;
}

QByteArray FileIO::readFileToByteArray(const QString& filename)
{
  TRACE_METHOD();

  QFile file(filename);
  QByteArray byteArray;
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    while (in.atEnd() == false)
    {
      byteArray.append(in.readLine());
    }
    file.close();
  }
  return byteArray;
}

QString FileIO::readFileToString(const QString& filename)
{
  TRACE_METHOD();

  QStringList strings = readFile(filename);
  return strings.join(QStringLiteral("\n"));
}

void FileIO::copyPath(const QString& src, const QString& dst)
{
  TRACE_METHOD();

  QDir dir(src);
  if (dir.exists() == false)
  {
    return;
  }

  foreach (const QString& d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
  {
    QString dst_path = dst + QDir::separator() + d;
    dir.mkpath(dst_path);
    copyPath(src + QDir::separator() + d, dst_path);
  }

  foreach (const QString& f, dir.entryList(QDir::Files))
  {
    deleteFile(dst + QDir::separator() + f);
    QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
  }
}

bool FileIO::directoryIsEmpty(const QString& directory)
{
  TRACE_METHOD();

  return (QDir(directory)
              .entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)
              .count() == 0);
}

bool FileIO::directoryExists(const QString& path)
{
  QDir dir(path);
  return dir.exists();
}
