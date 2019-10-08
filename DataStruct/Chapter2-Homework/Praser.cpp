//
// Created by along on 17-10-31.
//

#include "Praser.h"

#include <utility>

// 算数运算符 + - * / ( ) % ! ^

Praser::Praser(std::string middle) : middle(std::move(middle)) {
    this->flag = 1;
    this->pos = 0;
}

bool Praser::compare(const char sop, const char nop) {
    flag = 1;
    if (nop == '+' || nop == '-') {
        return sop == '+' || sop == '-' || sop == '(';
    }
    if (nop == '*' || nop == '/' || nop == '%') {
        return sop != '^';
    }
    return nop == '^' || nop == '(';
}

const Mstack<char> &Praser::getMstack() const {
    return mstack;
}

const std::string &Praser::getBack() const {
    return back;
}

std::string Praser::stepOne() {
    if (pos == middle.size()) {
        while (!mstack.empty()) {
            back += ' ';
            back += mstack.pop();
        }
        return back;
    }
    char curr = middle[pos++];
    if (isalnum(curr) || curr == '.' || (flag == 1 && curr == '-')) {
        flag = 0;
        back += curr;
        return back;
    } else if (curr == ' ')
        return back;
    back += ' ';
    if (mstack.empty()) {
        mstack.push(curr);
    } else if (curr == '(') {
        mstack.push(curr);
        back.pop_back();
    } else if (curr == ')') {
        while (mstack.top() != '(') {
            back += mstack.pop();
        }
        mstack.pop();
    } else if (compare(mstack.top(), curr)) {
        mstack.push(curr);
    } else {
        do {
            back += mstack.pop();
            back += ' ';
        } while (!mstack.empty() && !compare(mstack.top(), curr));
        mstack.push(curr);
    }
    return back;
}

std::string Praser::step(unsigned long i) {
    for (unsigned long step = 0; step != i && !IsOver(); ++step) {
        stepOne();
    }
    return back;
}

std::string Praser::run() {
    step(middle.length() + 1);
    return back;
}

void Praser::reset() {
    reset("");
}

void Praser::reset(std::string middle) {
    this->middle = std::move(middle);
    pos = 0;
    flag = 1;
    mstack.reset();
}

const std::string &Praser::getMiddle() const {
    return middle;
}

unsigned long Praser::getPos() const {
    return pos;
}

bool Praser::IsOver() {
    return pos == middle.size() && mstack.empty();
}





