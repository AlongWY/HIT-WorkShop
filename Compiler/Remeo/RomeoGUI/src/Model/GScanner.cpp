/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "GScanner.h"
#include "scanner/Lex.h"

#include <QColor>
#include <QBrush>
#include <iostream>

namespace Romeo {
    void GScanner::reset_stream(std::istream *stream) {
        Scanner::reset_stream(stream);
        beginResetModel();
        tokens.clear();
        push_back((*this)());
        endResetModel();
    }

    void GScanner::reset() {
        Scanner::reset();
        beginResetModel();
        if (is_valid()) push_back(Scanner::operator()());
        tokens.clear();
        endResetModel();
    }

    Lex *GScanner::next() {
        auto res = Scanner::next();
        beginInsertRows(createIndex(0, 0), 0, 1);
        push_back(res);
        endInsertRows();
        return res;
    }

    int GScanner::rowCount(const QModelIndex &parent) const {
        return tokens.size();
    }

    int GScanner::columnCount(const QModelIndex &parent) const {
        return 4;
    }

    QVariant GScanner::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) return QVariant();
        if (Qt::TextAlignmentRole == role) {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        } else if (Qt::DisplayRole == role) {
            auto res = tokens.cbegin() + index.row();
            switch (index.column()) {
                case 0:
                    return std::get<0>(*res);
                case 1:
                    return std::get<1>(*res);
                case 2:
                    return (unsigned) std::get<2>(*res);
                case 3:
                    return (unsigned) std::get<3>(*res);
                default:
                    return QVariant();
            }
        } else if (role == Qt::ForegroundRole) {
            int row = index.row();
            return QBrush(std::get<QColor>(tokens[row]));
        }
        return QVariant();
    }

    QVariant GScanner::headerData(int section, Qt::Orientation orientation, int role) const {
        // 单词种别码，自身值
        if (Qt::DisplayRole != role) {
            return QVariant();
        }
        if (orientation == Qt::Horizontal) {
            //横向头
            switch (section) {
                case 0:
                    return tr("Type");
                case 1:
                    return tr("Value");
                case 2:
                    return tr("Row");
                case 3:
                    return tr("Column");
                default:
                    return QVariant();
            }
        } else if (orientation == Qt::Vertical) {
            return section + 1;                   //纵向头依次递增
        }
        return QVariant();
    }

    void GScanner::push_back(const Lex *lex) {
        auto code = tokenizer->code(lex);
        tokens.push_back(std::make_tuple(
                QString::fromStdString(tokenizer->non_str(code)),from_lex(lex),
                lex == nullptr ? 0 : lex->row, lex == nullptr ? 0 : lex->column,
                tokenizer->code(lex) == Tokenizer::UNKNOWN ? QColor("Red") : QColor("Black")));
    }

    void GScanner::display() {
        for (auto[type, value, row, column, color]:tokens) {
            std::cout << "Value:" << value.toStdString() <<
                      "\tRow:" << row <<
                      "\tColumn:" << column <<
                      "\tType:" << type.toStdString() <<
                      std::endl;
        }
    }
}