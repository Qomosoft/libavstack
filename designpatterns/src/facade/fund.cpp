#include <iostream>
#include <memory>

using namespace std;

class Stock1 {
public:
    void Sell() {
        cout << "Sell stock 1\n";
    }

    void Buy() {
        cout << "Buy stock 1\n";
    }
};

class Stock2 {
public:
    void Sell() {
        cout << "Sell stock 2\n";
    }

    void Buy() {
        cout << "Buy stock 2\n";
    }
};

class Stock3 {
public:
    void Sell() {
        cout << "Sell stock 3\n";
    }

    void Buy() {
        cout << "Buy stock 3\n";
    }
};

class NationalDebt1 {
public:
    void Sell() {
        cout << "Sell national debt 1\n";
    }

    void Buy() {
        cout << "Buy national debt 1\n";
    }
};

class Realty1 {
public:
    void Sell() {
        cout << "Sell realty 1\n";
    }

    void Buy() {
        cout << "Buy realty 1\n";
    }
};

class Fund {
public:
    void BuyFund() {
        stock1.Buy();
        stock2.Buy();
        stock3.Buy();
        debt1.Buy();
        realty1.Buy();
    }

    void SellFund() {
        stock1.Sell();
        stock2.Sell();
        stock3.Sell();
        debt1.Sell();
        realty1.Sell();
    }

private:
    Stock1 stock1;
    Stock2 stock2;
    Stock3 stock3;
    NationalDebt1 debt1;
    Realty1 realty1;
};

int main(int argc, const char *argv[])
{
    Fund fund;
    fund.BuyFund();
    fund.SellFund();
    return 0;
}