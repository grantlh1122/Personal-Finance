#include <iostream>
#include "Account.hpp"
#include <nlohmann/json.hpp>
#include <string>

#pragma once

class User{
    
    public:

        User();
        User(const std::string &filename);

        //saves account data to json file
        void save_to_json(const std::string &filename);

        std::map<std::string, int> getPortfolio();
        
    private:
        std::string name;
        CheckingAccount checking;
        SavingsAccount savings;
        RetirementAccount retirement;
        std::map<std::string,int> portfolio;
        
};
