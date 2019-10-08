//
// Created by along on 17-10-31.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "Praser.h"
#include "Caclu.h"
#include <iomanip>

// | + |      (1 + 2) * 3 + 4
// | ( |           ^
// |___|       1  2


int main(int argc, char *argv[]) {
  std::string middle;
  std::ofstream outfile;

  if (argc > 1)
    for (int i = 0; i != argc; ++i) {
      if (0 == strcmp(argv[i], "-f")) {
        std::ifstream file(argv[++i]);
        std::getline(file, middle);
        file.close();
      } else if (0 == strcmp(argv[i], "-o")) {
        outfile.open(argv[++i]);
      }
    }
  else
    std::cout << "Usage:" << std::endl
              << "\t -f filename : input file\"No #\"" << std::endl
              << "\t -o filename : output file" << std::endl;

  Praser praser(middle);
  while (!praser.IsOver()) {
    praser.stepOne();
    unsigned long pos = praser.getMstack().getPos();
    for (unsigned long i = pos > 5 ? pos : 5; i != 0; --i) {
      char curr = praser.getMstack().getData()[i - 1];
      if (pos < i)
        curr = ' ';
      std::cout << "|   " << curr << "   | ";
      if (i == 2)
        std::cout << praser.getMiddle();
      else if (i == 1) {
        for (unsigned long j = praser.getPos(); j != 1; --j)
          std::cout << " ";
        std::cout << "^";
      }
      std::cout << std::endl;
    }
    std::cout << "|_______| " << praser.getBack() << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
  }
  outfile << praser.run();

  Caclu caclu(praser.run());
  while (!caclu.IsOver()) {
    caclu.stepOne();
    unsigned long pos = caclu.getMstack().getPos();
    for (unsigned long i = pos > 5 ? pos : 5; i != 0; --i) {
      float curr = caclu.getMstack().getData()[i - 1];
      if (pos < i)
        std::cout << "|       | ";
      else
        std::cout << "|" << std::setw(6) << curr << " | ";
      if (i == 1)
        std::cout << caclu.getBack();

      std::cout << std::endl;
    }
    std::cout << "|_______| ";
    unsigned long currPos = caclu.getPos();
    unsigned long backSize = caclu.getBack().size();
    if (backSize != 0)
      for (unsigned long j = currPos == backSize ? currPos : currPos - 1; j != 1; --j)
        std::cout << " ";
    std::cout << "^" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
  }
  return 0;
}