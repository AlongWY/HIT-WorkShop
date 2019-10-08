//
// Created by along on 17-12-7.
//

#ifndef PROJECT_AVLTREE_H
#define PROJECT_AVLTREE_H

#include <utility>
#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <iostream>

#define EASY_VISIT_LAMBDA [](const AvlNode *node) {std::cout << node->element << " " <<std::flush; }

template<typename Comparable>
class AvlTree {
    struct AvlNode;
    static const int ALLOWED_IMBLANCE = 1;
public:
    AvlTree() : root(nullptr) {}
    ~AvlTree() { makeEmpty(root); }
    bool contains(const Comparable &x) {
        return contains(x, root);
    }

    const Comparable &find(const Comparable &x) {
        return find(x, root);
    }

    void insert(const Comparable &x) {
        insert(x, root);
    }

    void remove(const Comparable &x) {
        remove(x, root);
    }

    /** 层序便利各节点 */
    void visitTreeCover(const std::function<void(AvlNode *)> &visit = EASY_VISIT_LAMBDA) const {
        std::queue<AvlNode *> nodeQueue;
        AvlNode *node = root;
        while (node != nullptr) {
            if (node->left != nullptr)
                nodeQueue.push(node->left);
            if (node->right != nullptr)
                nodeQueue.push(node->right);
            visit(node);
            if (!nodeQueue.empty()) {
                node = nodeQueue.front();
                nodeQueue.pop();
            } else
                node = nullptr;
        }
    }

    /** 以dot格式打印图的内容 */
    void printGraph(std::ostream &out = std::cout) const {
        out << "digraph tree{" << std::endl;
        visitTreeCover([&](AvlNode *bitNode) {
            if (bitNode->left != nullptr)
                out << "  \"" << bitNode->element << "\" -> \"" << bitNode->left->element << R"("[label = "left"];)"
                    << std::endl;
            if (bitNode->right != nullptr)
                out << "  \"" << bitNode->element << "\" -> \"" << bitNode->right->element << R"("[label = "right"];)"
                    << std::endl;
        });
        out << "}" << std::endl;
    }

    /** 中序便利节点 */
    void visitTree(const std::function<void(AvlNode *)> &visit = EASY_VISIT_LAMBDA) const { visitTree(root, visit); }
private:
    struct AvlNode {
        Comparable element;
        AvlNode *left;
        AvlNode *right;
        int height;
        AvlNode(const Comparable &ele, AvlNode *lt, AvlNode *rt, int h = 0)
            : element(ele), left(lt), right(rt), height(h) {}
        AvlNode(Comparable &&ele, AvlNode *lt, AvlNode *rt, int h = 0)
            : element(std::move(ele)), left(lt), right(rt), height(h) {}
    } *root;

    void makeEmpty(AvlNode *&t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }
        t = nullptr;
    }

    /** 返回节点的高度，如果为空则返回-1 */
    int height(AvlNode *node) {
        return node == nullptr ? -1 : node->height;
    }

    /**
     * 向一棵子树插入的内部方法
     * @param x 要插入的项
     * @param t 子树的根节点
     */
    void insert(const Comparable &x, AvlNode *&t) {
        if (t == nullptr)
            t = new AvlNode(x, nullptr, nullptr);
        else if (x < t->element)
            insert(x, t->left);
        else if (x > t->element)
            insert(x, t->right);

        balance(t);
    }

    /** 右右 */
    void rotateWithLeftChild(AvlNode *&k2) {
        AvlNode *k1 = k2->left;
        k2->left = k1->right;
        k1->right = k2;
        k2->height = std::max(height(k2->left), height(k2->right)) + 1;
        k1->height = std::max(height(k1->left), k2->height) + 1;
        k2 = k1;
    }

    /** 左左 */
    void rotateWithRightChild(AvlNode *&k2) {
        AvlNode *k1 = k2->right;
        k2->right = k1->left;
        k1->left = k2;
        k2->height = std::max(height(k2->left), height(k2->right)) + 1;
        k1->height = std::max(height(k1->right), k2->height) + 1;
        k2 = k1;
    }

    /** 左右 */
    void doubleWithLeftChild(AvlNode *&k3) {
        rotateWithRightChild(k3->left);
        rotateWithLeftChild(k3);
    }

    /** 右左 */
    void doubleWithRightChild(AvlNode *&k3) {
        rotateWithLeftChild(k3->right);
        rotateWithRightChild(k3);
    }

    /** 使树重新平衡 */
    void balance(AvlNode *&t) {
        if (t == nullptr)
            return;
        if (height(t->left) - height(t->right) > ALLOWED_IMBLANCE) {
            if (height(t->left->left) >= height(t->left->right))
                rotateWithLeftChild(t);
            else
                doubleWithLeftChild(t);
        } else if (height(t->right) - height(t->left) > ALLOWED_IMBLANCE) {
            if (height(t->right->right) >= height(t->right->left))
                rotateWithRightChild(t);
            else
                doubleWithRightChild(t);
        }
        t->height = std::max(height(t->left), height(t->right)) + 1;
    }

    AvlNode *findMin(AvlNode *t) const {
        if (t == nullptr)
            return nullptr;
        if (t->left == nullptr)
            return t;
        return findMin(t->left);
    }

    void remove(const Comparable &x, AvlNode *&t) {
        if (t == nullptr)
            return;

        if (x < t->element)
            remove(x, t->left);
        else if (t->element < x)
            remove(x, t->right);
        else if (t->left != nullptr && t->right != nullptr) {
            t->element = findMin(t->right)->element;
            remove(t->element, t->right);
        } else {
            AvlNode *oldNode = t;
            t = (t->left != nullptr) ? t->left : t->right;
            delete oldNode;
        }

        balance(t);
    }

    bool contains(const Comparable &x, AvlNode *t) const {
        if (t == nullptr)
            return false;
        if (x < t->element)
            return contains(x, t->left);
        else if (t->element < x)
            return contains(x, t->right);
        else
            return true;
    }

    const Comparable &find(const Comparable &x, AvlNode *t) {
        if (t == nullptr) {
            exit(EXIT_FAILURE);
        }

        if (x < t->element)
            return find(x, t->left);
        else if (t->element < x)
            return find(x, t->right);
        else
            return t->element;
    }

    /** 中序遍历 */
    void visitTree(AvlNode *t, const std::function<void(AvlNode *)> &visit) const {
        if (t == nullptr)
            return;
        std::stack<AvlTree<Comparable>::AvlNode *> nodeStack;
        AvlTree<Comparable>::AvlNode *curr = t;
        while (curr != nullptr || !nodeStack.empty()) {
            while (curr != nullptr) {
                //中序遍历，将左子树入栈
                nodeStack.push(curr);
                if (curr->left != nullptr)
                    curr = curr->left;
                else
                    break;
            }
            if (!nodeStack.empty()) {
                //当前栈顶是最左子树
                curr = nodeStack.top();
                //访问左子树，而后出栈，访问栈顶的右子树
                visit(curr);
                nodeStack.pop();
                if (curr->right != nullptr)
                    curr = curr->right;
                else
                    curr = nullptr;
            }
        }
    }
};

#endif //PROJECT_AVLTREE_H
