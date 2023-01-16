#include <iostream>
#include <memory>
#include <string>

using namespace std;

template <typename T>
class Operation {
public:
    Operation() {};
    virtual ~Operation() {}
    virtual T result() = 0;
    void set_lhs(const T &lhs) { lhs_ = lhs; }
    void set_rhs(const T &rhs) { rhs_ = rhs; }

protected:
    T lhs_;
    T rhs_;
};

template <typename T>
class OperationAdd : public Operation<T> {
public:
    T result() {
        return Operation<T>::lhs_ + Operation<T>::rhs_;
    }
};

template <typename T>
class OperationSub : public Operation<T> {
public:
    T result() {
        return Operation<T>::lhs_ - Operation<T>::rhs_;
    }
};

template <typename T>
class OperationMul : public Operation<T> {
    T result() {
        return Operation<T>::lhs_ * Operation<T>::rhs_;
    }
};

template <typename T>
class OperationDiv : public Operation<T> {
    T result() {
        return Operation<T>::lhs_ / Operation<T>::rhs_;
    }
};

template <typename T>
class OperationFactory {
public:
    static unique_ptr<Operation<T>> CreateOperate(const string &operate) {
        if (operate == "+") {
            return make_unique<OperationAdd<T>>();
        } else if (operate == "-") {
            return make_unique<OperationSub<T>>();
        } else if (operate == "*") {
            return make_unique<OperationMul<T>>();
        } else if (operate == "/") {
            return make_unique<OperationDiv<T>>();
        } else {
            return nullptr;
        }
    }
};

int main(int argc, const char *argv[])
{
    unique_ptr<Operation<int>> operation = OperationFactory<int>::CreateOperate("/");
    int lhs = 3, rhs = 3;
    operation->set_lhs(lhs);
    operation->set_rhs(rhs);

    cout << operation->result() << endl;
}
