//
// Created by along on 17-11-11.
//

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "BitTree.hpp"

using namespace std;

int main(int argv, char **argc) {
    ifstream fileIn;
    ofstream dotFile("tree.dot");
    vector<int> front;
    vector<int> middle;
    BitTree<int> tree;

    if (argv > 1) {
        for (int i = 0; i != argv; ++i) {
            if (0 == strncmp(argc[++i], "-I", 2)) {
                int data;
                string line;
                istringstream lineStream;
                fileIn.open(argc[++i]);
                getline(fileIn, line);
                lineStream.str(line);
                lineStream.clear();
                while (lineStream >> data) {
                    front.push_back(data);
                }
                getline(fileIn, line);
                lineStream.str(line);
                lineStream.clear();
                while (lineStream >> data) {
                    middle.push_back(data);
                }
                tree = BitTree<int>::CreateBitTree(front, middle);
            }
        }
    } else {
        tree =
            BitTree<int>::CreateBitTree({1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12}, true);
    }

    cout << "先序非递归:";
    tree.visitTree(tree.DLR);
    cout << endl;
    cout << "中序非递归:";
    tree.visitTree(tree.LDR);
    cout << endl;
    cout << "后序非递归:";
    tree.visitTree(tree.LRD);
    cout << endl;

    cout << "先序递归:";
    tree.visitTreeR(tree.DLR);
    cout << endl;
    cout << "中序递归:";
    tree.visitTreeR(tree.LDR);
    cout << endl;
    cout << "后序递归:";
    tree.visitTreeR(tree.LRD);
    cout << endl;

    BitTree<int>::BitTreeThreading(tree);
    tree.printGraph(dotFile);

    cout << "先序线索:";
    tree.threadVisit(tree.DLR);
    cout << endl;
    cout << "中序线索:";
    tree.threadVisit(tree.LDR);
    cout << endl;
    cout << "后序线索:";
    tree.threadVisit(tree.LRD);
    cout << endl;

    cout << "层序非递归:";
    tree.visitTreeCover();
    cout << endl;

    fileIn.close();
    dotFile.close();
    return 0;
}

