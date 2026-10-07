#include <iostream>
#include <cmath>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <fstream>
#include "User.hpp"


using namespace std;

using json = nlohmann::json;

json fetchPrices(const map<string, int> &portfolio, int years){
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

map<string, double> getValueOfStocks(const map<string, int> &portfolio, int years = 0){
    json data = fetchPrices(portfolio, years);
    map<string, double> prices;
    for (auto &[ticker, info] : data.items()) {
        if (info.is_number()) prices[ticker] = info.get<double>();
        else cerr << "no price data for " << ticker << ": "
                  << (info.is_string() ? info.get<string>() : info.dump()) << endl;
    }
    return prices;
}

double calculatePortfolioValue(const map<string, int> &portfolio, int years = 0){
    double value = 0;
    map<string, double> stockPrices = getValueOfStocks(portfolio, years);
    for(auto const& pair : portfolio){
        value += stockPrices[pair.first] * pair.second;
    }
    return value;
}

//number of years before today to look back at
double calculateAverageYearlyReturn(const map<string, int> &portfolio, int numYears){
    double endValue = calculatePortfolioValue(portfolio);
    double startValue = calculatePortfolioValue(portfolio, numYears);

    return 100 * (pow(endValue/startValue, (1.0/numYears)) - 1);
}

//timeframes in days
//move their own versions into account??
//error when inputTimeFrame is 0
double calculateCompounding(double startingBalance, double input, double percentYearlyGrowth,
            int timeFrame, int inputTimeFrame, int compoundingTimeFrame){
    double bal = startingBalance;
    double amountToCompound = (percentYearlyGrowth/ 100) * (compoundingTimeFrame / 365.0);

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

double calculatePortfolioGrowth(const map<string, int> &portfolio, double input, int inputTimeFrame, int historicalYears, int futureYears){
    double startingBalance = calculatePortfolioValue(portfolio);
    return calculateCompounding(startingBalance, input, calculateAverageYearlyReturn(portfolio, historicalYears), futureYears * 365, inputTimeFrame, 365);
}

bool isNumber(const string &a){
    if(a.empty()){
        return false;
    }

    for(char c : a){
        if(!isdigit(c)){
            return false;
        }
    }
    return true;
}