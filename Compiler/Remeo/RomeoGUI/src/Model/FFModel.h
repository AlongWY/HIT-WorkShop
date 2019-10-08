/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_FIRSTMODEL_H
#define ROMEO_FIRSTMODEL_H

#include <cfg/CFG.h>
#include <parser/Parser.h>
#include <QAbstractTableModel>

namespace Romeo {

    class T_FFModel : public QAbstractTableModel {
    Q_OBJECT
    public:
        T_FFModel() : cfg(nullptr), parser(nullptr) {};

        void build(const CFG::CFG *cfg_, const Parser *pParser);

        virtual const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *GetTable() const;

        virtual void reset();

        int rowCount(const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    protected:
        const CFG::CFG *cfg;
        const Parser *parser;

        std::vector<std::string> rows;
        std::vector<std::string> columns;
    };


    enum TableType {
        FIRST,
        FOLLOW
    };

    template<TableType Type>
    class FFModel {
        // nothing
    };

    template<>
    class FFModel<FIRST> : public T_FFModel {
    public:
        void reset() override {
            T_FFModel::reset();
        }

        const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *GetTable() const override {
            return parser->First();
        }
    };

    template<>
    class FFModel<FOLLOW> : public T_FFModel {
    public:
        const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *GetTable() const override {
            return parser->Follow();
        }
    };

}

#endif //ROMEO_FIRSTMODEL_H
