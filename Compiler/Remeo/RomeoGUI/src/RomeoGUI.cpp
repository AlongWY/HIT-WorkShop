/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "View/MainWindow.h"
#include <QApplication>
#include <QtCore/QTranslator>
#include <QCommandLineParser>
#include <QDebug>


using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon(":ICON"));
    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("lang"),
                        QLatin1String("_"), QLatin1String("lang"))) {
        QApplication::installTranslator(&translator);
    }
    Romeo::MainWindow window;

    // 解析工具
    QCommandLineParser parser;
    parser.setApplicationDescription("The GUI interface of Romeo Language Compiler.");
    const QCommandLineOption helpOption = parser.addHelpOption();

    parser.addOptions({
                              {
                                      {"g", "grammer"},
                                      QCoreApplication::translate("main", "Grammer file to load."),
                                      QCoreApplication::translate("main", "filename")
                              },
                              {
                                      {"c", "code"},
                                      QCoreApplication::translate("main", "Code file to parse."),
                                      QCoreApplication::translate("main", "filename")
                              }
                      });

    parser.parse(QApplication::arguments());

    if (parser.isSet(helpOption)) {
        parser.showHelp();
    } else {
        if (parser.isSet("grammer")) {
            window.Open(Romeo::MainWindow::GRAMMER, parser.value("grammer"));
        }
        if (parser.isSet("code")) {
            window.Open(Romeo::MainWindow::CODE, parser.value("code"));
        }
        window.show();
    }

    return QApplication::exec();
}