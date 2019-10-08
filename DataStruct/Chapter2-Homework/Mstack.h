//
// Created by along on 17-10-31.
//

#ifndef PROJECT_MSTACK_H
#define PROJECT_MSTACK_H


template<class T>
class Mstack {
public:
    explicit Mstack(unsigned long size) : size(size), pos(0) {
        data = new T[size];
    }

    virtual ~Mstack() {
        delete data;
    }

    Mstack() : Mstack(20) {}

    const T &top() const {
        return data[pos - 1];
    }

    T pop() {
        if (pos != 0)
            return data[--pos];
    }

    void push(T in) {
        if (pos == size) {
            auto *newdata = new T[2 * size];
            for (int i = 0; i != pos; ++pos) {
                newdata[i] = data[i];
            }
            delete data;
            data = newdata;
        }
        data[pos++] = in;
    }

    bool empty() const {
        return pos == 0;
    }

    const T *getData() const {
        return data;
    }

    const unsigned long &getPos() const {
        return pos;
    }

    void reset() {
        pos = 0;
    }

private:
    unsigned long size;
    unsigned long pos;
    T *data;
};


#endif //PROJECT_MSTACK_H
