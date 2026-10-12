#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <limits>
#include "User.hpp"
#include "Account.hpp"
#define CALCULATIONS_NO_MAIN
#include "calculations.cpp"

using namespace std;

// Helper: Trim leading and trailing whitespace
static string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Helper: Convert string to lowercase
static string toLower(const string &s) {
    string res = s;
    for (char &c : res) c = tolower(c);
    return res;
}

// Helper: Tokenize an input line by whitespace
static vector<string> tokenize(const string &line) {
    vector<string> tokens;
    stringstream ss(line);
    string tok;
    while (ss >> tok) tokens.push_back(tok);
    return tokens;
}

// Helper: Safely prompt for a double with an optional default
static double promptDouble(const string &prompt, double defaultValue = -1.0) {
    while (true) {
        cout << prompt;
        if (defaultValue >= 0.0) {
            cout << " [" << fixed << setprecision(2) << defaultValue << "]: ";
        } else {
            cout << ": ";
        }
        string input;
        if (!getline(cin, input)) return (defaultValue >= 0.0 ? defaultValue : 0.0);
        input = trim(input);
        if (input.empty() && defaultValue >= 0.0) return defaultValue;
        try {
            size_t idx;
            double val = stod(input, &idx);
            if (idx == input.size() && val >= 0.0) return val;
        } catch (...) {}
        cout << "  Invalid number. Please enter a valid non-negative number." << endl;
    }
}

// Helper: Safely prompt for an int with an optional default
static int promptInt(const string &prompt, int defaultValue = -1) {
    while (true) {
        cout << prompt;
        if (defaultValue >= 0) {
            cout << " [" << defaultValue << "]: ";
        } else {
            cout << ": ";
        }
        string input;
        if (!getline(cin, input)) return (defaultValue >= 0 ? defaultValue : 0);
        input = trim(input);
        if (input.empty() && defaultValue >= 0) return defaultValue;
        try {
            size_t idx;
            int val = stoi(input, &idx);
            if (idx == input.size() && val >= 0) return val;
        } catch (...) {}
        cout << "  Invalid integer. Please enter a valid non-negative integer." << endl;
    }
}

// Helper: Prompt for a string
static string promptString(const string &prompt, const string &defaultValue = "") {
    while (true) {
        cout << prompt;
        if (!defaultValue.empty()) {
            cout << " [" << defaultValue << "]: ";
        } else {
            cout << ": ";
        }
        string input;
        if (!getline(cin, input)) return defaultValue;
        input = trim(input);
        if (input.empty() && !defaultValue.empty()) return defaultValue;
        if (!input.empty()) return input;
        cout << "  Field cannot be empty. Please try again." << endl;
    }
}

// Interactively create a new user and save to JSON
static User createNewUserInteractive(string &currentFilename) {
    cout << "\n================ CREATE NEW USER PROFILE ================\n";
    string name = promptString("Enter User Name");

    cout << "\n--- Checking Account ---" << endl;
    double checkingBal = promptDouble("Initial Balance ($)", 1000.0);
    double checkingRate = promptDouble("Annual Interest Rate (%)", 0.5);

    cout << "\n--- Savings Account ---" << endl;
    double savingsBal = promptDouble("Initial Balance ($)", 2500.0);
    double savingsRate = promptDouble("Annual Interest Rate (%)", 4.0);

    cout << "\n--- Retirement Account (Cash) ---" << endl;
    double retirementBal = promptDouble("Initial Balance ($)", 5000.0);
    double retirementRate = promptDouble("Annual Interest Rate (%)", 0.0);

    cout << "\n--- Investment Portfolio ---" << endl;
    cout << "Enter stock holdings one per line as '<TICKER> <QUANTITY>' (e.g. AAPL 10)." << endl;
    cout << "Type 'done' or press Enter on an empty line when finished." << endl;
    map<string, int> portfolio;
    while (true) {
        cout << "  Stock holding or 'done': ";
        string line;
        if (!getline(cin, line)) break;
        line = trim(line);
        if (line.empty() || toLower(line) == "done") break;
        stringstream ss(line);
        string ticker;
        int qty;
        if (ss >> ticker >> qty && qty > 0) {
            for (char &c : ticker) c = toupper(c);
            portfolio[ticker] += qty;
            cout << "    Added " << qty << " shares of " << ticker 
                 << " (Current holding: " << portfolio[ticker] << ")" << endl;
        } else {
            cout << "    Invalid input. Please enter ticker and quantity (e.g. GOOG 5)." << endl;
        }
    }

    CheckingAccount checking(checkingBal, checkingRate);
    SavingsAccount savings(savingsBal, savingsRate);
    RetirementAccount retirement(retirementBal, retirementRate);
    User newUser(name, checking, savings, retirement, portfolio);

    cout << "\n--- Export JSON Profile ---" << endl;
    string defaultFile = toLower(name) + ".json";
    string filename = promptString("Enter filename to save JSON", defaultFile);
    if (filename.find(".json") == string::npos) filename += ".json";

    newUser.save_to_json(filename);
    currentFilename = filename;
    cout << "User '" << name << "' created and active in current session!\n" << endl;
    return newUser;
}

