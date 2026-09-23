#include <iostream>
#include "Account.hpp"
#include <nlohmann/json.hpp>
#include <string>

class User{
    private: 
        std::string name;
        CheckingAccount checking;
        SavingsAccount savings;
        RetirementAccount retirement;
        //portfolio
    public:
        User(const nlohmann::json&userData);
        //void saveDataToJson(fileNameToBeSavedTo);
        
};

//portfolio class with a map that stores tickers? is that necesary? 
//yep. then make another file that just gets the data needed for the portfolio.
//in the cpp file, somehow figure out getting the input data into the json. you can do it.