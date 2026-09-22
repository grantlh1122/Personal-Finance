#pragma once
#include <iostream>

void makeTransaction(Account a, int amount);
void makeTransfer(Account a, Account b, int amount);

class Account{
    private:
        double balance;
        double interestRate;

    public:
        Account(double balance);
        Account(double balance, double interestRate);

        double getBalance();
        double setBalance();
        double getInterestRate();
};



class SavingsAccount : public Account{
    public:
        using Account::Account;

        double calculateSaving(double input, int inputTimeframe, int compoundingTimeframe);

};

class CheckingAccount : public Account{
    public:
        using Account::Account;

};

class RetirementAccount : public Account{
    private:
        double cash;
        //balance cash plus stock value. We are going to assume all stocks are for retirement.
    public:
        using Account::Account;

        //double estimateFutureAccount(NOT tickers, get from user. do calc in calculations, 
        //pass in timeframe to input, timeframe for history, future timeframe using their tickers)
        //implement portfolio
        //balance PLUS tf input cash PLUS tf input into stocks
};