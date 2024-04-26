import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

Item {
    id: app

    Connections {
        target: FluTheme
        function onDarkModeChanged() {
            SettingsHelper.saveDarkMode(FluTheme.darkMode)
        }
    }

    Connections {
        target: FluApp
        function onUseSystemAppBarChanged() {
            SettingsHelper.saveUseSystemAppBar(FluApp.useSystemAppBar)
        }
    }

    Connections {
        target: TranslateHelper
        function onCurrentChanged() {
            SettingsHelper.saveLanguage(TranslateHelper.current)
        }
    }

    Component.onCompleted: {
        FluNetwork.openLog = false
        FluNetwork.setInterceptor(function (param) {
            param.addHeader("Token", "000000000000000000000")
        })
        FluApp.init(app, Qt.locale(TranslateHelper.current))
        FluApp.windowIcon = "qrc:/res/image/favicon.ico"
        FluApp.useSystemAppBar = SettingsHelper.getUseSystemAppBar()
        FluTheme.darkMode = SettingsHelper.getDarkMode()
        FluTheme.enableAnimation = true
        FluApp.routes = {
            "/": "qrc:/qml/fluentUI/window/MainWindow.qml",
            "/about": "qrc:/qml/fluentUI/window/AboutWindow.qml",
            "/login": "qrc:/qml/fluentUI/window/LoginWindow.qml",
            "/hotload": "qrc:/qml/fluentUI/window/HotloadWindow.qml",
            "/crash": "qrc:/qml/fluentUI/window/CrashWindow.qml",
            "/singleTaskWindow": "qrc:/qml/fluentUI/window/SingleTaskWindow.qml",
            "/standardWindow": "qrc:/qml/fluentUI/window/StandardWindow.qml",
            "/singleInstanceWindow": "qrc:/qml/fluentUI/window/SingleInstanceWindow.qml",
            "/pageWindow": "qrc:/qml/fluentUI/window/PageWindow.qml"
        }
        var args = Qt.application.arguments
        if (args.length >= 2 && args[1].startsWith("-crashed=")) {
            FluApp.navigate("/crash", {
                                "crashFilePath": args[1].replace("-crashed=",
                                                                 "")
                            })
        } else {
            FluApp.navigate("/")
        }
    }
}
