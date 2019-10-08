/**
 * 二叉树的模板实现
 */

#ifndef PROJECT_BITTREE_H
#define PROJECT_BITTREE_H

#include <utility>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <functional>

template<typename Comparable>
class BitTree {
    struct BitNode;
    using BitNodePtr = BitNode *;

#define EASY_VISIT_LAMBDA [](const BitNode *node) {std::cout << node->data << " " <<std::flush; }

public:
    BitTree() : root(nullptr), header(nullptr) {};
    BitTree(const BitTree &rhs) : header(nullptr) { root = clone(rhs.root); }
    BitTree(BitTree &&rhs) noexcept: root(rhs.root), header(nullptr) { rhs.root = nullptr; }
    ~BitTree() {
        makeEmpty();
    };

    /** 判断树是否为空*/
    bool isEmpty() const {
        return root == nullptr;
    }

    /** 将树清空 */
    void makeEmpty() {
        makeEmpty(root);
        if (header != nullptr)
            delete header;
    }

    /** 以dot格式打印图的内容 */
    void printGraph(std::ostream &out = std::cout) const {
        std::queue<BitNode *> nodeQueue;
        BitNode *node = root;
        out << "digraph tree{" << std::endl;
        if (header != nullptr) {
            if (header->left != nullptr)
                out << R"(  "header" -> ")" << header->left->data
                    << R"("[label = "left"];)" << std::endl;
            if (header->right != nullptr)
                out << R"(  "header" -> ")" << header->right->data
                    << R"("[label = "right"];)" << std::endl;
        }
        visitTreeCover([&](BitNode *bitNode) {
            if (bitNode->left != nullptr && bitNode->left != header)
                out << "  \"" << bitNode->data << "\" -> \"" << bitNode->left->data << R"("[label = "left"];)"
                    << std::endl;
            else if (bitNode->left != nullptr)
                out << "  \"" << bitNode->data << R"(" -> "header"[label = "left"];)" << std::endl;

            if (bitNode->right != nullptr && bitNode->right != header)
                out << "  \"" << bitNode->data << "\" -> \"" << bitNode->right->data << R"("[label = "right"];)"
                    << std::endl;
            else if (bitNode->right != nullptr)
                out << "  \"" << bitNode->data << R"(" -> "header"[label = "right"];)" << std::endl;
        });
        out << "}" << std::endl;
    }

    /**
     * L:left
     * D:data
     * R:right
     */
    enum Mode { DLR, LDR, LRD };

    /** 迭代器 */
    class Iterator {
    public:
        Iterator(BitNode *Root, Mode theMode) : header(Root), mode(theMode), isEnd(header == nullptr) {
            switch (mode) {
            default:
            case DLR:curr = header->left;
                break;
            case LDR:
            case LRD:curr = header->left;
                while (curr->lTag && curr != nullptr)
                    curr = curr->left;
            }
            isEnd = curr == header;
        }
        const BitNode *operator*() {
            return curr;
        }
        void operator++(int) {
            switch (mode) {
            default:
            case DLR:preNext();
                isEnd = (curr == header);
                break;
            case LDR:inNext();
                isEnd = (curr == header);
                break;
            case LRD:postNext();
                isEnd = (curr == header);
                break;
            }
        }
        const bool &IsEnd() { return isEnd; }
    private:

        const BitNodePtr header;
        Mode mode;
        const BitNode *curr;
        bool isEnd;

        /**
         * 前序遍历的后继结点：
         * (1)P的左子树不为空，此时P的后继结点就是P的左儿子；
         * (2)P的左子树为空但右子树不为空，此时P的后继结点就是P的右儿子；
         * (3)P的左右子树均为空，此时在从P开始的右线索序列中，第一个有右儿子的节点的右儿子或者头结点就是P的后继结点。
         */
        void preNext() {
            if (curr->lTag)
                curr = curr->left;
            else {
                while (!curr->rTag) {
                    curr = curr->right;
                    if (curr == header)
                        return;
                }
                curr = curr->right;
            }
        }
        /**
         * 中序遍历的后继结点：
         * (1)若一个节点的右子树为空，此时右线索所指节点即为所求；
         * (2)若这个节点的右子树不为空，此时它的后继结点是其右子树的最左节点。
         */
        void inNext() {
            if (!curr->rTag)
                curr = curr->right;
            else {
                curr = curr->right;
                while (curr->lTag)
                    curr = curr->left;
            }
        }

        /**
         *  后序遍历的后继结点：
         *  (1)当一个节点是它的双亲节点的右孩子时，它的后序遍历的后继就是父节点。
         *  (2)当它是父节点的左孩子，且父节点没有右子树时，它的后序遍历的后继也是父节点。
         *  (3)当它是父节点的左孩子，且父节点有右子树时，它的后序遍历的后继是父节点右子树中的最左节点（如果左子树为空，右子树不空，则为右子树的最左节点）
         */
        void postNext() {
            const BitNode *father = findFather(curr);
            if (father->right == curr || (father->left == curr && (!father->rTag || father->right == nullptr)))
                curr = father;
            else if (father->left == curr && father->rTag) {
                curr = father->right;
                do {
                    while (curr->lTag) {
                        curr = curr->left;
                    }
                    if (curr->rTag)
                        curr = curr->right;
                } while (curr->lTag);

            }
        }

        /**
         * 查找一个节点的父节点
         * @param child
         * @return
         * @details
         */
        const BitNode *findFather(const BitNode *child) {
            const BitNode *father = child;
            while (father->rTag)
                father = father->right;
            const BitNode *lFather = father->left;
            if (lFather->right == child && lFather->rTag)
                return lFather;
            const BitNode *rFather = father->right;
            if (rFather->left == child && rFather->lTag)
                return rFather;

            father = child;
            while (father->lTag)
                father = father->left;
            lFather = father->left;
            if (lFather->right == child && lFather->rTag)
                return lFather;
            rFather = father->right;
            if (rFather->left == child && rFather->lTag)
                return rFather;
        }

    };

    /** 层序建树 */
    static BitTree CreateBitTree(std::vector<Comparable> vec, bool skip = false, Comparable END = Comparable()) {
        BitTree<Comparable> bitTree;
        std::queue<BitTree::BitNode **> nodeQueue;
        nodeQueue.push(&bitTree.root);
        for (const auto &data:vec) {
            BitTree::BitNode **node = nodeQueue.front();
            nodeQueue.pop();
            if (skip && data == END) {
                continue;
            }
            *node = new BitNode(data, nullptr, nullptr);
            nodeQueue.push(&(*node)->left);
            nodeQueue.push(&(*node)->right);

        }
        return bitTree;
    }

    /** 先序+中序建树 中序+后序建树 */
    static BitTree CreateBitTree(std::vector<Comparable> vecA,
                                 std::vector<Comparable> vecB,
                                 Mode mode = DLR) {
        BitTree<Comparable> bitTree;
        if (vecA.size() == 0 || vecA.size() != vecB.size())
            bitTree.root = nullptr;
        else if (mode == DLR)
            bitTree.root = CreateNodePreMiddle(vecA, vecB, 0, vecA.size() - 1, 0, vecB.size() - 1);
        else
            bitTree.root = CreateNodeMiddlePost(vecA, vecB, 0, vecA.size() - 1, 0, vecB.size() - 1);
        return bitTree;
    }

    /** 将二叉树线索化 */
    static void BitTreeThreading(BitTree &tree) {
        tree.header = new BitNode(Comparable(), tree.root, nullptr);
        tree.header->right = tree.header;
        tree.header->rTag = false;
        BitNodePtr *curr = &tree.root;
        BitNodePtr *pre = &tree.header;
        if (*curr == nullptr)
            return;
        std::stack<BitNodePtr *> nodeStack;
        while (*curr != nullptr || !nodeStack.empty()) {
            while (*curr != nullptr) {
                //中序遍历，将左子树入栈
                nodeStack.push(curr);
                curr = &(*curr)->left;
            }
            if (!nodeStack.empty()) {
                //当前栈顶是最左子树
                curr = nodeStack.top();
                //访问左子树，而后出栈，访问栈顶的右子树
                (*curr)->lTag = (*curr)->left != nullptr;
                (*curr)->rTag = (*curr)->right != nullptr;

                if (!(*curr)->lTag) {
                    (*curr)->left = *pre;
                }

                if ((*pre)->right == nullptr) {
                    (*pre)->rTag = false;
                    (*pre)->right = *curr;
                }

                pre = curr;
                nodeStack.pop();
                curr = &(*curr)->right;
            }
        }
        (*pre)->right = tree.header;
        tree.header->right = *pre;
    }

    /** 获取不同访问模式的迭代器 */
    Iterator getIterator(Mode mode = Mode::DLR) {
        if (header == nullptr)
            BitTreeThreading(*this);
        return Iterator(header, mode);
    }

    /** 线索遍历各节点，可更改模式 */
    void threadVisit(Mode mode = DLR, const std::function<void(const BitNode *)> &visit = EASY_VISIT_LAMBDA) {
        Iterator iter = getIterator(mode);
        while (!iter.IsEnd()) {
            visit(*iter);
            iter++;
        }
    }

    /** 选用不同模式进行遍历，递归式 */
    void visitTreeR(Mode mode = DLR, const std::function<void(BitNode *)> &visit = EASY_VISIT_LAMBDA) const {
        switch (mode) {
        default:
        case DLR:visitTreeR(root, visit);
            break;
        case LDR:visitTreeMR(root, visit);
            break;
        case LRD:visitTreeBR(root, visit);
            break;
        }
    }

    /** 选用不同模式进行遍历，非递归式 */
    void visitTree(Mode mode = DLR, const std::function<void(BitNode *)> &visit = EASY_VISIT_LAMBDA) const {
        switch (mode) {
        default:
        case DLR:visitTree(root, visit);
            break;
        case LDR:visitTreeM(root, visit);
            break;
        case LRD:visitTreeB(root, visit);
            break;
        }
    }

    /** 层序便利各节点 */
    void visitTreeCover(const std::function<void(BitNode *)> &visit = EASY_VISIT_LAMBDA) const {
        std::queue<BitNode *> nodeQueue;
        BitNode *node = root;
        while (node != nullptr) {
            if (node->left != nullptr && node->lTag)
                nodeQueue.push(node->left);
            if (node->right != nullptr && node->rTag)
                nodeQueue.push(node->right);
            visit(node);
            if (!nodeQueue.empty()) {
                node = nodeQueue.front();
                nodeQueue.pop();
            } else
                node = nullptr;
        }
    }

    /** 将x插入树中，忽略重复项 */
    void insertR(const Comparable &x) {
        insertR(x, root);
    }
    void insert(const Comparable &x) {
        insert(x, root);
    }

    /** 拷贝赋值 */
    BitTree &operator=(const BitTree &rhs) {
        makeEmpty();
        root = clone(rhs.root);
        return *this;
    }

