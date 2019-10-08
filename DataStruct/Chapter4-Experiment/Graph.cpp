//
// Created by along on 17-11-26.
//

#include "Graph.h"
#include <stack>
#include <queue>

using namespace std;

void Graph::clone(const Graph &graph) {
    reset(graph.vexCount());

    function<bool(unsigned long, unsigned long)> func = [&](unsigned long source, unsigned long sink) {
        this->addEdge(source, sink);
        return true;
    };

    unsigned long verNum = graph.vexCount();
    for (unsigned long ver = 0; ver != verNum; ++ver) {
        graph.foreach(ver, func);
    }
}

unsigned long Graph::edgeCount() const {
    unsigned long count = 0;
    for (unsigned long vex = 0; vex != vexCount(); ++vex)
        count += outDegree(vex);
    return count;
}

Graph &Graph::operator=(const Graph &rhs) {
    this->clone(rhs);
    return *this;
}
Graph::Graph(const Graph &rhs) {
    this->clone(rhs);
}
void Graph::addEdge(unsigned long, unsigned long) {
    ++edgeNum;
}
void Graph::delEdge(unsigned long, unsigned long) {
    --edgeNum;
}
unsigned long Graph::vexCount() const {
    return vexNum;
}
void Graph::reset(unsigned long vexNum) {
    clear();
    this->vexNum = vexNum;
    edgeNum = 0;
}
void Graph::clear() {
    vexNum = 0;
    edgeNum = 0;
}
unsigned long Graph::outDegree(unsigned long source) const {
    unsigned long outDegree = 0;
    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        ++outDegree;
        return true;
    };
    foreach(source, func);
    return outDegree;
}

unsigned long Graph::inDegree(unsigned long source) const {
    unsigned long inDegree = 0;
    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        if (dst == source)
            ++inDegree;
        return true;
    };
    for (unsigned long i = 0; i != vexNum; ++i)
        foreach(i, func);
    return inDegree;
}

bool Graph::hasEdge(unsigned long source, unsigned long sink) const {
    bool hasEdge = false;
    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        if (dst == sink) {
            hasEdge = true;
            return false;
        }
        return true;
    };
    foreach(source, func);
    return hasEdge;
}

void Graph::DFSR(std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        if (!visited[src]) {
            visit(src);
            visited[src] = true;
        }
        if (!visited[dst]) {
            visit(dst);
            visited[dst] = true;
            foreach(dst, func);
        }
        return true;
    };

    for (unsigned long vex = 0; vex != vexNum; ++vex) {
        if (visited[vex])
            continue;
        if (outDegree(vex) == 0) {
            visit(vex);
            visited[vex] = true;
        } else
            foreach(vex, func);
    }
}

void Graph::DFS(std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    stack<unsigned long> vexStack;

    unsigned long finded = 0;
    // 查找未访问过的节点
    function<bool(unsigned long, unsigned long)> find = [&](unsigned long src, unsigned long dst) {
        if (!visited[dst]) {
            finded = dst;
            return false;
        }

        return true;
    };

    for (unsigned long vex = 0; vex != vexNum; ++vex) {
        if (visited[vex])
            continue;
        visit(vex);
        visited[vex] = true;
        if (outDegree(vex) != 0) {
            vexStack.push(vex);
            while (!vexStack.empty()) {
                auto top = finded = vexStack.top();
                foreach(top, find);

                if (finded != top) {
                    visit(finded);
                    visited[finded] = true;
                    vexStack.push(finded);
                } else {
                    vexStack.pop();
                }
            }
        }
    }
}

void Graph::BFS(std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    queue<unsigned long> vexQueue;

    function<bool(unsigned long, unsigned long)> find = [&](unsigned long src, unsigned long dst) {
        if (!visited[dst]) {
            visit(dst);
            visited[dst] = true;
            vexQueue.push(dst);
        }
        return true;
    };

    for (unsigned long vex = 0; vex != vexNum; ++vex) {
        if (visited[vex])
            continue;
        visit(vex);
        visited[vex] = true;
        if (outDegree(vex) != 0) {
            vexQueue.push(vex);
            while (!vexQueue.empty()) {
                auto front = vexQueue.front();
                vexQueue.pop();
                foreach(front, find);
            }
        }
    }
}

void Graph::DFS(Graph &DFSTree, unsigned long vex, std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    stack<unsigned long> vexStack;

    unsigned long finded = 0;
    // 查找未访问过的节点
    function<bool(unsigned long, unsigned long)> find = [&](unsigned long src, unsigned long dst) {
        if (!visited[dst]) {
            finded = dst;
            return false;
        }
        return true;
    };

    visit(vex);
    visited[vex] = true;
    if (outDegree(vex) != 0) {
        vexStack.push(vex);
        while (!vexStack.empty()) {
            auto top = finded = vexStack.top();
            foreach(top, find);

            if (finded != top) {
                visit(finded);
                visited[finded] = true;
                DFSTree.addEdge(top, finded);
                vexStack.push(finded);
            } else {
                vexStack.pop();
            }
        }
    }
}

