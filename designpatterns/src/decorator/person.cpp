#include <iostream>
#include <string>
#include <memory>

using namespace std;

//Component & ConcreteComponent
class Person {
public:
    Person() {}
    Person(const string &name) : name_(name) {}

    virtual void show() {
        cout << "decorated " << name_;
    }

private:
    string name_;
};

//Decorator
class Finery : public Person {
public:
    void Decorator(shared_ptr<Person> component) {
        component_ = component;
    }

    void show() override {
        if (component_) {
            component_->show();
        }
    }
protected:
    shared_ptr<Person> component_;
};

//ConcreteDecorator
class TShirts : public Finery {
public:
    void show() override {
        cout << "big T shirts ";
        Finery::show();
    }
};

class BigTrouser : public Finery {
public:
    void show() override {
        cout << "big trouser ";
        Finery::show();
    }
};

int main(int argc, const char *argv[])
{
    shared_ptr<Person> cai = make_shared<Person>("cai");
    cout << "First decorator:\n";
    shared_ptr<TShirts> shirts = make_shared<TShirts>();
    shared_ptr<BigTrouser> trouser = make_shared<BigTrouser>();

    shirts->Decorator(cai);
    trouser->Decorator(shirts);
    trouser->show();

    return 0;
}