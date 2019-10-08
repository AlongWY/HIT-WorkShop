//
// Created by along on 17-10-31.
//

#ifndef PROJECT_PRASER_H
#define PROJECT_PRASER_H

#include <string>
#include "Mstack.h"

class Praser {
public:
    /**
     * 使用中缀表达式初始化
     * @param middle
     */
    explicit Praser(std::string middle);

    /**
     * 获取栈
     * @return
     */
    const Mstack<char> &getMstack() const;

    /**
     * 获取后缀表达式
     * @return
     */
    const std::string &getBack() const;

    /**
     * 步进i步
     * @param i
     */
    std::string step(unsigned long i);

    /**
     * 完全解析
     */
    std::string run();

    /**
     * 重置
     */
    void reset();

    /**
     * 使用新的中缀表达式重置
     * @param middle
     */
    void reset(std::string middle);

    /**
     * 步进一步
     */
    std::string stepOne();

    /**
     * 取中缀表达式
     * @return
     */
    const std::string &getMiddle() const;

    /**
     * 取当前步进位置
     * @return
     */
    unsigned long getPos() const;

    /**
     * 是否转换结束
     * @return
     */
    bool IsOver();

private:
    bool compare(const char sop, const char nop);

    unsigned long pos;
    Mstack<char> mstack;
    // flag = 1 说明下一个读的-是负号
    int flag;
    std::string middle;
    std::string back;
};


#endif //PROJECT_PRASER_H
