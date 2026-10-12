#include <iostream>
#include <cmath>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>
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


double calculateCompounding(double startingBalance, double input, double percentYearlyGrowth,
            int timeFrame, int inputTimeFrame, int compoundingTimeFrame){
    double bal = startingBalance;
    double amountToCompound = (compoundingTimeFrame > 0)
        ? (percentYearlyGrowth / 100.0) * (compoundingTimeFrame / 365.0)
        : 0.0;

    for(int day = 1; day <= timeFrame; day++){
        if(inputTimeFrame > 0 && day % inputTimeFrame == 0){
            bal += input;
        }
        if(compoundingTimeFrame > 0 && day % compoundingTimeFrame == 0){
            bal *= 1 + amountToCompound;
        }
    }
    return bal;
}

double calculatePortfolioGrowth(const map<string, int> &portfolio, double input, int inputTimeFrame, int historicalYears, int futureYears){
    double startingBalance = calculatePortfolioValue(portfolio);
    return calculateCompounding(startingBalance, input, calculateAverageYearlyReturn(portfolio, historicalYears), futureYears * 365, inputTimeFrame, 365);
}

struct MonteCarloResult {
    int numSimulations = 0;
    double mean = 0.0;
    double stdDev = 0.0;
    double median = 0.0;      // 50th percentile
    double p10 = 0.0;         // 10th percentile (bear market / conservative)
    double p25 = 0.0;         // 25th percentile
    double p75 = 0.0;         // 75th percentile
    double p90 = 0.0;         // 90th percentile (bull market / optimistic)
    double min = 0.0;
    double max = 0.0;
    double target = 0.0;
    double successRate = 0.0; // Percentage of simulations reaching or exceeding target (0-100%)
    vector<double> simulationEndBalances; // All sorted end balances

    double getPercentile(double p) const {
        if (simulationEndBalances.empty()) return 0.0;
        if (p <= 0.0) return simulationEndBalances.front();
        if (p >= 100.0) return simulationEndBalances.back();
        double index = (p / 100.0) * (simulationEndBalances.size() - 1);
        size_t lower = static_cast<size_t>(index);
        size_t upper = lower + 1;
        if (upper >= simulationEndBalances.size()) return simulationEndBalances.back();
        double weight = index - lower;
        return simulationEndBalances[lower] * (1.0 - weight) + simulationEndBalances[upper] * weight;
    }

    void print() const {
        cout << fixed << setprecision(2);
        cout << "\n================ Monte Carlo Simulation Results ================\n";
        cout << "Simulations:             " << numSimulations << "\n";
        cout << "Mean Ending Balance:     $" << mean << "\n";
        cout << "Standard Deviation:      $" << stdDev << "\n";
        cout << "10th Percentile (P10):   $" << p10 << " (Conservative / Bear Market)\n";
        cout << "25th Percentile (P25):   $" << p25 << "\n";
        cout << "50th Percentile (Median):$" << median << "\n";
        cout << "75th Percentile (P75):   $" << p75 << "\n";
        cout << "90th Percentile (P90):   $" << p90 << " (Optimistic / Bull Market)\n";
        cout << "Worst Case (Min):        $" << min << "\n";
        cout << "Best Case (Max):         $" << max << "\n";
        if (target > 0.0) {
            cout << "Success Rate (>= $" << target << "): " << successRate << "%\n";
        }
        cout << "================================================================\n";
    }
};

// Calculates historical annual volatility (standard deviation of yearly returns in percent).
// Returns 15.0% as a standard default if historical data is insufficient or invalid.
double calculateHistoricalVolatility(const map<string, int> &portfolio, int numYears) {
    if (numYears < 2) return 15.0;

    vector<double> annualReturns;
    double prevVal = calculatePortfolioValue(portfolio, numYears);
    for (int y = numYears - 1; y >= 0; --y) {
        double currVal = calculatePortfolioValue(portfolio, y);
        if (prevVal > 0.0 && currVal > 0.0) {
            double r = (currVal - prevVal) / prevVal;
            annualReturns.push_back(r);
        }
        prevVal = currVal;
    }

    if (annualReturns.size() < 2) return 15.0;

    double sum = 0.0;
    for (double r : annualReturns) sum += r;
    double mean = sum / annualReturns.size();

    double var = 0.0;
    for (double r : annualReturns) {
        var += (r - mean) * (r - mean);
    }
    var /= (annualReturns.size() - 1);

    double vol = sqrt(var) * 100.0;
    return (vol > 0.0 && !isnan(vol) && !isinf(vol)) ? vol : 15.0;
}

