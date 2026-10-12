#include "Account.hpp"
#include "User.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

User::User() : checking(), savings(), retirement() {}

User::User(const string &filename){
    fs::path filepath(filename);

    if (!fs::exists(filepath) || !fs::is_regular_file(filepath)){
        throw runtime_error("Error: invalid file");
    }

    ifstream fileStream(filepath);
    if(!fileStream.is_open()){
        throw runtime_error("Error: failed to open file");
    }

    try {
        json data = json::parse(fileStream);

        name = data["name"];

        checking.setBalance(data["checking"][0]);
        checking.setInterestRate(data["checking"][1]);

        savings.setBalance(data["savings"][0]);
        savings.setInterestRate(data["savings"][1]);

        retirement.setBalance(data["retirement"][0]);
        retirement.setInterestRate(data["retirement"][1]);

        portfolio = data["portfolio"];
    } catch (const json::parse_error &error){
        throw runtime_error(string("JSON parsing error: ") + error.what());
    }
    
}
    

void User::save_to_json(const string &filename){
    json output_data;
    output_data["name"] = name;
    output_data["checking"] = {checking.getBalance(), checking.getInterestRate()};
    output_data["savings"] = {savings.getBalance(), savings.getInterestRate()};
    output_data["retirement"] = {retirement.getBalance(), retirement.getInterestRate()};
    output_data["portfolio"] = portfolio;


    ofstream file(filename);
    if(file.is_open()){
        file << output_data.dump(4);
        file.close();
        cout << "Success!" << endl;
    } else {
        cerr << "Error" << endl;
    }
}

User::User(const string &name,
           const CheckingAccount &checking,
           const SavingsAccount &savings,
           const RetirementAccount &retirement,
           const map<string, int> &portfolio)
    : name(name), checking(checking), savings(savings), retirement(retirement), portfolio(portfolio) {}

const string& User::getName() const {
    return name;
}

void User::setName(const string &n) {
    name = n;
}

CheckingAccount& User::getChecking() {
    return checking;
}

const CheckingAccount& User::getChecking() const {
    return checking;
}

SavingsAccount& User::getSavings() {
    return savings;
}

const SavingsAccount& User::getSavings() const {
    return savings;
}

RetirementAccount& User::getRetirement() {
    return retirement;
}

const RetirementAccount& User::getRetirement() const {
    return retirement;
}

std::map<std::string, int> User::getPortfolio() const {
    return portfolio;
}

void User::setPortfolio(const map<string, int> &p) {
    portfolio = p;
}

void User::addStock(const string &ticker, int quantity) {
    if (quantity > 0) {
        portfolio[ticker] += quantity;
    }
}

bool User::removeStock(const string &ticker, int quantity) {
    if (quantity <= 0) return false;
    auto it = portfolio.find(ticker);
    if (it == portfolio.end() || it->second < quantity) {
        return false;
    }
    it->second -= quantity;
    if (it->second == 0) {
        portfolio.erase(it);
    }
    return true;
}

Account* User::resolveAccount(const string &accountName) {
    string lower = accountName;
    for (char &c : lower) c = tolower(c);
    if (lower == "checking" || lower == "check") return &checking;
    if (lower == "savings" || lower == "saving" || lower == "save") return &savings;
    if (lower == "retirement" || lower == "retire") return &retirement;
    return nullptr;
}

bool User::transfer(const string &fromAccount, const string &toAccount, double amount) {
    if (amount <= 0.0) {
        cerr << "Error: Transfer amount must be positive." << endl;
        return false;
    }
    Account *from = resolveAccount(fromAccount);
    Account *to = resolveAccount(toAccount);
    if (!from) {
        cerr << "Error: Unknown source account '" << fromAccount << "'. Valid options: checking, savings, retirement" << endl;
        return false;
    }
    if (!to) {
        cerr << "Error: Unknown destination account '" << toAccount << "'. Valid options: checking, savings, retirement" << endl;
        return false;
    }
    if (from == to) {
        cerr << "Error: Cannot transfer to the same account." << endl;
        return false;
    }
    if (from->getBalance() < amount) {
        cerr << "Error: Insufficient funds in " << fromAccount << ". Current balance: $" << from->getBalance() << endl;
        return false;
    }
    from->setBalance(from->getBalance() - amount);
    to->setBalance(to->getBalance() + amount);
    return true;
}

bool User::deposit(const string &accountName, double amount) {
    if (amount <= 0.0) {
        cerr << "Error: Deposit amount must be positive." << endl;
        return false;
    }
    Account *acc = resolveAccount(accountName);
    if (!acc) {
        cerr << "Error: Unknown account '" << accountName << "'. Valid options: checking, savings, retirement" << endl;
        return false;
    }
    acc->setBalance(acc->getBalance() + amount);
    return true;
}

bool User::withdraw(const string &accountName, double amount) {
    if (amount <= 0.0) {
        cerr << "Error: Withdrawal amount must be positive." << endl;
        return false;
    }
    Account *acc = resolveAccount(accountName);
    if (!acc) {
        cerr << "Error: Unknown account '" << accountName << "'. Valid options: checking, savings, retirement" << endl;
        return false;
    }
    if (acc->getBalance() < amount) {
        cerr << "Error: Insufficient funds in " << accountName << ". Current balance: $" << acc->getBalance() << endl;
        return false;
    }
    acc->setBalance(acc->getBalance() - amount);
    return true;
}
