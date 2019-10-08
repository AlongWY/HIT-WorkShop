//
// Created by along on 19-4-7.
//

#include "RmoDefine.h"

namespace RmoLang {

    const std::map<std::string, std::string> &Definations() {
        const static std::map<std::string, std::string> operators = {
                {"BREAK",           "break"},
                {"CLASS",           "class"},
                {"CONTINUE",        "continue"},
                {"DEF",             "def"},
                {"ELSE",            "else"},
                {"FALSE",           "false"},
                {"FOR",             "for"},
                {"IF",              "if"},
                {"IMPORT",          "import"},
                {"IS",              "is"},
                {"NONE",            "none"},
                {"RETURN",          "return"},
                {"STATIC",          "static"},
                {"SUPER",           "super"},
                {"THIS",            "this"},
                {"TRUE",            "true"},
                {"WHILE",           "while"},

                {"INT",             "int"},
                {"BOOL",            "bool"},
                {"REAL",            "real"},
                {"STRING",          "string"},

                {"NOT_EQUAL",       "!="},
                {"LOGIC_NOT",       "!"},
                {"MOD",             "%"},
                {"LOGIC_AND",       "&&"},
                {"BIT_AND",         "&"},
                {"BIT_XOR",         "^"},
                {"LEFT_PARAN",      "("},
                {"RIGHT_PARAN",     ")"},
                {"MUL",             "*"},
                {"ADD",             "+"},
                {"COMMA",           ","},
                {"SUB",             "-"},
                {"DOT",             "."},
                {"DIV",             "/"},
                {"COLON",           ":"},
                {"SEMICOLON",       ";"},
                {"BIT_SHIFT_LEFT",  "<<"},
                {"LESS_EQUAL",      "<="},
                {"LESS",            "<"},
                {"EQUAL",           "=="},
                {"ASSIGN",          "="},
                {"BIT_SHIFT_RIGHT", ">>"},
                {"GRATE_EQUAL",     ">="},
                {"GRATE",           ">"},
                {"QUESTION",        "?"},
                {"LEFT_BRACKET",    "["},
                {"RIGHT_BRACKET",   "]"},
                {"LEFT_BRACE",      "{"},
                {"LOGIC_OR",        "||"},
                {"BIT_OR",          "|"},
                {"RIGHT_BRACE",     "}"},
                {"BIT_NOT",         "~"},
        };
        return operators;
    }
}