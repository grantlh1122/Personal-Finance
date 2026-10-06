#include <iostream>
#include <cmath>

#pragma once

class Account{
    public:
        Account() : balance(0), interestRate(0) {}
        Account(double balance) : balance(balance), interestRate(0) {}
        Account(double balance, double interestRate) : balance(balance), interestRate(interestRate) {}

        double getBalance();
        void setBalance(double amount);
        double getInterestRate();
        void setInterestRate(double rate);

    private:
        double balance;
        double interestRate;
};



class SavingsAccount : public Account{
    public:
        using Account::Account;


};

class CheckingAccount : public Account{
    public:
        using Account::Account;

};

class RetirementAccount : public Account{
    public:
        using Account::Account;

        //double estimateFutureAccount(NOT tickers, get from user. do calc in calculations, 
        //pass in timeframe to input, timeframe for history, future timeframe using their tickers)
        //implement portfolio
        //balance PLUS tf input cash PLUS tf input into stocks
    private:
        double totalBuyingPower;
        //balance cash plus stock value. We are going to assume all stocks are for retirement.
};

