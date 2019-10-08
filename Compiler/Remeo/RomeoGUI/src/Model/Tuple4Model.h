//
// Created by along on 19-5-11.
//

#ifndef ROMEO_TUPLE4MODEL_H
#define ROMEO_TUPLE4MODEL_H

#include <QAbstractTableModel>

namespace RmoLang {
    class LR1Context;
}

namespace Romeo {
    class Tuple4Model : public QAbstractTableModel {
    public:
        void build(RmoLang::LR1Context *context);

        int rowCount(const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    private:
        std::vector<std::tuple<std::string, std::string, std::string, std::string>> statements;
    };
}
#endif //ROMEO_TUPLE4MODEL_H
