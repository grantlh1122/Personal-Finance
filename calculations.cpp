#include <iostream>
#include <cmath>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <fstream>


using namespace std;

using json = nlohmann::json;

json fetchPrices(const map<string, double> &portfolio, int years){
    string cmd = "./.venv/bin/python prices.py";
    for (const auto &[ticker, quantity] : portfolio) cmd += " " + ticker;
    cmd += " " + to_string(years) + " 2>/dev/null";
    FILE *pipe = popen(cmd.c_str(), "r");
    char buf[4096];
    string out;
    while (fgets(buf, sizeof buf, pipe)) out += buf;
    pclose(pipe);
    if (out.empty()) return json::object();
    return json::parse(out);
}

map<string, double> getValueOfStocks(const map<string, double> &portfolio, int years = 0){
    json data = fetchPrices(portfolio, years);
    map<string, double> prices;
    for (auto &[ticker, info] : data.items()) {
        if (info.is_number()) prices[ticker] = info.get<double>();
        else cerr << "no price data for " << ticker << ": "
                  << (info.is_string() ? info.get<string>() : info.dump()) << endl;
    }
    return prices;
}

//use map to calculate weighted total of portfolio of each year, then calculate return rate each year, 
//then calculate average return rate of portfolio over the years


//getReturnRateOfPortfolio(portfolio,historicalTimeFrame)
//check portfolio for more info




int main(){
    ifstream file("grant.json");
    json account = json::parse(file);
    map<string, double> portfolio = account["portfolio"].get<map<string, double>>();

    for (const auto &[ticker, price] : getValueOfStocks(portfolio, 4))
        cout << ticker << ": " << price << endl;
}