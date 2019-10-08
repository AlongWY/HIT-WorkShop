/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include <tuple>
#include <utils/Utility.h>
#include "PredictModel.h"

namespace Romeo {
    void Romeo::PredictModel::build(const CFG::CFG *cfg_, const Romeo::LR1 *ll1_) {
        cfg = cfg_;
        parser = ll1_;
        reset();
    }

    void PredictModel::reset() {
        columns.clear();
        beginResetModel();
        if (cfg) {
            for (const auto &key:cfg->Terminals())
                columns.push_back(parser->code(key));
            columns.push_back(Tokenizer::ENDFILE);
            columns.push_back(Tokenizer::IDENTIFIER);
            columns.push_back(Tokenizer::INTEGER);
            columns.push_back(Tokenizer::REAL);
            goto_size = columns.size();
            for (const auto &key:cfg->NonTerminals())
                columns.push_back(parser->code(key));
        }
        endResetModel();
    }

    int PredictModel::rowCount(const QModelIndex &parent) const {
        return parser ? parser->Items().size() : 0;
    }

    int PredictModel::columnCount(const QModelIndex &parent) const {
        return columns.size();
    }

    QVariant PredictModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid())
            return QVariant();
        if (Qt::TextAlignmentRole == role) {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        } else if (Qt::DisplayRole == role) {
            auto unknown_action = std::make_tuple(LR1::UNKNOWN, size_t(), size_t());

            auto X = columns[index.column()];

            if (index.column() < goto_size) {
                auto actions = parser->Actions().find(index.row());
                auto next = GetWithDef(actions->second, X, unknown_action);

                auto &[action, state, body] = next;
                switch (action) {
                    case LR1::STACK: { // 移进
                        return QString("s %1").arg(state);
                    }
                    case LR1::REDUCE: { // 归约 (state, body) -> (product, body)
                        return QString("r %1,%2").arg(state).arg(body);
                    }
                    case LR1::ACCEPT: { // 接受
                        return "ACCEPT";
                    }
                    default: {// 遇到错误
                        return QVariant();
                    }
                }

            } else {
                auto goto_ = parser->Gotos().find(index.row());
                auto go_state = GetWithDef(goto_->second, X, -1);
                if (go_state != -1)return go_state;
                else return QVariant();
            }
            return QVariant();
        }
        return QVariant();
    }

    QVariant PredictModel::headerData(int section, Qt::Orientation orientation, int role) const {
        if (Qt::DisplayRole != role) {
            return QVariant();
        }
        if (orientation == Qt::Horizontal) {
            //横向头
            auto res = columns[section];
            return QString::fromStdString(parser->str(res));
        } else if (orientation == Qt::Vertical) {
            return section;
        }
        return QVariant();
    }

}