void Graph::DFSR(Graph &DFSTree, unsigned long vex, std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        if (!visited[src]) {
            visit(src);
            visited[src] = true;
        }
        if (!visited[dst]) {
            visit(dst);
            visited[dst] = true;
            DFSTree.addEdge(src, dst);
            foreach(dst, func);
        }
        return true;
    };

    if (outDegree(vex) == 0) {
        visit(vex);
        visited[vex] = true;
    } else
        foreach(vex, func);

}

void Graph::BFS(Graph &BFSTree, unsigned long vex, std::function<void(unsigned long)> &visit) const {
    vector<bool> visited(vexNum, false);
    queue<unsigned long> vexQueue;

    function<bool(unsigned long, unsigned long)> find = [&](unsigned long src, unsigned long dst) {
        if (!visited[dst]) {
            visit(dst);
            visited[dst] = true;
            BFSTree.addEdge(src, dst);
            vexQueue.push(dst);
        }
        return true;
    };

    visit(vex);
    visited[vex] = true;
    if (outDegree(vex) != 0) {
        vexQueue.push(vex);
        while (!vexQueue.empty()) {
            auto front = vexQueue.front();
            vexQueue.pop();
            foreach(front, find);
        }
    }
}

void Graph::reset() {
    reset(vexNum);
}

void Graph::printDot(std::ostream &out) {
    out << "digraph mGraph{" << endl;
    for (unsigned long i = 0; i != vexNum; ++i)
        out << "Node" << i << "[label = \"" << i << "\"];" << endl;

    function<bool(unsigned long, unsigned long)> func = [&](unsigned long src, unsigned long dst) {
        out << "Node" << src << " -> " << "Node" << dst << ";" << endl;
        return true;
    };
    for (unsigned long i = 0; i != vexNum; ++i)
        foreach(i, func);

    out << "}" << endl;
}

void Graph::resetFromStream(istream &theStream) {
    unsigned long vexNum;
    theStream >> vexNum;
    reset(vexNum);

    unsigned long src, dst;
    while (theStream >> src >> dst)
        addEdge(src, dst);
}

GraphT::GraphT(unsigned long n) : Graph(n) {
    for (int i = 0; i != n; ++i) {
        vexes.push_back({0, {}});
    }
}

void GraphT::addEdge(unsigned long source, unsigned long sink) {
    if (!hasEdge(source, sink)) {
        Graph::addEdge(source, sink);
        auto &src = vexes[source];
        auto &dst = vexes[sink];
        src.adjVex.push_front(sink);

        ++src.out;
        ++dst.in;
    }
}

unsigned long GraphT::vexCount() const {
    return vexes.size();
}

unsigned long GraphT::outDegree(unsigned long source) const {
    return vexes[source].out;
}

bool GraphT::hasEdge(unsigned long source, unsigned long sink) const {
    for (auto &adjVex:vexes[source].adjVex) {
        if (adjVex == sink)
            return true;
    }
    return false;
}
void GraphT::foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const {
    for (auto &sink:vexes[source].adjVex) {
        if (!func(source, sink))
            break;
    }
}

void GraphT::clear() {
    Graph::clear();
    vexes.clear();
}

unsigned long GraphT::inDegree(unsigned long source) const {
    return vexes[source].in;
}

void GraphT::reset(unsigned long vexNum) {
    Graph::reset(vexNum);
    for (int i = 0; i != vexNum; ++i) {
        vexes.push_back({0, {}});
    }
}
void GraphT::delEdge(unsigned long source, unsigned long sink) {
    if (hasEdge(source, sink)) {
        Graph::delEdge(source, sink);
        auto &src = vexes[source];
        auto &dst = vexes[sink];

        src.adjVex.remove(sink);

        --src.out;
        --dst.in;
    }
}
GraphT::GraphT(const Graph &rhs) : Graph(0) {
    *(dynamic_cast<Graph *>(this)) = rhs;
}
unsigned long GraphT::edgeCount() const {
    return Graph::edgeCount();
}
void GraphT::reset() {
    Graph::reset();
}

GraphM::GraphM(unsigned long n) : Graph(n) {
    for (unsigned long i = 0; i != n; ++i) {
        vexes.emplace_back(n, false);
    }
}

void GraphM::clear() {
    Graph::clear();
    vexes.clear();
}

void GraphM::addEdge(unsigned long source, unsigned long sink) {
    Graph::addEdge(source, sink);
    vexes[source][sink] = true;
}

unsigned long GraphM::vexCount() const {
    return vexes.size();
}

unsigned long GraphM::outDegree(unsigned long source) const {
#ifdef USE_BOOST_LIB
    return vexes[source].count();
#else
    unsigned long count = 0;
    for (auto out:vexes[source])
        count += out;
    return count;
#endif
}

