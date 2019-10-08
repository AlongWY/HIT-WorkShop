/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_PREDICTMODEL_H
#define ROMEO_PREDICTMODEL_H

#include <cfg/CFG.h>
#include <QAbstractTableModel>

#include <parser/LR1.h>
#include <sstream>
#include <QString>

namespace Romeo {
    class LR1;

    class PredictModel : public QAbstractTableModel {
    Q_OBJECT
    public:
        PredictModel() : cfg(nullptr), parser(nullptr), goto_size(0) {}

        void build(const CFG::CFG *cfg_, const LR1 *ll1_);

        void reset();

        int rowCount(const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    private:
        int goto_size;
        const LR1 *parser;
        const CFG::CFG *cfg;

        std::vector<Tokenizer::Code> columns;
    };
}

#endif //ROMEO_PREDICTMODEL_H
