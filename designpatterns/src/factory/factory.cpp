#include <iostream>
#include <memory>

using namespace std;

class Operation {
public:
    void set_lhs(double lhs) { lhs_ = lhs; }
    void set_rhs(double rhs) { rhs_ = rhs; }

    virtual double result() = 0;

protected:
    double lhs_;
    double rhs_;
};

class AddOperation : public Operation {
public:
    double result() override {
        return lhs_ + rhs_;
    }
};

class SubOperation : public Operation {
public:
    double result() override {
        return lhs_ - rhs_;
    }
};

class MulOperation : public Operation {
public:
    double result() override {
        return lhs_ * rhs_;
    }
};

class DivOperation : public Operation {
public:
    double result() override {
        return lhs_ / rhs_;
    }
};

class IFactory {
public:
    IFactory() = default;
    virtual ~IFactory() = default;
    virtual unique_ptr<Operation> CreateOperation() = 0;
};

class AddFactory : public IFactory {
public:
    unique_ptr<Operation> CreateOperation() override {
        return make_unique<AddOperation>();
    }
};

class SubFactory : public IFactory {
public:
    unique_ptr<Operation> CreateOperation() override {
        return make_unique<SubOperation>();
    }
};

class MulFactory : public IFactory {
public:
    unique_ptr<Operation> CreateOperation() override {
        return make_unique<MulOperation>();
    }
};

class DivFactory : public IFactory {
public:
    unique_ptr<Operation> CreateOperation() override {
        return make_unique<DivOperation>();
    }
};

int main(int argc, const char *argv[])
{
    unique_ptr<IFactory> factory = make_unique<AddFactory>();
    unique_ptr<Operation> operation = factory->CreateOperation();
    operation->set_lhs(3);
    operation->set_rhs(3);
    cout << operation->result();

    return 0;
}