// Interactively load an existing user from JSON
static bool loadUserInteractive(User &user, string &currentFilename) {
    while (true) {
        string filename = promptString("Enter JSON filename (or 'cancel')");
        if (toLower(filename) == "cancel") return false;
        try {
            user = User(filename);
            currentFilename = filename;
            cout << "Successfully loaded user '" << user.getName() << "' from " << filename << "!\n" << endl;
            return true;
        } catch (const exception &e) {
            cout << "  " << e.what() << ". Please try again." << endl;
        }
    }
}

// Display user status summary
static void displayStatus(const User &user, const string &currentFilename) {
    cout << fixed << setprecision(2);
    cout << "\n==================== USER FINANCIAL STATUS ====================\n";
    cout << " Profile Name:  " << user.getName() 
         << (!currentFilename.empty() ? (" [" + currentFilename + "]") : "") << "\n";
    cout << "--------------------------------------------------------------\n";
    cout << " Cash Accounts:\n";
    cout << "   Checking:    $" << setw(10) << right << user.getChecking().getBalance()
         << "   |   Rate: " << user.getChecking().getInterestRate() << "% APY\n";
    cout << "   Savings:     $" << setw(10) << right << user.getSavings().getBalance()
         << "   |   Rate: " << user.getSavings().getInterestRate() << "% APY\n";
    cout << "   Retirement:  $" << setw(10) << right << user.getRetirement().getBalance()
         << "   |   Rate: " << user.getRetirement().getInterestRate() << "% APY\n";

    double totalCash = user.getChecking().getBalance() +
                       user.getSavings().getBalance() +
                       user.getRetirement().getBalance();
    cout << "   Total Cash:  $" << setw(10) << right << totalCash << "\n";
    cout << "--------------------------------------------------------------\n";

    map<string, int> port = user.getPortfolio();
    cout << " Investment Portfolio Holdings:\n";
    if (port.empty()) {
        cout << "   (No stocks in portfolio)\n";
    } else {
        map<string, double> prices = getValueOfStocks(port, 0);
        double totalStocks = 0.0;
        for (const auto &[ticker, qty] : port) {
            double price = (prices.count(ticker) ? prices[ticker] : 0.0);
            double val = price * qty;
            totalStocks += val;
            cout << "   " << setw(6) << left << ticker << ": " 
                 << setw(4) << right << qty << " shares";
            if (price > 0.0) {
                cout << " @ $" << setw(8) << right << price << " each = $" << setw(10) << right << val;
            } else {
                cout << " (live price unavailable)";
            }
            cout << "\n";
        }
        cout << "   Total Portfolio Value: $" << setw(10) << right << totalStocks << "\n";
        cout << "--------------------------------------------------------------\n";
        cout << " Estimated Net Worth:     $" << setw(10) << right << (totalCash + totalStocks) << "\n";
    }
    cout << "==============================================================\n" << endl;
}

