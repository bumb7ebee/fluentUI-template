/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 İlker Yasin AKSOY
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <QDir>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QNetworkProxy>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSslConfiguration>
#include <QtQml/qqmlextensionplugin.h>

#include "fluentUI/helper/InitializrHelper.h"
#include "fluentUI/helper/Log.h"
#include "fluentUI/helper/Network.h"
#include "fluentUI/helper/SettingsHelper.h"
#include "fluentUI/helper/TranslateHelper.h"

#include "fluentUI/component/CircularReveal.h"
#include "fluentUI/component/FileWatcher.h"
#include "fluentUI/component/FpsItem.h"
#include "fluentUI/component/OpenGLItem.h"

#include "../Version.h"
#include "fluentUI/AppInfo.h"

#ifdef WIN32
#include "fluentUI/app_dmp.h"
#endif

int main(int argc, char *argv[]) {
    
  const char *uri = "example";
  int major = 1;
  int minor = 0;

#ifdef WIN32
  ::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
  qputenv("QT_QPA_PLATFORM", "windows:darkmode=2");
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
#else
  qputenv("QT_QUICK_CONTROLS_STYLE", "Default");
#endif
#ifdef Q_OS_LINUX
  // fix bug UOSv20 does not print logs
  qputenv("QT_LOGGING_RULES", "");
  // fix bug UOSv20 v-sync does not work
  qputenv("QSG_RENDER_LOOP", "basic");
#endif

  QGuiApplication::setOrganizationName("Bumb7ebee");
  QGuiApplication::setOrganizationDomain("https://ilkeraksoy.github.io/");
  QGuiApplication::setApplicationName("FluentUI");
  QGuiApplication::setApplicationDisplayName("FluentUI Example");
  QGuiApplication::setApplicationVersion(APPLICATION_VERSION);
  QGuiApplication::setQuitOnLastWindowClosed(false);

  SettingsHelper::getInstance()->init(argv);
  Log::setup(argv, uri);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

  QGuiApplication app(argc, argv);

  //@uri example
  qmlRegisterType<CircularReveal>(uri, major, minor, "CircularReveal");
  qmlRegisterType<FileWatcher>(uri, major, minor, "FileWatcher");
  qmlRegisterType<FpsItem>(uri, major, minor, "FpsItem");
  qmlRegisterType<NetworkCallable>(uri,major,minor,"NetworkCallable");
  qmlRegisterType<NetworkParams>(uri,major,minor,"NetworkParams");
  qmlRegisterType<OpenGLItem>(uri,major,minor,"OpenGLItem");
  qmlRegisterUncreatableMetaObject(NetworkType::staticMetaObject, uri, major, minor, "NetworkType", "Access to enums & flags only");

  QQmlApplicationEngine engine;
  TranslateHelper::getInstance()->init(&engine);

  engine.rootContext()->setContextProperty("AppInfo", AppInfo::getInstance());
  engine.rootContext()->setContextProperty("SettingsHelper",
                                           SettingsHelper::getInstance());
  engine.rootContext()->setContextProperty("InitializrHelper",
                                           InitializrHelper::getInstance());
  engine.rootContext()->setContextProperty("TranslateHelper",
                                           TranslateHelper::getInstance());
  engine.rootContext()->setContextProperty("Network",Network::getInstance());

  const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);
  const int exec = QGuiApplication::exec();
  if (exec == 931) {
    QProcess::startDetached(qApp->applicationFilePath(), qApp->arguments());
  }

  return exec;
}