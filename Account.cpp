#include <iostream>
#include <cmath>
#include "Account.hpp"


//defaults to adding to a
void makeTransaction(Account a, int amount){
    a.setBalance(a.getBalance() + amount);
}

//from a to b
void makeTransfer(Account a, Account b, int amount){
    makeTransaction(a, -amount);
    makeTransaction(b, amount);
}


double Account::getBalance(){
            return balance;
        }
void Account::setBalance(double amount){
    balance = amount;
    //implement error check
}
double Account::getInterestRate(){
    return interestRate;
}
void Account::setInterestRate(double rate){
    interestRate = rate;
    //implement error check
}