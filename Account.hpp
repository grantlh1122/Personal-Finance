#include <iostream>
#include <cmath>


class Account{
    private:
        double balance;
        double interestRate;

    public:
        Account() : balance(0), interestRate(0) {}
        Account(double balance) : balance(balance), interestRate(0) {}
        Account(double balance, double interestRate) : balance(balance), interestRate(interestRate) {}

        double getBalance(){
            return balance;
        }
        void setBalance(double amount){
            balance = amount;
        }
        double getInterestRate(){
            return interestRate;
        }
        double calculateInterest(double input, 
            int timeFrame, int inputTimeFrame, int compoundingTimeFrame){
            double bal = balance;
            double amountToCompound = (interestRate / 100) * (compoundingTimeFrame / 365.0);

            for(int day = 1; day <= timeFrame; day++){
                if(day % inputTimeFrame == 0){
                    bal += input;
                }
                if(day % compoundingTimeFrame == 0){
                    bal *= 1 + amountToCompound;
                }
            }
            return bal;
        }

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

//defaults to adding to a
void makeTransaction(Account a, int amount){
    a.setBalance(a.getBalance() + amount);
}

//from a to b
void makeTransfer(Account a, Account b, int amount){
    makeTransaction(a, -amount);
    makeTransaction(b, amount);
}
