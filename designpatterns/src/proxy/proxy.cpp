#include <iostream>
#include <memory>

using namespace std;

class Subject {
public:
    virtual void Request() = 0;
};

class RealSubject : public Subject {
public:
    void Request() override {
        cout << "Real request\n";
    }
};

class Proxy : public Subject {
public:
    void Request() override {
        if (!real_subject_) {
            real_subject_ = make_unique<RealSubject>();
        }
        real_subject_->Request();
    }

private:
    unique_ptr<RealSubject> real_subject_;
};

int main(int argc, const char *argv[])
{
    unique_ptr<Proxy> proxy = make_unique<Proxy>();
    proxy->Request();
}