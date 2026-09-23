#include "Account.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

using namespace std;
using json = nlohmann::json;

struct User{
    std::string name;
    CheckingAccount checking;
    SavingsAccount savings;
    RetirementAccount retirement;
    map<string,int> portfolio;

    User(const nlohmann::json &userData);
    User() : checking(), savings(), retirement() {}

    void save_to_json(const string &filename){
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
};

int main(){
    User grant;
    grant.name = "Grant";
    grant.checking = {100,5};
    grant.savings = {200,10};
    grant.retirement = {1000, 0};
    grant.portfolio = {{"AAPL", 1}, {"gree", 2}};

    grant.save_to_json("grant.json");
}
//portfolio class with a map that stores tickers? is that necesary? 
//yep. then make another file that just gets the data needed for the portfolio.
//in the cpp file, somehow figure out getting the input data into the json. you can do it.