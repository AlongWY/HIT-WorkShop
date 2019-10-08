//
// Created by along on 19-4-5.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cfg/CFG.h>
#include <functional>

#include <scanner/Scanner.h>
#include <scanner/Token.h>
#include <parser/LL1.h>
#include <parser/LR1.h>
#include <context/BaseContext.h>
#include <utils/Reflection.h>

#include "LR1Context.h"
#include "RmoDefine.h"

using namespace std;
using namespace Romeo;
using namespace RmoLang;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << argv[0] << " file_grammer file_code" << endl;
        return 0;
    }

    ifstream is_grammer(argv[1]);
    if (!is_grammer) cout << "Grammer file Error!!!" << endl;

    ifstream is_code(argv[2]);
    if (!is_code) cout << "Code file Error!!!" << endl;

    CFG::CFG cfg("EPS", is_grammer);
    Scanner scanner(&is_code);

    auto tokenizer = new BaseTokenizer();
    auto context = new LR1Context();

    for (const auto &[key, value]:Definations()) {
        if (cfg.Terminals().count(key)) {
            tokenizer->add_terminal(key, value);
            scanner.add_op(value);
        }
    }

    for (const auto &n_term:cfg.NonTerminals()) {
        tokenizer->add_non_terminal(n_term);
    }

    using Ref = REFLECTION(LR1Context, LR1Context::REF_TYPE);
    for (const auto &act:cfg.Actions()) {
        context->push_back(tokenizer->add_action(act), Ref::Ref(act)(context));
    }

    tokenizer->add_non_terminal("EPS", Tokenizer::EPSILON);
    tokenizer->add_non_terminal("EOF", Tokenizer::ENDFILE);
    tokenizer->add_non_terminal("UNKNOWN", Tokenizer::UNKNOWN);
    tokenizer->add_non_terminal("UNIT", Tokenizer::IDENTIFIER);
    tokenizer->add_non_terminal("INT_VAL", Tokenizer::INTEGER);
    tokenizer->add_non_terminal("REAL_VAL", Tokenizer::REAL);
    tokenizer->add_non_terminal("STRING_VAL", Tokenizer::STRING);

    LR1 parser_(&scanner, tokenizer, &cfg);
    parser_.init_grammer();
    parser_.parse(context);

    std::cerr << "\n四元式:" << endl;
    context->display_statements(cerr);
    std::cerr << "\n符号表:" << endl;
    context->display_symbol_table(cerr);

    delete tokenizer;
    delete context;

    return 0;
}