//
// Created by along on 17-12-7.
//

#include <iostream>
#include <fstream>
#include "AvlTree.h"

void printMenu();

int main() {
    using namespace std;
    AvlTree<int> tree;

    fstream file;

    char ch;
    int key;
    string filename;
    do {
        printMenu();
        scanf(" %c", &ch);
        switch (ch) {
        case '1':cout << "文件名：" << flush;
            cin >> filename;
            file.open(filename, ios::in);
            while (file >> key) {
                tree.insert(key);
            }
            file.close();
            break;
        case '2':cout << "输入数据：" << flush;
            cin >> key;
            tree.insert(key);
            break;
        case '3':cout << "删除数据：" << flush;
            cin >> key;
            tree.remove(key);
            break;
        case '4':tree.visitTree();
            cout << endl;
            break;
        case '5':cout << "文件名：" << flush;
            cin >> filename;
            file.open(filename, ios::out);
            tree.printGraph(file);
            file.close();
            break;
        case '6':cout << "输入数据：" << flush;
            cin >> key;
            cout << tree.contains(key) << endl;
            break;
        default:break;
        }

    } while (ch != 'q');
    return 0;
}

void printMenu() {
    using namespace std;
    cout << "1. 从文件读取数据" << endl;
    cout << "2. 添加数据" << endl;
    cout << "3. 删除数据" << endl;
    cout << "4. 打印数据" << endl;
    cout << "5. 输出dot图" << endl;
    cout << "6. 查找" << endl;
}