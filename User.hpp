#include <iostream>
#include "Account.hpp"
#include <nlohmann/json.hpp>
#include <string>

#pragma once

class User{
    
    public:

        User();
        User(const std::string &filename);
        User(const std::string &name,
             const CheckingAccount &checking,
             const SavingsAccount &savings,
             const RetirementAccount &retirement,
             const std::map<std::string, int> &portfolio);

        // saves account data to json file
        void save_to_json(const std::string &filename);

        const std::string& getName() const;
        void setName(const std::string &name);

        CheckingAccount& getChecking();
        const CheckingAccount& getChecking() const;

        SavingsAccount& getSavings();
        const SavingsAccount& getSavings() const;

        RetirementAccount& getRetirement();
        const RetirementAccount& getRetirement() const;

        std::map<std::string, int> getPortfolio() const;
        void setPortfolio(const std::map<std::string, int> &p);
        void addStock(const std::string &ticker, int quantity);
        bool removeStock(const std::string &ticker, int quantity);

        bool transfer(const std::string &fromAccount, const std::string &toAccount, double amount);
        bool deposit(const std::string &accountName, double amount);
        bool withdraw(const std::string &accountName, double amount);
        
    private:
        std::string name;
        CheckingAccount checking;
        SavingsAccount savings;
        RetirementAccount retirement;
        std::map<std::string, int> portfolio;

        Account* resolveAccount(const std::string &accountName);
};
