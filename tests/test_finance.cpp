#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <cstdio>

#include <gtest/gtest.h>

#include "../Account.hpp"
#include "../User.hpp"
#define CALCULATIONS_NO_MAIN
#include "../calculations.cpp"


// Account Unit Tests

TEST(AccountTest, DefaultConstructor) {
    CheckingAccount ch;
    EXPECT_DOUBLE_EQ(ch.getBalance(), 0.0);
    EXPECT_DOUBLE_EQ(ch.getInterestRate(), 0.0);
}

TEST(AccountTest, ParameterizedConstructor) {
    SavingsAccount sa(1500.50, 4.25);
    EXPECT_DOUBLE_EQ(sa.getBalance(), 1500.50);
    EXPECT_DOUBLE_EQ(sa.getInterestRate(), 4.25);
}

TEST(AccountTest, SettersAndTransactions) {
    RetirementAccount ret(2000.0, 1.0);
    ret.setBalance(3000.0);
    ret.setInterestRate(2.5);
    EXPECT_DOUBLE_EQ(ret.getBalance(), 3000.0);
    EXPECT_DOUBLE_EQ(ret.getInterestRate(), 2.5);

    makeTransaction(ret, 500.0);
    EXPECT_DOUBLE_EQ(ret.getBalance(), 3500.0);

    makeTransaction(ret, -200.0);
    EXPECT_DOUBLE_EQ(ret.getBalance(), 3300.0);
}

TEST(AccountTest, MakeTransferSuccess) {
    CheckingAccount from(1000.0, 0.5);
    SavingsAccount to(200.0, 3.5);

    bool ok = makeTransfer(from, to, 350.0);
    EXPECT_TRUE(ok);
    EXPECT_DOUBLE_EQ(from.getBalance(), 650.0);
    EXPECT_DOUBLE_EQ(to.getBalance(), 550.0);
}

