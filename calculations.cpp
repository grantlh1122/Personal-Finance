#include <iostream>
#include <cmath>
#include <nlohmann/json.hpp>
#include <string>


using namespace std;

using json = nlohmann::json;

double getValueOfStock(string symbol){
    return getValueOfStock(symbol, 0);
}
double getValueOfStock(string symbol, int years){
    string cmd = "./.venv/bin/python prices.py " + symbol + " " + to_string(years) + " 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    char buf[4096];
    string out;
    while (fgets(buf, sizeof buf, pipe)) out += buf;
    pclose(pipe);
    json data = json::parse(out);
    return data["price"];
}
//order of business: function to add price data to map?
// only want to run this once per portfolio kinda
// take porfolio, get json with each stock from year y to zero

//getTotalValueOfStocks(portfolio)
//getReturnRateOfPortfolio(portfolio,historicalTimeFrame)
//check portfolio for more info




int main(){
    cout << getValueOfStock("AAPL", 5) << endl; 
    cout << getValueOfStock("AAPL") << endl; 
}