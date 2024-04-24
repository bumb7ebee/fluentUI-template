#include "AppInfo.h"

#include "../../Version.h"
#include <QDebug>
#include <QGuiApplication>
#include <QQmlContext>

AppInfo::AppInfo(QObject *parent) : QObject{parent} {
  version(APPLICATION_VERSION);
}

void AppInfo::testCrash() {
  auto *crash = reinterpret_cast<volatile int *>(0);
  *crash = 0;
}
