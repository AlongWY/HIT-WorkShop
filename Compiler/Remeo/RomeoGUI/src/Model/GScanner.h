/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_GSCANNER_H
#define ROMEO_GSCANNER_H

#include <scanner/Scanner.h>
#include <scanner/Lex.h>
#include <scanner/Token.h>
#include <QAbstractTableModel>
#include <string>
#include <tuple>

namespace Romeo {

    class GScanner : public QAbstractTableModel, public Romeo::Scanner {
    Q_OBJECT
        using Scanner =  Romeo::Scanner;
    public:

        template<class... T>
        explicit GScanner(Tokenizer *tokenizer, QObject *parent = nullptr, T &&... t)
                :tokenizer(tokenizer), QAbstractTableModel(parent), Scanner(std::forward<T>(t)...) {
        }

        void reset_stream(std::istream *stream) override;

        void reset() override;

        Lex *next() final;

        int rowCount(const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        void display();

    private:
        void push_back(const Lex *lex);

        QString from_lex(const Lex *lex) const {
            if (lex == nullptr)
                return QString::fromStdString(tokenizer->str(Tokenizer::ENDFILE));
            else if (!lex->is_valid && lex->is_integer())
                return QString::fromStdString(tokenizer->str(Tokenizer::ENDFILE));
            else if (std::holds_alternative<std::string>(lex->unit))
                return QString::fromStdString((std::string) *lex);
            else if (lex->is_real())
                return QString::number((double) *lex);
            else if (lex->is_integer())
                return QString::number((int) (*lex));
            else
                return QString();
        }

        Romeo::Tokenizer *tokenizer;
        QVector<std::tuple<QString, QString, size_t, size_t, QColor>> tokens;
    };
}

#endif //ROMEO_GSCANNER_H
