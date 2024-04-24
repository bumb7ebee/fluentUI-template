#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include "../stdafx.h"
#include <QFileSystemWatcher>
#include <QObject>
#include <QtQml/qqml.h>

class FileWatcher : public QObject {
  Q_OBJECT
  Q_PROPERTY_AUTO(QString, path);

public:
  explicit FileWatcher(QObject *parent = nullptr);
  Q_SIGNAL void fileChanged();

private:
  void clean();

private:
  QFileSystemWatcher _watcher;
};

#endif // FILEWATCHER_H
