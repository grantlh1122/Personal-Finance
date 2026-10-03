#include <iostream>
#include "Account.hpp"
#include <nlohmann/json.hpp>
#include <string>

class User{
    
    public:

        User() : checking(), savings(), retirement() {}
        User(const std::string &filename);

        //saves account data to json file
        void save_to_json(const std::string &filename);
        
    private:
        std::string name;
        CheckingAccount checking;
        SavingsAccount savings;
        RetirementAccount retirement;
        std::map<std::string,int> portfolio;
};

//portfolio class with a map that stores tickers? is that necesary? 
//yep. then make another file that just gets the data needed for the portfolio.
//in the cpp file, somehow figure out getting the input data into the json. you can do it.