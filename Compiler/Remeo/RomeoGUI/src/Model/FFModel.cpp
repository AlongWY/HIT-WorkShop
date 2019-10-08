/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "FFModel.h"

namespace Romeo {
    void Romeo::T_FFModel::build(const CFG::CFG *cfg_, const Romeo::Parser *pParser) {
        cfg = cfg_;
        parser = pParser;
        reset();
    }

    const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *T_FFModel::GetTable() const {
        return nullptr;
    }

    void T_FFModel::reset() {
        rows.clear();
        columns.clear();
        beginResetModel();
        for (const auto &key:cfg->NonTerminals())
            rows.push_back(key);

        for (const auto &key:cfg->Terminals())
            columns.push_back(key);
        endResetModel();
    }

    int T_FFModel::rowCount(const QModelIndex &parent) const {
        return rows.size();
    }

    int T_FFModel::columnCount(const QModelIndex &parent) const {
        return columns.size();
    }

    QVariant T_FFModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid())
            return QVariant();
        if (Qt::TextAlignmentRole == role) {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        } else if (Qt::DisplayRole == role) {
            auto row_key = rows[index.row()];
            auto column_key = columns[index.column()];
            auto row_data = GetTable()->find(parser->code(row_key));
            if (row_data == GetTable()->end()) {
                return false;
            }
            auto res = row_data->second.count(parser->code(column_key));
            if (res > 0) {
                return true;
            }
            return QVariant();
        }
        return QVariant();
    }

    QVariant T_FFModel::headerData(int section, Qt::Orientation orientation, int role) const {
        if (Qt::DisplayRole != role) {
            return QVariant();
        }
        if (orientation == Qt::Horizontal) {
            //横向头
            auto res = columns[section];
            return QString::fromStdString(parser->str(parser->code(res)));
        } else if (orientation == Qt::Vertical) {
            auto res = rows[section];
            return QString::fromStdString(res);
        }
        return QVariant();
    }

}