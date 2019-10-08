//
// Created by along on 19-4-6.
//

#ifndef ROMEO_LEX_H
#define ROMEO_LEX_H

#include <ostream>
#include <string>
#include <variant>

namespace Romeo {
    struct Lex {
        template<typename ...Args>
        Lex(size_t row, size_t column, bool valid, Args &&...args)
                : row(row), column(column), is_valid(valid), unit(std::forward<Args>(args)...) {};

        Lex() : Lex(0, 0, false) {};

        bool is_unit() const {
            if (std::holds_alternative<std::string>(unit)) {
                char first = std::get<std::string>(unit).front();
                return isalpha(first) || first == '_';
            } else
                return false;
        };

        bool is_punctuation() const {
            try {
                char first = std::get<std::string>(unit).front();
                return ispunct(first) && first != '_';
            } catch (std::exception &e) {
                return false;
            }
        }

        bool is_string() const {
            return std::holds_alternative<std::string>(unit)
                   && std::get<std::string>(unit).front() == '\"';
        };

        bool is_integer() const { return std::holds_alternative<int>(unit); };

        bool is_real() const { return std::holds_alternative<double>(unit); };

        std::string to_string() const {
            return std::get<std::string>(unit);
        }

        int to_integer() const {
            return std::get<int>(unit);
        }

        double to_real() const {
            return std::get<double>(unit);
        }

        explicit operator std::string() const {
            return std::get<std::string>(unit);
        }

        explicit operator int() const {
            return std::get<int>(unit);
        }

        explicit operator double() const {
            return std::get<double>(unit);
        }

        bool is_valid;
        const size_t row;                             // Token位置
        const size_t column;                          // Token位置
        std::variant<std::string, int, double> unit;  // value

        friend std::ostream &operator<<(std::ostream &os, const Lex &lex) {
            os << "row: " << lex.row << " column: " << lex.column << " valid: " << lex.is_valid;
            if (lex.is_unit())
                os << "   unit: " << std::get<std::string>(lex.unit);
            else if (lex.is_punctuation())
                os << "   punc: " << std::get<std::string>(lex.unit);
            else if (lex.is_string())
                os << " string: " << std::get<std::string>(lex.unit);
            else if (lex.is_integer())
                os << "    int: " << std::get<int>(lex.unit);
            else if (lex.is_real())
                os << "   real: " << std::get<double>(lex.unit);

            return os;
        }
    };


    template<typename, typename>
    struct lex_serializer {
        template<typename ValueType>
        static auto from_lex(Lex &&j, ValueType &val) noexcept(
        noexcept(from_lex(std::forward<Lex>(j), val)))
        -> decltype(from_lex(std::forward<Lex>(j), val), void()) {
            from_lex(std::forward<Lex>(j), val);
        }

        template<typename ValueType>
        static auto to_lex(Lex &j, ValueType &&val) noexcept(
        noexcept(to_lex(j, std::forward<ValueType>(val))))
        -> decltype(to_lex(j, std::forward<ValueType>(val)), void()) {
            to_lex(j, std::forward<ValueType>(val));
        }
    };
}

#endif //ROMEO_LEX_H