// Print detailed help
static void printHelp() {
    cout << "\n======================= CLI COMMAND REFERENCE =======================\n";
    cout << " Account Management:\n";
    cout << "   status, view, balance\n";
    cout << "       Display financial overview, account balances, and portfolio.\n\n";
    cout << "   transfer [from] [to] [amount]\n";
    cout << "       Transfer funds between accounts (checking, savings, retirement).\n";
    cout << "       Example: transfer checking savings 250.00\n\n";
    cout << "   deposit [account] [amount]\n";
    cout << "       Deposit funds into an account (checking, savings, retirement).\n";
    cout << "       Example: deposit checking 1000.00\n\n";
    cout << "   withdraw [account] [amount]\n";
    cout << "       Withdraw funds from an account (checking, savings, retirement).\n";
    cout << "       Example: withdraw savings 50.00\n\n";
    cout << " Portfolio Management:\n";
    cout << "   portfolio, stocks\n";
    cout << "       View stock holdings, current market prices, and total value.\n\n";
    cout << "   buy [ticker] [quantity]\n";
    cout << "       Add shares of a stock to your portfolio.\n";
    cout << "       Example: buy AAPL 10\n\n";
    cout << "   sell [ticker] [quantity]\n";
    cout << "       Remove shares of a stock from your portfolio.\n";
    cout << "       Example: sell AAPL 5\n\n";
    cout << " Retirement Forecasting:\n";
    cout << "   plan [input] [freq_days] [hist_years] [future_years]\n";
    cout << "       Deterministic retirement calculator using portfolio CAGR.\n";
    cout << "       Parameters: periodic input ($), frequency (days), lookback (yrs), horizon (yrs)\n";
    cout << "       Example: plan 500 30 5 30\n\n";
    cout << "   montecarlo, mc [input] [freq] [hist_yrs] [future_yrs] [vol%] [sims] [target]\n";
    cout << "       Stochastic Monte Carlo simulation predicting retirement outcomes\n";
    cout << "       with percentiles (P10, P25, Median, P75, P90) and target success rate.\n";
    cout << "       Example: montecarlo 500 30 5 30 15 10000 1000000\n\n";
    cout << " File & Session:\n";
    cout << "   save [filename]    Save current user state to JSON.\n";
    cout << "   load [filename]    Load a different profile from JSON.\n";
    cout << "   new                Create a new user profile.\n";
    cout << "   help               Display this help guide.\n";
    cout << "   exit, quit         Exit the application.\n";
    cout << "=====================================================================\n" << endl;
}

// Handle transfer command
static void handleTransfer(User &user, const vector<string> &args) {
    string fromAcc, toAcc;
    double amount = 0.0;

    if (args.size() >= 4) {
        fromAcc = args[1];
        toAcc = args[2];
        try {
            amount = stod(args[3]);
        } catch (...) {
            cout << "Invalid amount: " << args[3] << endl;
            return;
        }
    } else {
        cout << "\n--- Transfer Funds ---" << endl;
        fromAcc = promptString("From account (checking/savings/retirement)");
        toAcc = promptString("To account (checking/savings/retirement)");
        amount = promptDouble("Amount ($)");
    }

    if (user.transfer(fromAcc, toAcc, amount)) {
        cout << fixed << setprecision(2);
        cout << "Transferred $" << amount << " from " << fromAcc << " to " << toAcc << ".\n";
        cout << "  " << fromAcc << " balance: $" << (fromAcc == "checking" ? user.getChecking().getBalance() : (fromAcc == "savings" ? user.getSavings().getBalance() : user.getRetirement().getBalance())) << "\n";
        cout << "  " << toAcc << " balance: $" << (toAcc == "checking" ? user.getChecking().getBalance() : (toAcc == "savings" ? user.getSavings().getBalance() : user.getRetirement().getBalance())) << endl;
    }
}

// Handle deposit command
static void handleDeposit(User &user, const vector<string> &args) {
    string acc;
    double amount = 0.0;

    if (args.size() >= 3) {
        acc = args[1];
        try {
            amount = stod(args[2]);
        } catch (...) {
            cout << "Invalid amount: " << args[2] << endl;
            return;
        }
    } else {
        cout << "\n--- Deposit Funds ---" << endl;
        acc = promptString("Account (checking/savings/retirement)");
        amount = promptDouble("Deposit Amount ($)");
    }

    if (user.deposit(acc, amount)) {
        cout << fixed << setprecision(2);
        cout << "Deposited $" << amount << " into " << acc << ".\n";
    }
}

