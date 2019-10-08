/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "MainWindow.h"
#include "ui_RomeoGUI.h"

#include <sstream>
#include <iomanip>
#include <QDebug>
#include <QFileDialog>

#include <RmoDefine.h>
#include <parser/LR1.h>
#include <LR1Context.h>
#include <utils/Reflection.h>

using namespace Romeo;
using namespace RmoLang;

namespace Romeo {
    MainWindow::MainWindow(QWidget *parent) :
            QMainWindow(parent), ui(new Ui::RomeoGUI), parser(nullptr),
            scanner(nullptr), context(nullptr), first(nullptr),
            tokenizer(nullptr), predict(nullptr), cfg(nullptr),
            tuple4Model(nullptr) {
        ui->setupUi(this);

        cfg = new CFG::CFG("EPS");
        tokenizer = new BaseTokenizer();
        scanner = new GScanner(tokenizer);
        parser = new LR1(scanner, tokenizer, cfg);

        first = new FFModel<FIRST>();
        predict = new PredictModel();
        context = new LR1Context();

        first->build(cfg, parser);
        predict->build(cfg, parser);
        tuple4Model = new Tuple4Model();

        auto set_view = [](QTableView *view, QAbstractTableModel *model, QHeaderView::ResizeMode mode) {
            view->setModel(model);
            view->horizontalHeader()->setSectionResizeMode(mode);
            view->setSelectionMode(QAbstractItemView::SingleSelection);
        };

        set_view(ui->tokenTable, scanner, QHeaderView::Stretch);
        set_view(ui->first_table, first, QHeaderView::ResizeToContents);
        set_view(ui->predict_table, predict, QHeaderView::ResizeToContents);
        set_view(ui->tuple4Table, tuple4Model, QHeaderView::Stretch);

        ui->actionOpen->setIcon(QIcon::fromTheme("document-open"));
        ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
        connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OnOpen);
        connect(ui->actionSave, &QAction::triggered, this, &MainWindow::OnSave);
        connect(ui->actionAnalyze, &QAction::triggered, this, &MainWindow::OnAnalyze);
    }

    MainWindow::~MainWindow() {
        delete cfg;
        delete tokenizer;
        delete scanner;
        delete first;
        delete predict;
        delete context;
        delete ui;
    }

    void MainWindow::OnOpen() {
        QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Text (*.txt)");
        Open(filename);
    }

    void MainWindow::Open(const QString &filename) {
        if (ui->codeTab->isActiveWindow()) {
            Open(CODE, filename);
        } else {
            Open(GRAMMER, filename);
        }
    }

    void MainWindow::Open(LOAD_TYPE type, const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Can't open the file!" << endl;
            return;
        }

        QTextStream in(&file);
        switch (type) {
            case CODE:
                ui->codeEdit->setPlainText(in.readAll());
                break;
            case GRAMMER:
                ui->cfgEdit->setPlainText(in.readAll());
                break;
            default:
                break;
        }
    }

    void MainWindow::OnSave() {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", "Text (*.txt)");
        Save(filename);
    }

    void MainWindow::Save(const QString &filename) {
        if (ui->codeTab->isActiveWindow()) {
            Save(CODE, filename);
        } else {
            Save(GRAMMER, filename);
        }
    }

    void MainWindow::Save(LOAD_TYPE type, const QString &filename) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Can't open the file!" << endl;
            return;
        }

        QTextStream out(&file);
        switch (type) {
            case CODE:
                out << ui->codeEdit->toPlainText();
                break;
            case GRAMMER:
                out << ui->cfgEdit->toPlainText();
                break;
            default:
                break;
        }
    }

    void MainWindow::OnAnalyze() {
        tokenizer->reset();
        {
            qDebug() << "build cfg";
            auto text = ui->cfgEdit->toPlainText();
            std::istringstream stream(text.toStdString());
            cfg->build(stream);

            for (const auto &[non, term]:Definations()) {
                tokenizer->add_terminal(non, term);
                if (ispunct(term.at(0))) {
                    scanner->add_op(term);
                }
            }
            for (const auto &n_term:cfg->NonTerminals())
                tokenizer->add_non_terminal(n_term);

            using Ref = REFLECTION(RmoContext, RmoContext::REF_TYPE);
            for (const auto &act:cfg->Actions()) {
                context->push_back(tokenizer->add_action(act), Ref::Ref(act)(context));
            }

            tokenizer->add_non_terminal("EPS", Tokenizer::EPSILON);
            tokenizer->add_non_terminal("EOF", Tokenizer::ENDFILE);
            tokenizer->add_non_terminal("UNKNOWN", Tokenizer::UNKNOWN);
            tokenizer->add_non_terminal("UNIT", Tokenizer::IDENTIFIER);
            tokenizer->add_non_terminal("INT_VAL", Tokenizer::INTEGER);
            tokenizer->add_non_terminal("REAL_VAL", Tokenizer::REAL);
            tokenizer->add_non_terminal("STRING_VAL", Tokenizer::STRING);

        }
        {
            qDebug() << "build token";
            auto text = ui->codeEdit->toPlainText();
            std::istringstream is_code(text.toStdString());
            scanner->reset_stream(&is_code);

            {
                qDebug() << "build grammer";
                parser->init_grammer();
                parser->parse(context);
            }

            first->reset();
            predict->reset();

            ui->reduceTable->clear();
            for (const auto &str:context->get_reduces()) {
                ui->reduceTable->addItem(QString::fromStdString(str));
            }

            tuple4Model->build(context);
            std::ostringstream symbols;
            context->display_symbol_table(symbols);
            ui->symbolsBrowser->setText(QString::fromStdString(symbols.str()));
        }
    }

}