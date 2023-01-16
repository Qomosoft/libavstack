#include <iostream>
#include <string>
#include <memory>
#include <cmath>

using namespace std;

class CashSuper {
public:
    virtual double acceptCash(const double money) = 0;
};

class CashNormal : public CashSuper {
public:
    double acceptCash(const double money) override {
        return money;
    }
};

class CashRebate : public CashSuper {
public:
    CashRebate(double rebate) : rebate_(rebate) {}

    double acceptCash(const double money) override {
        return money * rebate_;
    }

private:
    double rebate_ = 1.0;
};

class CashReturn : public CashSuper {
public:
    CashReturn(double moneyCondition, double moneyReturn) : money_condition_(moneyCondition),
                                                            money_return(moneyReturn) {}

    double acceptCash(const double money) override {
        if (money >= money_condition_) {
            int rate = floor(money / money_condition_);
            return money - rate * money_return;
        }

        return money;
    }

private:
    double money_condition_ = 0.0;
    double money_return = 0.0;
};

class CashContext {
public:
    CashContext(const string &type) {
        if (type == "normal") {
            cs_ = make_unique<CashNormal>();
        } else if (type == "return") {
            cs_ = make_unique<CashReturn>(300, 100);
        } else if (type == "rebate") {
            cs_ = make_unique<CashRebate>(0.8);
        } else {
            cs_ = make_unique<CashNormal>();
        }
    }

    double result(const double money) {
        return cs_->acceptCash(money);
    }

private:
    unique_ptr<CashSuper> cs_;
};

int main(int argc, const char *argv[])
{
    unique_ptr<CashContext> cash_ctx = make_unique<CashContext>("return");
    cout << cash_ctx->result(400) << endl;
}


