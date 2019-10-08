//
// Created by along on 17-10-31.
//

#ifndef PROJECT_CACLU_H
#define PROJECT_CACLU_H

#include <string>
#include "Mstack.h"

class Caclu {
public:
    /**
     * 使用后缀表达式初始化
     * @param back
     */
    explicit Caclu(std::string back);

    /**
     * 获得栈数据
     * @return
     */
    const Mstack<float> &getMstack() const;

    /**
     * 步进一步
     * @return
     */
    float stepOne();

    /**
     * 步进i步
     * @param i
     * @return
     */
    float step(unsigned long i);

    /**
     * 计算出结果
     * @return
     */
    float run();

    /**
     * 重置
     */
    void reset();

    /**
     * 取得栈顶数据
     * @return
     */
    float stackTop();

    /**
     * 适用新的后缀表达式重置
     * @param back
     */
    void reset(std::string back);

    /**
     * 获取当前步进位置
     * @return
     */
    unsigned long getPos() const;

    /**
     * 获取后缀表达式
     * @return
     */
    const std::string &getBack() const;

    /**
     * 计算是否结束
     * @return
     */
    bool IsOver();

private:
    unsigned long pos;
    Mstack<float> mstack;
    std::string back;
};


#endif //PROJECT_CACLU_H
