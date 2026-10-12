#include <iostream>
#include <cmath>
#include "Account.hpp"


// Adds amount to account a (positive for deposit, negative for withdrawal)
void makeTransaction(Account &a, double amount){
    a.setBalance(a.getBalance() + amount);
}

// Transfers amount from a to b with balance check
bool makeTransfer(Account &a, Account &b, double amount){
    if (a.getBalance() < amount) {
        return false;
    }
    makeTransaction(a, -amount);
    makeTransaction(b, amount);
    return true;
}


double Account::getBalance() const {
    return balance;
}

void Account::setBalance(double amount){
    balance = amount;
}

double Account::getInterestRate() const {
    return interestRate;
}

void Account::setInterestRate(double rate){
    interestRate = rate;
}