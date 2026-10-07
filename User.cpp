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

std::map<std::string, int> User::getPortfolio(){
    return portfolio;
}
