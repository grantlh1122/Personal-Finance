#pragma once
#include <iostream>

class Account{
private:
    std::string accountType;
    double balance;

public:
    Account(std::string accountType, double balance);

    double getBalance();
    void makeTransation();
    void makeTransfer();
    //non member functions?
};