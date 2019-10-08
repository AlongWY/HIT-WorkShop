//
// Created by along on 19-5-11.
//

#include "Tuple4Model.h"
#include <LR1Context.h>


namespace Romeo {
    void Tuple4Model::build(RmoLang::LR1Context *context) {
        beginResetModel();
        statements.clear();
        for (auto &[block_id, op, arg1, arg2, res] :context->Statements()) {
            if (res != nullptr && typeid(*res.get()) == typeid(Romeo::ValueMeta)) {
                statements.emplace_back(op, arg1, arg2, std::dynamic_pointer_cast<Romeo::ValueMeta>(res)->value);
            } else if (res != nullptr && typeid(*res.get()) == typeid(Romeo::JmpMeta)) {
                statements.emplace_back(op, arg1, arg2,
                                        std::to_string(std::dynamic_pointer_cast<Romeo::JmpMeta>(res)->jmp_to));
            }
        }
        endResetModel();
    }

    int Tuple4Model::rowCount(const QModelIndex &parent) const {
        return statements.size();
    }

    int Tuple4Model::columnCount(const QModelIndex &parent) const {
        return 4;
    }

    QVariant Tuple4Model::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) return QVariant();
        if (Qt::TextAlignmentRole == role) {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        } else if (Qt::DisplayRole == role) {
            auto res = statements.cbegin() + index.row();
            switch (index.column()) {
                case 0:
                    return QString::fromStdString(std::get<0>(*res));
                case 1:
                    return QString::fromStdString(std::get<1>(*res));
                case 2:
                    return QString::fromStdString(std::get<2>(*res));
                case 3:
                    return QString::fromStdString(std::get<3>(*res));
                default:
                    return QVariant();
            }
        }
        return QVariant();
    }

    QVariant Tuple4Model::headerData(int section, Qt::Orientation orientation, int role) const {
        if (Qt::DisplayRole != role) {
            return QVariant();
        }
        if (orientation == Qt::Horizontal) {
            //横向头
            switch (section) {
                case 0:
                    return tr("Operation");
                case 1:
                    return tr("Arg1");
                case 2:
                    return tr("Arg2");
                case 3:
                    return tr("Res");
                default:
                    return QVariant();
            }
        } else if (orientation == Qt::Vertical) {
            return section + 1;                   //纵向头依次递增
        }
        return QVariant();
    }
}