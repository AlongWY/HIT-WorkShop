//
// Created by along on 17-10-31.
//

#include "Caclu.h"
#include <sstream>
#include <utility>
#include <cmath>

Caclu::Caclu(std::string back) : back(std::move(back)), pos(0) {}

const Mstack<float> &Caclu::getMstack() const {
  return mstack;
}

unsigned long Caclu::getPos() const {
  return pos;
}

const std::string &Caclu::getBack() const {
  return back;
}

void Caclu::reset(std::string back) {
  this->back = std::move(back);
  mstack.reset();
  pos = 0;
}

void Caclu::reset() {
  reset("");
}

float Caclu::stepOne() {
  if (pos == back.size())
    return stackTop();
  std::string buf;
  float temp;
  do {
    buf += back[pos++];
  } while (back[pos] != ' ' && pos != back.size());
  if (back[pos] == ' ')
    ++pos;
  std::stringstream sin(buf);
  // buf里面是一个数字
  if (sin >> temp) {
    mstack.push(temp);
  } else {
    float second = mstack.pop();
    float first = mstack.pop();
    switch (buf[0]) {
      case '-':second = -second;
      case '+':first += second;
        break;
      case '*':first *= second;
        break;
      case '/':first /= second;
        break;
      case '^':first = powf(first, second);
        break;
      default:break;
    }
    mstack.push(first);
  }
  return stackTop();
}

float Caclu::step(unsigned long length) {
  for (unsigned long i = 0; i != length && !IsOver(); ++i) {
    stepOne();
  }
  return stackTop();
}

float Caclu::run() {
  while (pos != back.size()) {
    stepOne();
  }
  return stackTop();
}

float Caclu::stackTop() {
  return mstack.top();
}

bool Caclu::IsOver() {
  return (pos == back.size() && mstack.getPos() == 1) || back.size() == 0;
}
