//
// Created by along on 19-4-17.
//

#ifndef ROMEO_LR1CONTEXT_H
#define ROMEO_LR1CONTEXT_H

#include <stack>
#include <tuple>
#include <memory>
#include <context/BaseContext.h>
#include <context/Meta.h>

namespace Romeo {
    class Lex;
}

namespace RmoLang {

    class LR1Context : public Romeo::BaseContext {
    public:
        using Lex = Romeo::Lex;
        using Lexs = std::vector<const Romeo::Lex *>;
        using REF_TYPE = std::function<void(std::vector<const Romeo::Lex *> &)>;

        LR1Context() {
            metas = std::make_shared<Romeo::BlockMeta>(0, nullptr);
        }

        ~LR1Context() override;

        void clear() override {
            BaseContext::clear();
            while (!meta_stack.empty())
                meta_stack.pop();
            statements.clear();
            metas->metas.clear();
        }

        auto &Statements() {
            return statements;
        }

        void display_statements(std::ostream &ostream = std::cout);

        void display_symbol_table(std::ostream &ostream = std::cout) {
            metas->display_symbol_table(0, ostream);
        }

        /// 值相关
        void IntVal(Lexs &lexs);;

        void RealVal(Lexs &lexs);;

        void BoolVal(Lexs &lexs);;

        void UnitVal(Lexs &lexs);

        void StringVal(Lexs &lexs);

        /// 声明相关
        void Type(Lexs &lexs);

        void PopVarDecl(Lexs &lexs);

        void VarDeclare(Lexs &lexs);

        void FuncDeclMark(Lexs &lexs);

        void FuncDeclare(Lexs &lexs);

        void FuncDefine(Lexs &lexs);

        void FuncArgsLeft(Lexs &lexs);

        void FuncArgsRight(Lexs &lexs);

        void ArrayDeclare(Lexs &lexs);

        void ArrayExpend(Lexs &lexs);

        void ArrayExpendEnd(Lexs &lexs);

        /// 调用相关
        void FuncCall(Lexs &lexs);

        void Import(Lexs &lexs);

        /// 逻辑运算
        void CtrlOver(Lexs &lexs);

        void IfMark(Lexs &lexs);

        void IfStatement(Lexs &lexs);

        void ElseMark(Lexs &lexs);

        void ElseStatement(Lexs &lexs);

        void WhileMark(Lexs &lexs);

        void WhileStatement(Lexs &lexs);

        void BlockStart(Lexs &lexs);

        void BlockEnd(Lexs &lexs);

        /// 元素操作相关
        void Assign(Lexs &lexs);;

        void BinOp(Lexs &lexs);;

        void UnaryOp(Lexs &lexs);;

        void Def(Lexs &lexs);;

        void Call(Lexs &lexs);;

        /// 工具函数
        void Skip(int pops, Lexs &lexs);

        const LR1Context::Lex *Pop(Lexs &lexs);

    private:
        std::shared_ptr<Romeo::BlockMeta> metas;

        std::stack<std::shared_ptr<Romeo::Meta>> meta_stack;
        std::deque<std::tuple<int, std::string, std::string, std::string, std::shared_ptr<Romeo::Meta>>> statements;
    };
}


#endif //ROMEO_LR1CONTEXT_H