TEST(AccountTest, MakeTransferInsufficientFunds) {
    CheckingAccount from(100.0, 0.5);
    SavingsAccount to(500.0, 3.5);

    bool ok = makeTransfer(from, to, 250.0);
    EXPECT_FALSE(ok);
    EXPECT_DOUBLE_EQ(from.getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(to.getBalance(), 500.0);
}


// User & Portfolio Unit Tests


TEST(UserTest, UserConstructorAndGetters) {
    CheckingAccount ch(1000.0, 1.0);
    SavingsAccount sa(2000.0, 4.0);
    RetirementAccount ret(5000.0, 0.0);
    std::map<std::string, int> port = {{"AAPL", 10}, {"GOOG", 5}};

    User user("Alex", ch, sa, ret, port);
    EXPECT_EQ(user.getName(), "Alex");
    EXPECT_DOUBLE_EQ(user.getChecking().getBalance(), 1000.0);
    EXPECT_DOUBLE_EQ(user.getSavings().getBalance(), 2000.0);
    EXPECT_DOUBLE_EQ(user.getRetirement().getBalance(), 5000.0);
    EXPECT_EQ(user.getPortfolio().at("AAPL"), 10);
    EXPECT_EQ(user.getPortfolio().at("GOOG"), 5);
}

TEST(UserTest, UserTransferBetweenAccounts) {
    CheckingAccount ch(1000.0, 1.0);
    SavingsAccount sa(500.0, 4.0);
    RetirementAccount ret(200.0, 0.0);
    User user("Alex", ch, sa, ret, {});

    EXPECT_TRUE(user.transfer("checking", "savings", 400.0));
    EXPECT_DOUBLE_EQ(user.getChecking().getBalance(), 600.0);
    EXPECT_DOUBLE_EQ(user.getSavings().getBalance(), 900.0);

    EXPECT_TRUE(user.transfer("savings", "retirement", 100.0));
    EXPECT_DOUBLE_EQ(user.getSavings().getBalance(), 800.0);
    EXPECT_DOUBLE_EQ(user.getRetirement().getBalance(), 300.0);

    // Overdraft attempt
    EXPECT_FALSE(user.transfer("checking", "savings", 9999.0));
    EXPECT_DOUBLE_EQ(user.getChecking().getBalance(), 600.0);

    // Unknown account
    EXPECT_FALSE(user.transfer("bitcoin", "savings", 50.0));
}

TEST(UserTest, UserDepositAndWithdraw) {
    CheckingAccount ch(500.0, 0.5);
    SavingsAccount sa(1000.0, 4.0);
    RetirementAccount ret(0.0, 0.0);
    User user("Alex", ch, sa, ret, {});

    EXPECT_TRUE(user.deposit("checking", 250.0));
    EXPECT_DOUBLE_EQ(user.getChecking().getBalance(), 750.0);

    EXPECT_TRUE(user.withdraw("savings", 300.0));
    EXPECT_DOUBLE_EQ(user.getSavings().getBalance(), 700.0);

    EXPECT_FALSE(user.withdraw("savings", 2000.0)); // Insufficient
    EXPECT_DOUBLE_EQ(user.getSavings().getBalance(), 700.0);
}

TEST(UserTest, PortfolioAddAndRemoveStocks) {
    User user;
    user.addStock("NVDA", 10);
    EXPECT_EQ(user.getPortfolio().at("NVDA"), 10);

    user.addStock("NVDA", 5);
    EXPECT_EQ(user.getPortfolio().at("NVDA"), 15);

    EXPECT_TRUE(user.removeStock("NVDA", 5));
    EXPECT_EQ(user.getPortfolio().at("NVDA"), 10);

    EXPECT_TRUE(user.removeStock("NVDA", 10)); // All shares removed
    EXPECT_EQ(user.getPortfolio().count("NVDA"), 0);

    EXPECT_FALSE(user.removeStock("NVDA", 1)); // Not in portfolio
}

TEST(UserTest, JsonSerializationRoundTrip) {
    CheckingAccount ch(1234.50, 0.75);
    SavingsAccount sa(5678.90, 4.50);
    RetirementAccount ret(9999.00, 1.25);
    std::map<std::string, int> port = {{"AAPL", 15}, {"MSFT", 8}};

    User original("GrantTest", ch, sa, ret, port);
    const std::string tmpFile = "temp_test_roundtrip.json";

    original.save_to_json(tmpFile);

    User loaded(tmpFile);
    EXPECT_EQ(loaded.getName(), "GrantTest");
    EXPECT_DOUBLE_EQ(loaded.getChecking().getBalance(), 1234.50);
    EXPECT_DOUBLE_EQ(loaded.getChecking().getInterestRate(), 0.75);
    EXPECT_DOUBLE_EQ(loaded.getSavings().getBalance(), 5678.90);
    EXPECT_DOUBLE_EQ(loaded.getSavings().getInterestRate(), 4.50);
    EXPECT_DOUBLE_EQ(loaded.getRetirement().getBalance(), 9999.00);
    EXPECT_DOUBLE_EQ(loaded.getRetirement().getInterestRate(), 1.25);
    EXPECT_EQ(loaded.getPortfolio().at("AAPL"), 15);
    EXPECT_EQ(loaded.getPortfolio().at("MSFT"), 8);

    std::remove(tmpFile.c_str());
}


// Calculation & Helper Tests

TEST(CalculationsTest, IsNumberValidation) {
    EXPECT_TRUE(isNumber("12345"));
    EXPECT_TRUE(isNumber("0"));
    EXPECT_FALSE(isNumber(""));
    EXPECT_FALSE(isNumber("12a45"));
    EXPECT_FALSE(isNumber("-10"));
    EXPECT_FALSE(isNumber("12.34"));
}

TEST(CalculationsTest, CompoundingFlatGrowth) {
    // $1,000 growing at 10% for 1 year with annual compounding and 0 inputs
    double res = calculateCompounding(1000.0, 0.0, 10.0, 365, 365, 365);
    EXPECT_NEAR(res, 1100.0, 1e-2);
}

TEST(CalculationsTest, CompoundingPeriodicInputs) {
    // $0 starting, $100 added monthly (every 30 days) for 360 days, 0% growth
    double res = calculateCompounding(0.0, 100.0, 0.0, 360, 30, 365);
    EXPECT_DOUBLE_EQ(res, 1200.0);
}

TEST(CalculationsTest, CompoundingZeroInputSafety) {
    // Guard against inputTimeFrame == 0 dividing by zero
    double res = calculateCompounding(1000.0, 100.0, 5.0, 365, 0, 365);
    EXPECT_GT(res, 1000.0);
}


// Monte Carlo Tests


TEST(MonteCarloTest, SimulationCountAndBounds) {
    const int numSims = 1500;
    MonteCarloResult res = calculateCompoundingMonteCarlo(
        10000.0, 200.0, 7.0, 15.0, 5 * 365, 30, 365, numSims, 25000.0
    );

    EXPECT_EQ(res.numSimulations, numSims);
    EXPECT_EQ((int)res.simulationEndBalances.size(), numSims);
    EXPECT_GT(res.mean, 0.0);
    EXPECT_LE(res.min, res.max);
    EXPECT_GT(res.stdDev, 0.0);
}

TEST(MonteCarloTest, MonotonicPercentiles) {
    MonteCarloResult res = calculateCompoundingMonteCarlo(
        15000.0, 500.0, 8.0, 16.0, 10 * 365, 30, 365, 2000, 100000.0
    );

    EXPECT_LE(res.min, res.p10);
    EXPECT_LE(res.p10, res.p25);
    EXPECT_LE(res.p25, res.median);
    EXPECT_LE(res.median, res.p75);
    EXPECT_LE(res.p75, res.p90);
    EXPECT_LE(res.p90, res.max);
}

TEST(MonteCarloTest, ArbitraryPercentilesMethod) {
    MonteCarloResult res = calculateCompoundingMonteCarlo(
        5000.0, 100.0, 6.0, 12.0, 3 * 365, 30, 365, 1000, 0.0
    );

    EXPECT_DOUBLE_EQ(res.getPercentile(0.0), res.min);
    EXPECT_DOUBLE_EQ(res.getPercentile(100.0), res.max);
    EXPECT_LE(res.getPercentile(25.0), res.getPercentile(75.0));
}

TEST(MonteCarloTest, TargetSuccessRateCalculation) {
    // Very low target should have ~100% success rate
    MonteCarloResult lowTarget = calculateCompoundingMonteCarlo(
        50000.0, 0.0, 5.0, 10.0, 5 * 365, 365, 365, 1000, 1000.0
    );
    EXPECT_DOUBLE_EQ(lowTarget.successRate, 100.0);

    // Extremely high target should have ~0% success rate
    MonteCarloResult highTarget = calculateCompoundingMonteCarlo(
        1000.0, 0.0, 5.0, 10.0, 1 * 365, 365, 365, 1000, 10000000.0
    );
    EXPECT_DOUBLE_EQ(highTarget.successRate, 0.0);
}

TEST(MonteCarloTest, HistoricalVolatilityFallback) {
    std::map<std::string, int> port = {{"FAKE_TICKER", 10}};
    // Insufficient historical years (< 2) returns 15.0% default
    double vol = calculateHistoricalVolatility(port, 1);
    EXPECT_DOUBLE_EQ(vol, 15.0);
}

// =========================================================================
// Main Entry Point
// =========================================================================


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