private:
    struct BitNode {
        // 约定lTag为true时，left或right是有效值
        Comparable data;
        bool lTag;
        bool rTag;
        BitNode *left;
        BitNode *right;
        BitNode(const Comparable &theData, BitNode *lt, BitNode *rt)
            : data(theData), left(lt), right(rt), lTag(true), rTag(true) {}
        BitNode(const Comparable &&theData, BitNode *lt, BitNode *rt)
            : data(std::move(theData)), left(lt), right(rt), lTag(true), rTag(true) {}
    };

    /** 树的根节点 */
    BitNode *root;
    /** 线索树的头 */
    BitNode *header;

    void makeEmpty(BitNode *&t) {
        if (t != nullptr) {
            if (t->lTag)
                makeEmpty(t->left);
            if (t->rTag)
                makeEmpty(t->right);
            delete t;
        }
        t = nullptr;
    }
    void insertR(const Comparable &x, BitNode *&t) {
        if (t == nullptr)
            t = new BitNode(x, nullptr, nullptr);
        else if (x < t->data)
            insertR(x, t->left);
        else if (t->data < x)
            insertR(x, t->right);
        else
            return;
    }
    void insert(const Comparable &x, BitNode *&t) {
        if (t == nullptr) {
            t = new BitNode(x, nullptr, nullptr);
        } else {
            BitTree<Comparable>::BitNode *node = t;
            while (true) {
                //相等则忽略
                if (x == node->data)
                    return;
                if (x < node->data) {
                    if (node->left != nullptr)
                        node = node->left;
                    else {
                        node->left = new BitNode(x, nullptr, nullptr);
                        return;
                    }
                } else {
                    if (node->right != nullptr)
                        node = node->right;
                    else {
                        node->right = new BitNode(x, nullptr, nullptr);
                        return;
                    }
                }
            }
        }
    }

    /** 先序遍历 */
    void visitTreeR(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t != nullptr) {
            visit(t);
            if (t->lTag)
                visitTreeR(t->left, visit);
            if (t->rTag)
                visitTreeR(t->right, visit);
        }
    }
    /** 中序遍历 */
    void visitTreeMR(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t != nullptr) {
            if (t->lTag)
                visitTreeMR(t->left, visit);
            visit(t);
            if (t->rTag)
                visitTreeMR(t->right, visit);
        }
    }
    /** 后序遍历 */
    void visitTreeBR(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t != nullptr) {
            if (t->lTag)
                visitTreeBR(t->left, visit);
            if (t->rTag)
                visitTreeBR(t->right, visit);
            visit(t);
        }
    }

    /** 先序遍历 */
    void visitTree(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t == nullptr)
            return;
        std::stack<BitTree<Comparable>::BitNode *> nodeStack;
        BitTree<Comparable>::BitNode *curr = t;
        while (curr != nullptr || !nodeStack.empty()) {
            while (curr != nullptr) {
                //先序遍历，所以可以直接访问，之后入栈，访问左子树
                visit(curr);
                nodeStack.push(curr);
                if (curr->lTag)
                    curr = curr->left;
                else
                    break;
            }
            //栈不空
            if (!nodeStack.empty()) {
                //之前左子树已经访问过了，栈顶肯定是之前的最左子树
                //将其出栈访问其右子树
                curr = nodeStack.top();
                nodeStack.pop();
                if (curr->rTag)
                    curr = curr->right;
                else
                    curr = nullptr;
            }
        }

    }
    /** 中序遍历 */
    void visitTreeM(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t == nullptr)
            return;
        std::stack<BitTree<Comparable>::BitNode *> nodeStack;
        BitTree<Comparable>::BitNode *curr = t;
        while (curr != nullptr || !nodeStack.empty()) {
            while (curr != nullptr) {
                //中序遍历，将左子树入栈
                nodeStack.push(curr);
                if (curr->lTag)
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
                if (curr->rTag)
                    curr = curr->right;
                else
                    curr = nullptr;
            }
        }
    }
    /** 后序遍历 */
    void visitTreeB(BitNode *t, const std::function<void(BitNode *)> &visit) const {
        if (t == nullptr)
            return;
        std::stack<BitTree<Comparable>::BitNode *> nodeStack;
        BitTree<Comparable>::BitNode *curr = t;
        BitTree<Comparable>::BitNode *visited = nullptr;
        while (curr != nullptr || !nodeStack.empty()) {
            while (curr != nullptr) {
                //左子树全部入栈
                nodeStack.push(curr);
                if (curr->lTag)
                    curr = curr->left;
                else
                    break;
            }
            //取得最左子树
            curr = nodeStack.top();
            if (curr->right == nullptr || !curr->rTag || curr->right == visited) {
                //右子树为空或者已经访问过了
                //说明当前该访问中间了
                visit(curr);
                visited = curr;
                nodeStack.pop();
                curr = nullptr;
            } else //右子树没有访问过，访问右子树
            if (curr->rTag)
                curr = curr->right;
            else
                curr = nullptr;
        }
    }

    /** 前序+中序建树 */
    static BitNode *CreateNodePreMiddle(const std::vector<Comparable> &vec,
                                        const std::vector<Comparable> &vecM,
                                        unsigned long start,
                                        unsigned long end,
                                        unsigned long startM,
                                        unsigned long endM) {
        auto *root = new BitNode(vec[start], nullptr, nullptr);
        if (startM == endM && start == end && startM == start)
            return root;

        unsigned long rootM;
        for (rootM = startM; rootM <= endM; ++rootM)
            if (vecM[rootM] == vec[start])
                break;// 找到中序的根节点

        // |root|   left  |   right  |
        // |   left  |root|   right  |

        if (rootM > startM)
            root->left = CreateNodePreMiddle(vec, vecM, start + 1, start + rootM - startM, startM, rootM - 1);

        if (rootM < endM)
            root->right = CreateNodePreMiddle(vec, vecM, start + rootM - startM + 1, end, rootM + 1, endM);

        return root;
    }

    /** 中序+后序建树 */
    static BitNode *CreateNodeMiddlePost(const std::vector<Comparable> &vecM,
                                         const std::vector<Comparable> &vec,
                                         unsigned long startM,
                                         unsigned long endM,
                                         unsigned long start,
                                         unsigned long end) {
        auto *root = new BitNode(vec[end], nullptr, nullptr);
        if (startM == endM && start == end && start == startM)
            return root;

        unsigned long rootM;
        for (rootM = startM; rootM <= endM; ++rootM)
            if (vecM[rootM] == vec[end])
                break;// 找到中序的根节点

        // |   left  |   right  |root|
        // |   left  |root|   right  |

        if (rootM > startM)
            root->left = CreateNodeMiddlePost(vecM, vec, startM, rootM - 1, start, start + (rootM - 1 - startM));

        if (rootM < endM)
            root->right = CreateNodeMiddlePost(vecM, vec, rootM + 1, endM, start + rootM - startM, end - 1);

        return root;
    }

    BitNode *clone(BitNode *t) const {
        if (t == nullptr)
            return nullptr;
        else if (t->lTag && t->rTag)
            return new BitNode(t->data, clone(t->left), clone(t->right));
        else if (t->lTag)
            return new BitNode(t->data, clone(t->left), nullptr);
        else if (t->rTag)
            return new BitNode(t->data, nullptr, clone(t->right));
    }

};

#endif //PROJECT_BITTREE_H