// Handle withdraw command
static void handleWithdraw(User &user, const vector<string> &args) {
    string acc;
    double amount = 0.0;

    if (args.size() >= 3) {
        acc = args[1];
        try {
            amount = stod(args[2]);
        } catch (...) {
            cout << "Invalid amount: " << args[2] << endl;
            return;
        }
    } else {
        cout << "\n--- Withdraw Funds ---" << endl;
        acc = promptString("Account (checking/savings/retirement)");
        amount = promptDouble("Withdrawal Amount ($)");
    }

    if (user.withdraw(acc, amount)) {
        cout << fixed << setprecision(2);
        cout << "Withdrew $" << amount << " from " << acc << ".\n";
    }
}

// Handle buy stock
static void handleBuyStock(User &user, const vector<string> &args) {
    string ticker;
    int qty = 0;

    if (args.size() >= 3) {
        ticker = args[1];
        try {
            qty = stoi(args[2]);
        } catch (...) {
            cout << "Invalid quantity: " << args[2] << endl;
            return;
        }
    } else {
        cout << "\n--- Add Stock to Portfolio ---" << endl;
        ticker = promptString("Ticker symbol (e.g. AAPL)");
        qty = promptInt("Number of shares to add");
    }

    if (qty <= 0) {
        cout << "Quantity must be greater than 0." << endl;
        return;
    }

    for (char &c : ticker) c = toupper(c);
    user.addStock(ticker, qty);
    cout << "Added " << qty << " shares of " << ticker 
         << ". Total holding: " << user.getPortfolio()[ticker] << " shares." << endl;
}

// Handle sell stock
static void handleSellStock(User &user, const vector<string> &args) {
    string ticker;
    int qty = 0;

    if (args.size() >= 3) {
        ticker = args[1];
        try {
            qty = stoi(args[2]);
        } catch (...) {
            cout << "Invalid quantity: " << args[2] << endl;
            return;
        }
    } else {
        cout << "\n--- Remove Stock from Portfolio ---" << endl;
        ticker = promptString("Ticker symbol (e.g. AAPL)");
        qty = promptInt("Number of shares to sell/remove");
    }

    for (char &c : ticker) c = toupper(c);
    if (user.removeStock(ticker, qty)) {
        int remaining = user.getPortfolio().count(ticker) ? user.getPortfolio()[ticker] : 0;
        cout << "Removed " << qty << " shares of " << ticker 
             << ". Remaining holding: " << remaining << " shares." << endl;
    } else {
        cout << "Error: Unable to remove " << qty << " shares of " << ticker 
             << ". Check your current holdings." << endl;
    }
}