bool GraphM::hasEdge(unsigned long source, unsigned long sink) const {
    return vexes[source][sink];
}

void GraphM::foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const {
    for (unsigned long adjVec = 0; adjVec != vexCount(); ++adjVec)
        if (hasEdge(source, adjVec))
            if (!func(source, adjVec))
                break;

}

unsigned long GraphM::inDegree(unsigned long source) const {
    unsigned long count = 0;
    for (unsigned long i = 0; i != vexCount(); ++i)
        count += vexes[i][source];
    return count;
}

void GraphM::reset(unsigned long vexNum) {
    Graph::reset(vexNum);
    for (unsigned long i = 0; i != vexNum; ++i) {
        vexes.emplace_back(vexNum, false);
    }
}

void GraphM::delEdge(unsigned long source, unsigned long sink) {
    Graph::delEdge(source, sink);
    vexes[source][sink] = false;
}

GraphM::GraphM(const Graph &rhs) : Graph(0) {
    *(dynamic_cast<Graph *>(this)) = rhs;
}

unsigned long GraphM::edgeCount() const {
    return Graph::edgeCount();
}
void GraphM::reset() {
    Graph::reset();
}

GraphL::GraphL(unsigned long n) : Graph(n) {
    for (unsigned long i = 0; i != n; ++i)
        vexes.emplace_back(VexNode());
}

void GraphL::addEdge(unsigned long source, unsigned long sink) {
    if (!hasEdge(source, sink)) {
        Graph::addEdge(source, sink);
        auto &src = vexes[source];
        auto &dst = vexes[sink];

        ++src.out;
        ++dst.in;

        auto *newArc = new ArcBox(source, sink, nullptr, nullptr);
        if (src.firstOut == nullptr)
            src.firstOut = newArc;
        else {
            ArcBox **findArc = &src.firstOut;
            while ((*findArc) != nullptr && (*findArc)->tailVex < sink)
                findArc = &(*findArc)->hLink;
            newArc->hLink = *findArc;
            *findArc = newArc;
        }

        if (dst.firstIn == nullptr)
            dst.firstIn = newArc;
        else {
            ArcBox **findArc = &dst.firstIn;
            while ((*findArc) != nullptr && (*findArc)->headVex < source)
                findArc = &(*findArc)->tLink;
            newArc->tLink = *findArc;
            *findArc = newArc;
        }
    }
}

void GraphL::delEdge(unsigned long source, unsigned long sink) {
    if (hasEdge(source, sink)) {
        Graph::delEdge(source, sink);
        auto &src = vexes[source];
        auto &dst = vexes[sink];

        --src.out;
        --dst.in;

        ArcBox **head = &src.firstOut, **tail = &dst.firstIn;
        while ((*tail)->headVex != source)
            tail = &(*tail)->tLink;
        *tail = (*tail)->tLink;

        while ((*head)->tailVex != sink)
            head = &(*head)->hLink;
        // 废物利用，暂时保存一下要释放的内存指针
        *tail = *head;
        *head = (*head)->hLink;
        delete *tail;
    }
}

unsigned long GraphL::vexCount() const {
    return vexes.size();
}

unsigned long GraphL::outDegree(unsigned long source) const {
    return vexes[source].out;
}

unsigned long GraphL::inDegree(unsigned long source) const {
    return vexes[source].in;
}

bool GraphL::hasEdge(unsigned long source, unsigned long sink) const {
    for (ArcBox *arc = vexes[source].firstOut; arc != nullptr; arc = arc->hLink)
        if (arc->tailVex == sink)
            return true;
    return false;
}

void GraphL::foreach(unsigned long source, std::function<bool(unsigned long, unsigned long)> &func) const {
    for (const ArcBox *arc = vexes[source].firstOut; arc != nullptr; arc = arc->hLink)
        if (!func(source, arc->tailVex))
            break;
}

void GraphL::clear() {
    Graph::clear();
    for (auto &vex:vexes) {
        while (vex.firstOut != nullptr) {
            auto curr = vex.firstOut;
            vex.firstOut = curr->tLink;
            delete curr;
        }
    }
    vexes.clear();
}

void GraphL::reset(unsigned long vexNum) {
    Graph::reset(vexNum);
    for (unsigned long i = 0; i != vexNum; ++i)
        vexes.emplace_back(VexNode());
}

GraphL::GraphL(const Graph &rhs) : Graph(0) {
    *(dynamic_cast<Graph *>(this)) = rhs;
}

unsigned long GraphL::edgeCount() const {
    return Graph::edgeCount();
}

void GraphL::foreachIn(unsigned long dst, std::function<bool(unsigned long, unsigned long)> &func) const {
    for (const ArcBox *arc = vexes[dst].firstIn; arc != nullptr; arc = arc->tLink)
        if (!func(arc->headVex, dst))
            break;
}
void GraphL::reset() {
    Graph::reset();
}