// Similar to calculateCompounding, but performs stochastic Monte Carlo simulations.
// percentYearlyGrowth: mean expected annual return rate in percent (e.g. 7.0 for 7%)
// annualVolatility: standard deviation of annual returns in percent (e.g. 15.0 for 15%)
// timeFrame: total duration in days (e.g. futureYears * 365)
// inputTimeFrame: frequency of contributions in days (e.g. 30 for monthly, 365 for annual)
// compoundingTimeFrame: frequency of compounding in days (default: 365)
// numSimulations: number of simulation runs (default: 10000)
// target: optional financial target to calculate success rate (default: 0.0)
MonteCarloResult calculateCompoundingMonteCarlo(
    double startingBalance,
    double input,
    double percentYearlyGrowth,
    double annualVolatility,
    int timeFrame,
    int inputTimeFrame = 365,
    int compoundingTimeFrame = 365,
    int numSimulations = 10000,
    double target = 0.0
) {
    if (numSimulations <= 0) numSimulations = 1000;
    if (compoundingTimeFrame <= 0) compoundingTimeFrame = 365;

    double dt = static_cast<double>(compoundingTimeFrame) / 365.0;
    double periodMean = (percentYearlyGrowth / 100.0) * dt;
    double periodStdDev = (annualVolatility / 100.0) * sqrt(dt);

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> dist(periodMean, periodStdDev);

    MonteCarloResult result;
    result.numSimulations = numSimulations;
    result.target = target;
    result.simulationEndBalances.resize(numSimulations);

    double sum = 0.0;
    int successCount = 0;

    for (int i = 0; i < numSimulations; ++i) {
        double bal = startingBalance;
        for (int day = 1; day <= timeFrame; ++day) {
            if (inputTimeFrame > 0 && day % inputTimeFrame == 0) {
                bal += input;
            }
            if (day % compoundingTimeFrame == 0) {
                double r = dist(gen);
                bal *= (1.0 + r);
                if (bal < 0.0) bal = 0.0;
            }
        }
        result.simulationEndBalances[i] = bal;
        sum += bal;
        if (target > 0.0 && bal >= target) {
            successCount++;
        }
    }

    sort(result.simulationEndBalances.begin(), result.simulationEndBalances.end());

    result.mean = sum / numSimulations;
    result.min = result.simulationEndBalances.front();
    result.max = result.simulationEndBalances.back();
    result.p10 = result.getPercentile(10.0);
    result.p25 = result.getPercentile(25.0);
    result.median = result.getPercentile(50.0);
    result.p75 = result.getPercentile(75.0);
    result.p90 = result.getPercentile(90.0);

    double varianceSum = 0.0;
    for (double bal : result.simulationEndBalances) {
        double diff = bal - result.mean;
        varianceSum += diff * diff;
    }
    result.stdDev = sqrt(varianceSum / numSimulations);

    if (target > 0.0) {
        result.successRate = (static_cast<double>(successCount) / numSimulations) * 100.0;
    }

    return result;
}

// Similar to calculatePortfolioGrowth, but supports Monte Carlo predictions
// across futureYears with expected growth from historical data and customizable volatility.
MonteCarloResult calculatePortfolioMonteCarlo(
    const map<string, int> &portfolio,
    double input,
    int inputTimeFrame,
    int historicalYears,
    int futureYears,
    double annualVolatility = 15.0,
    int numSimulations = 10000,
    double target = 0.0,
    double startingCash = 0.0
) {
    double startingBalance = calculatePortfolioValue(portfolio) + startingCash;
    double expectedYearlyGrowth = calculateAverageYearlyReturn(portfolio, historicalYears);
    if (isnan(expectedYearlyGrowth) || isinf(expectedYearlyGrowth)) {
        expectedYearlyGrowth = 7.0; // fallback to standard 7% long-term market average
    }

    if (annualVolatility <= 0.0) {
        annualVolatility = calculateHistoricalVolatility(portfolio, historicalYears);
    }

    return calculateCompoundingMonteCarlo(
        startingBalance,
        input,
        expectedYearlyGrowth,
        annualVolatility,
        futureYears * 365,
        inputTimeFrame,
        365,
        numSimulations,
        target
    );
}

// Convenient alias matching calculatePortfolioGrowth naming convention
MonteCarloResult calculatePortfolioGrowthMonteCarlo(
    const map<string, int> &portfolio,
    double input,
    int inputTimeFrame,
    int historicalYears,
    int futureYears,
    double annualVolatility = 15.0,
    int numSimulations = 10000,
    double target = 0.0,
    double startingCash = 0.0
) {
    return calculatePortfolioMonteCarlo(
        portfolio, input, inputTimeFrame, historicalYears, futureYears,
        annualVolatility, numSimulations, target, startingCash
    );
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