// Handle deterministic retirement planner
static void handleRetirementPlanner(const User &user, const vector<string> &args) {
    double input = 500.0;
    int inputTimeFrame = 30;
    int historicalYears = 5;
    int futureYears = 30;

    if (args.size() >= 5) {
        try {
            input = stod(args[1]);
            inputTimeFrame = stoi(args[2]);
            historicalYears = stoi(args[3]);
            futureYears = stoi(args[4]);
        } catch (...) {
            cout << "Invalid arguments. Usage: plan [input] [freq_days] [hist_years] [future_years]" << endl;
            return;
        }
    } else {
        cout << "\n================ RETIREMENT GROWTH PLANNER ================\n";
        input = promptDouble("Periodic contribution amount ($)", 500.0);
        inputTimeFrame = promptInt("Contribution frequency in days (30 = monthly, 365 = yearly)", 30);
        historicalYears = promptInt("Historical lookback years to estimate return", 5);
        futureYears = promptInt("Future projection years until retirement", 30);
    }

    if (inputTimeFrame <= 0) inputTimeFrame = 30;
    if (historicalYears <= 0) historicalYears = 5;
    if (futureYears <= 0) futureYears = 30;

    map<string, int> port = user.getPortfolio();
    double stockValue = calculatePortfolioValue(port);
    double retirementCash = user.getRetirement().getBalance();
    double startingBalance = stockValue + retirementCash;

    double cagr = calculateAverageYearlyReturn(port, historicalYears);
    if (isnan(cagr) || isinf(cagr)) {
        cout << "Note: Could not calculate historical CAGR from ticker data. Defaulting to 7.00% benchmark return.\n";
        cagr = 7.0;
    }

    double finalValue = calculateCompounding(
        startingBalance, input, cagr, futureYears * 365, inputTimeFrame, 365
    );

    double totalContributed = (inputTimeFrame > 0) ? (input * ((futureYears * 365) / inputTimeFrame)) : 0.0;
    double totalGrowth = finalValue - startingBalance - totalContributed;

    cout << fixed << setprecision(2);
    cout << "\n----------------- Projection Summary -----------------\n";
    cout << " Initial Starting Balance:   $" << setw(12) << startingBalance << "\n";
    cout << "   - Stock Portfolio:        $" << setw(12) << stockValue << "\n";
    cout << "   - Retirement Cash:        $" << setw(12) << retirementCash << "\n";
    cout << " Historical Estimated Return: " << setw(11) << cagr << "% per year\n";
    cout << " Periodic Contribution:      $" << setw(12) << input << " (every " << inputTimeFrame << " days)\n";
    cout << " Total Additional Principal: $" << setw(12) << totalContributed << "\n";
    cout << " Investment Growth (Gains):  $" << setw(12) << totalGrowth << "\n";
    cout << "------------------------------------------------------\n";
    cout << " PROJECTED RETIREMENT TOTAL: $" << setw(12) << finalValue << "\n";
    cout << "------------------------------------------------------\n" << endl;
}

// Handle Monte Carlo retirement planner
static void handleMonteCarlo(const User &user, const vector<string> &args) {
    double input = 500.0;
    int inputTimeFrame = 30;
    int historicalYears = 5;
    int futureYears = 30;
    double annualVolatility = 15.0;
    int numSimulations = 10000;
    double target = 1000000.0;

    if (args.size() >= 8) {
        try {
            input = stod(args[1]);
            inputTimeFrame = stoi(args[2]);
            historicalYears = stoi(args[3]);
            futureYears = stoi(args[4]);
            annualVolatility = stod(args[5]);
            numSimulations = stoi(args[6]);
            target = stod(args[7]);
        } catch (...) {
            cout << "Invalid arguments. Usage: montecarlo [input] [freq] [hist_yrs] [future_yrs] [vol%] [sims] [target]" << endl;
            return;
        }
    } else {
        cout << "\n============= MONTE CARLO RETIREMENT PREDICTIONS =============\n";
        input = promptDouble("Periodic contribution amount ($)", 500.0);
        inputTimeFrame = promptInt("Contribution frequency in days (30 = monthly, 365 = yearly)", 30);
        historicalYears = promptInt("Historical lookback years for expected return", 5);
        futureYears = promptInt("Future projection years", 30);
        annualVolatility = promptDouble("Annual volatility / standard deviation (%)", 15.0);
        numSimulations = promptInt("Number of Monte Carlo simulations", 10000);
        target = promptDouble("Target retirement goal ($)", 1000000.0);
    }

    if (inputTimeFrame <= 0) inputTimeFrame = 30;
    if (historicalYears <= 0) historicalYears = 5;
    if (futureYears <= 0) futureYears = 30;
    if (annualVolatility <= 0.0) annualVolatility = 15.0;
    if (numSimulations <= 0) numSimulations = 10000;

    double retirementCash = user.getRetirement().getBalance();
    cout << "\nRunning " << numSimulations << " stochastic simulations over " << futureYears << " years..." << endl;

    MonteCarloResult result = calculatePortfolioMonteCarlo(
        user.getPortfolio(),
        input,
        inputTimeFrame,
        historicalYears,
        futureYears,
        annualVolatility,
        numSimulations,
        target,
        retirementCash
    );

    result.print();
}

