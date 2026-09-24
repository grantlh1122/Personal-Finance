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
