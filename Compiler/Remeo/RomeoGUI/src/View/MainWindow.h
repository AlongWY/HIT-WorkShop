/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_MAINWINDOW_H
#define ROMEO_MAINWINDOW_H

#include "Model/GScanner.h"
#include "Model/FFModel.h"
#include "Model/PredictModel.h"
#include "Model/Tuple4Model.h"
#include <parser/LR1.h>
#include <QtWidgets/QMainWindow>

namespace Ui {
    class RomeoGUI;
}

namespace RmoLang {
    class LL1Context;

    class LR1Context;
}

namespace Romeo {
    class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);

        ~MainWindow() override;

    public slots:

        void OnOpen();

        void OnSave();

        void OnAnalyze();

    public:
        enum LOAD_TYPE {
            GRAMMER, CODE
        };

        void Open(const QString &filename);

        void Save(const QString &filename);

        void Open(LOAD_TYPE type, const QString &filename);

        void Save(LOAD_TYPE type, const QString &filename);

    private:
        using RmoContext = RmoLang::LR1Context;

        Ui::RomeoGUI *ui;

        GScanner *scanner;
        RmoContext *context;
        LR1 *parser;

        BaseTokenizer *tokenizer;
        FFModel<FIRST> *first;
        PredictModel *predict;
        Tuple4Model *tuple4Model;

        CFG::CFG *cfg;
    };
}


#endif //ROMEO_MAINWINDOW_H