int main(int argc, char* argv[]) {
    cout << "\n============================================================\n";
    cout << "           PERSONAL FINANCE & RETIREMENT CLI               \n";
    cout << "============================================================\n";

    User user;
    string currentFilename = "";
    bool userLoaded = false;

    // Check if a JSON file was provided as a command-line argument
    if (argc > 1) {
        try {
            currentFilename = argv[1];
            user = User(currentFilename);
            userLoaded = true;
            cout << "Loaded user profile from '" << currentFilename << "'.\n";
        } catch (const exception &e) {
            cout << "Could not load argument file '" << argv[1] << "': " << e.what() << "\n";
        }
    }

    // Startup menu if no user was loaded via argument
    while (!userLoaded) {
        cout << "\nChoose an option:\n";
        cout << "  1. Load existing account from JSON\n";
        cout << "  2. Create a new user account\n";
        cout << "  3. Exit\n";
        int choice = promptInt("Enter choice [1-3]");

        if (choice == 1) {
            userLoaded = loadUserInteractive(user, currentFilename);
        } else if (choice == 2) {
            user = createNewUserInteractive(currentFilename);
            userLoaded = true;
        } else if (choice == 3) {
            cout << "Goodbye!\n" << endl;
            return 0;
        } else {
            cout << "Invalid choice. Please select 1, 2, or 3." << endl;
        }
    }

    displayStatus(user, currentFilename);
    cout << "Type 'help' for a full list of available commands.\n";

    // Main interactive CLI command loop
    string line;
    while (true) {
        cout << "[" << user.getName() << " @ finance]$ ";
        if (!getline(cin, line)) break;
        line = trim(line);
        if (line.empty()) continue;

        vector<string> tokens = tokenize(line);
        string cmd = toLower(tokens[0]);

        if (cmd == "exit" || cmd == "quit") {
            string ans = promptString("Save changes to JSON before exiting? (y/n)", "y");
            if (toLower(ans) == "y" || toLower(ans) == "yes") {
                string saveFile = currentFilename.empty() ? (toLower(user.getName()) + ".json") : currentFilename;
                user.save_to_json(saveFile);
            }
            cout << "Exiting Personal Finance CLI. Have a great day!" << endl;
            break;
        } else if (cmd == "help" || cmd == "?") {
            printHelp();
        } else if (cmd == "status" || cmd == "view" || cmd == "balance" || cmd == "info") {
            displayStatus(user, currentFilename);
        } else if (cmd == "transfer") {
            handleTransfer(user, tokens);
        } else if (cmd == "deposit") {
            handleDeposit(user, tokens);
        } else if (cmd == "withdraw") {
            handleWithdraw(user, tokens);
        } else if (cmd == "portfolio" || cmd == "stocks") {
            displayStatus(user, currentFilename);
        } else if (cmd == "buy") {
            handleBuyStock(user, tokens);
        } else if (cmd == "sell") {
            handleSellStock(user, tokens);
        } else if (cmd == "plan" || cmd == "retirement" || cmd == "retirementplanner") {
            handleRetirementPlanner(user, tokens);
        } else if (cmd == "montecarlo" || cmd == "mc") {
            handleMonteCarlo(user, tokens);
        } else if (cmd == "save") {
            string filename = (tokens.size() >= 2) ? tokens[1] : (currentFilename.empty() ? (toLower(user.getName()) + ".json") : currentFilename);
            user.save_to_json(filename);
            currentFilename = filename;
        } else if (cmd == "load") {
            if (tokens.size() >= 2) {
                try {
                    user = User(tokens[1]);
                    currentFilename = tokens[1];
                    cout << "Successfully loaded user '" << user.getName() << "' from " << tokens[1] << "!\n";
                    displayStatus(user, currentFilename);
                } catch (const exception &e) {
                    cout << "Error: " << e.what() << endl;
                }
            } else {
                if (loadUserInteractive(user, currentFilename)) {
                    displayStatus(user, currentFilename);
                }
            }
        } else if (cmd == "new") {
            user = createNewUserInteractive(currentFilename);
            displayStatus(user, currentFilename);
        } else if (cmd == "clear") {
            cout << "\033[2J\033[1;1H";
        } else {
            cout << "Unknown command '" << tokens[0] << "'. Type 'help' to see all available commands." << endl;
        }
    }

    return 0;
}
