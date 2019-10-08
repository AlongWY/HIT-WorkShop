//
// Created by along on 17-11-26.
//

#ifndef PROJECT_GRAPH_H
#define PROJECT_GRAPH_H

#ifdef USE_BOOST_LIB
#include <boost/dynamic_bitset.hpp>
#endif

#include <vector>
#include <forward_list>
#include <functional>

class Graph {
public:
    /**
     * 产生一个顶点为0 -- n-1的图
     * @param n
     */
    explicit Graph(unsigned long n) : vexNum(n), edgeNum(0) {};

    /**
     * 使用已有的图构造一个新图
     * @param rhs
     */
    Graph(const Graph &rhs);

    /**
     * 析构函数，避免内存泄漏
     */
    virtual ~Graph() { clear(); };

    /**
     * 拷贝赋值函数
     * @param rhs
     * @return
     */
    Graph &operator=(const Graph &rhs);

    /**
     * 添加一条边
     * @param source
     * @param sink
     */
    virtual void addEdge(unsigned long source, unsigned long sink);

    /**
     * 删除一条边
     * @param source
     * @param sink
     */
    virtual void delEdge(unsigned long source, unsigned long sink);

    /**
     * 返回顶点个数
     * @return
     */
    virtual unsigned long vexCount() const;

    /**
     * 边的个数
     * @return
     */
    virtual unsigned long edgeCount() const;

    /**
     * 顶点的出度
     * @param source
     * @return
     */
    virtual unsigned long outDegree(unsigned long source) const;

    /**
     * 顶点的入度
     * @param source
     * @return
     */
    virtual unsigned long inDegree(unsigned long source) const;

    /**
     * 两个顶点之间是否有边
     * @param source
     * @param sink
     * @return
     */
    virtual bool hasEdge(unsigned long source, unsigned long sink) const;

    /**
     * 遍历与某个顶点相临接的所有顶点
     * 当func返回值为false的时候可以停止访问
     * @param source
     * @param func
     */
    virtual void foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const = 0;

    /**
     * 先深遍历
     * @param DFSTree
     * @param out
     */
    virtual void DFS(std::function<void(unsigned long)> &visit) const;

    /**
     * 先深遍历(递归)
     * @param DFSTree
     * @param out
     */
    virtual void DFSR(std::function<void(unsigned long)> &visit) const;

    /**
     * 先广遍历
     * @param BFSTree
     * @param out
     */
    virtual void BFS(std::function<void(unsigned long)> &visit) const;

    /**
     * 带起始点的先深遍历
     * @param DFSTree
     * @param start
     * @param visit
     */
    virtual void DFS(Graph &DFSTree, unsigned long start, std::function<void(unsigned long)> &visit) const;

    /**
     * 带起始点的先深遍历(递归)
     * @param DFSTree
     * @param start
     * @param out
     */
    virtual void DFSR(Graph &DFSTree, unsigned long start, std::function<void(unsigned long)> &visit) const;

    /**
     * 带起始点的先广遍历
     * @param BFSTree
     * @param start
     * @param out
     */
    virtual void BFS(Graph &BFSTree, unsigned long start, std::function<void(unsigned long)> &visit) const;

    /**
     * 重置
     */
    virtual void reset();

    /**
     * 重置
     * @param vexNum
     */
    virtual void reset(unsigned long vexNum);

    /**
     * 将dot图打印到流
     * @param out
     */
    void printDot(std::ostream &out);

    /**
     * 从文件构造一个图
     * @param filename
     */
    void resetFromStream(std::istream &theStream);
protected:
    /**
     * 对数据进行清空
     */
    virtual void clear();
private:

    /**
     * 克隆一个图
     * @param graph
     */
    void clone(const Graph &graph);

    unsigned long vexNum;
    unsigned long edgeNum;
};

/**
 * 图的邻接表实现
 * T:Table
 */
class GraphT : public Graph {
public:
    explicit GraphT(unsigned long n);
    explicit GraphT(const Graph &rhs);
    void addEdge(unsigned long source, unsigned long sink) override;
    void delEdge(unsigned long source, unsigned long sink) override;
    inline unsigned long vexCount() const override;
    unsigned long edgeCount() const override;
    unsigned long outDegree(unsigned long source) const override;
    unsigned long inDegree(unsigned long source) const override;
    bool hasEdge(unsigned long source, unsigned long sink) const override;
    void foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const override;
    void reset() override;
    void reset(unsigned long vexNum) override;
private:
    void clear() override;
    /** 顶点表的数据结构 */
    typedef struct VexNode {
        unsigned long in;
        unsigned long out;
        std::forward_list<unsigned long> adjVex;
    } VexNode;
    /** 邻接表顶点 */
    std::vector<VexNode> vexes;
};

/**
 * 图的邻接矩阵实现
 * M:Matrix
 */
class GraphM : public Graph {
public:
    explicit GraphM(unsigned long n);
    explicit GraphM(const Graph &rhs);
    void addEdge(unsigned long source, unsigned long sink) override;
    void delEdge(unsigned long source, unsigned long sink) override;
    inline unsigned long vexCount() const override;
    unsigned long edgeCount() const override;
    unsigned long outDegree(unsigned long source) const override;
    unsigned long inDegree(unsigned long source) const override;
    bool hasEdge(unsigned long source, unsigned long sink) const override;
    void foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const override;
    void reset() override;
    void reset(unsigned long vexNum) override;
private:
    void clear() override;
#ifdef USE_BOOST_LIB
    std::vector<boost::dynamic_bitset<>> vexes;
#else
    std::vector<std::vector<bool>> vexes;
#endif
};

/**
 * 图的十字链表实现
 * L:List
 */
class GraphL : public Graph {
public:
    explicit GraphL(unsigned long n);
    explicit GraphL(const Graph &rhs);
    ~GraphL() override { clear(); };
    void addEdge(unsigned long source, unsigned long sink) override;
    void delEdge(unsigned long source, unsigned long sink) override;
    unsigned long vexCount() const override;
    unsigned long edgeCount() const override;
    unsigned long outDegree(unsigned long source) const override;
    unsigned long inDegree(unsigned long source) const override;
    bool hasEdge(unsigned long source, unsigned long sink) const override;
    void foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const override;
    void foreachIn(unsigned long dst, std::function<bool(unsigned long, unsigned long)> &func) const;
    void reset() override;
    void reset(unsigned long vexNum) override;
private:
    typedef struct ArcBox {
        unsigned long headVex, tailVex;
        struct ArcBox *hLink, *tLink;
        ArcBox(unsigned long head, unsigned long tail, ArcBox *headLink, ArcBox *tailLink) :
            headVex(head), tailVex(tail), hLink(headLink), tLink(tailLink) {}
    } ArcBox;

    typedef struct VexNode {
        unsigned long in, out;
        ArcBox *firstIn, *firstOut;
        VexNode() : in(0), out(0), firstIn(nullptr), firstOut(nullptr) {};
    } VexNode;

    void clear() override;
    std::vector<VexNode> vexes;
};

#endif //PROJECT_GRAPH_H
