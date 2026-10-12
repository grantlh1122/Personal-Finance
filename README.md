# Personal Finance & Retirement Planning CLI

A personal finance and retirement projection system written in C++17 with real-time stock valuation powered by Python and the `yfinance` library. 

The application offers an interactive CLI for managing multiple bank accounts, tracking investment portfolios, transferring funds with overdraft safeguards, and forecasting long-term wealth growth using both deterministic compounding and stochastic **Monte Carlo** simulations.

---

## Features

### Account & Cash Management
- **Account Tracking**: Independent balances and interest rates for **Checking**, **Savings**, and **Retirement Cash** accounts.
- **Safe Transfers & Transactions**: Move funds between any account with automatic balance validation to prevent overdrafts.
- **Persistent Profiles**: Save and load complete user profiles to and from clean JSON schemas.

### Investment Portfolio & Live Ticker Data
- **Stock Holdings**: Track ticker symbols and share quantities.
- **Live Market Pricing**: Integrates with Yahoo Finance via Python (`prices.py`) to query real-time prices and historical valuations.
- **Dynamic Net Worth**: Combines cash balances and live stock valuations into a single estimated net worth overview.

### Advanced Retirement Forecasting
- **Basic Growth Planner (`plan`)**:
  - Computes historical Compound Annual Growth Rate (CAGR) from your portfolio's performance.
  - Projects future compounding based on custom periodic contribution amounts, contribution frequencies, and investment horizons.
- **Monte Carlo Simulator (`montecarlo` / `mc`)**:
  - Simulates thousands of randomized market paths (e.g., 10,000 runs) modeling annual volatility and market returns.
  - Outputs statistical distributions and calculates probability of meeting a specific retirement goal (e.g. $1,000,000 target).

### 🛠️ Developer & Testing Tooling
- **AddressSanitizer (ASan) & UBSan**: Built-in sanitizer configurations to ensure memory safety, zero leaks, and zero undefined behavior.
- **GoogleTest Suite**: 19 unit tests covering account operations, JSON serialization, financial calculations, and Monte Carlo simulations.
- **Makefile**: Pre-configured build and test automation.

---

## Project Structure

```
personal-finance/
├── Account.hpp / Account.cpp       # Account class hierarchy (Checking, Savings, Retirement)
├── User.hpp / User.cpp             # User profile model, JSON persistence, and transactions
├── calculations.cpp                # Financial math, CAGR, compounding, and Monte Carlo models
├── program.cpp                     # Interactive Command-Line Interface (CLI) entrypoint
├── prices.py                       # Python bridge fetching yfinance ticker prices
├── json.hpp                        # nlohmann/json single-header library
├── Makefile                        # Build and test automation
├── tests/
│   └── test_finance.cpp            # GoogleTest / ASan unit test suite
├── grant.json                      # Sample user data profile
└── README.md                       # Project documentation
```

---

## Prerequisites

- **C++ Compiler**: `g++` or `clang++` with C++17 support
- **Build Tool**: `make`
- **Python Environment**: Python 3.8+ with virtualenv:
  ```bash
  python3 -m venv .venv
  ./.venv/bin/pip install yfinance
  ```
- *(Optional)* **GoogleTest**: For native GoogleTest linking:
  ```bash
  sudo apt install -y libgtest-dev
  ```

---

## Building & Running

### Compile the Application
```bash
make build
```
This produces the executable `program.exe`.

### Launch the CLI
You can launch the program interactively:
```bash
./program.exe
```

Or directly load a user profile by passing the JSON file as an argument:
```bash
./program.exe grant.json
```

---

## CLI Usage & Commands

When running the application, you can execute commands with inline arguments or simply type the command name to receive guided interactive prompts with default values.

### Command Reference Table

| Command | Arguments | Description |
|---|---|---|
| `help` | None | Displays the interactive command guide and parameter definitions. |
| `status` / `view` | None | Displays financial dashboard: balances, APYs, stocks, and net worth. |
| `transfer` | `[from] [to] [amount]` | Transfers money between accounts (`checking`, `savings`, `retirement`). |
| `deposit` | `[account] [amount]` | Deposits cash into a specified account. |
| `withdraw` | `[account] [amount]` | Withdraws cash from a specified account. |
| `portfolio` | None | Displays stock holdings, current share prices, and position values. |
| `buy` | `[ticker] [quantity]` | Adds shares of a stock to the portfolio. |
| `sell` | `[ticker] [quantity]` | Removes shares of a stock from the portfolio. |
| `plan` | `[input] [freq] [hist_yrs] [future_yrs]` | Deterministic retirement calculator using historical CAGR. |
| `montecarlo` / `mc` | `[input] [freq] [hist] [future] [vol%] [sims] [target]` | Stochastic Monte Carlo simulation predicting retirement outcomes. |
| `save` | `[filename]` | Saves current profile state to a JSON file. |
| `load` | `[filename]` | Loads a different profile from a JSON file. |
| `new` | None | Launches the interactive new user creation wizard. |
| `clear` | None | Clears the terminal screen. |
| `exit` / `quit` | None | Exits the application (prompts to save pending changes). |

---

## Retirement Forecasting Examples

### 1. Deterministic Planning (`plan`)
Projects growth using historical portfolio Compound Annual Growth Rate (CAGR):
```bash
[Grant @ finance]$ plan 500 30 5 30
```
- Parameters: `$500` contribution every `30` days, using `5` years of historical data over a `30`-year future.

### 2. Monte Carlo Simulation (`montecarlo` / `mc`)
Runs thousands of stochastic simulation runs accounting for market volatility:
```bash
[Grant @ finance]$ montecarlo 500 30 5 30 15 10000 1000000
```
- Parameters: `$500`  input every `30` days using `5` years historical return lookback, `30` future years, `15.0%` annual volatility, `10,000` simulation runs, targeting a `$1,000,000` retirement goal.

**Sample Output:**
```
================ Monte Carlo Simulation Results ================
Simulations:             10000
Mean Ending Balance:     $876,432.18
Standard Deviation:      $382,109.45
10th Percentile (P10):   $442,105.12 (Conservative / Bear Market)
25th Percentile (P25):   $589,742.60
50th Percentile (Median):$801,234.90
75th Percentile (P75):   $1,084,520.15
90th Percentile (P90):   $1,421,809.84 (Optimistic / Bull Market)
Worst Case (Min):        $184,320.10
Best Case (Max):         $3,190,442.20
Success Rate (>= $1000000.00): 32.40%
================================================================
```

---

## Testing & Quality Assurance

The project includes unit tests covering accounts, user operations, JSON serialization, financial compounding, and Monte Carlo distributions with **AddressSanitizer (ASan)** and **UndefinedBehaviorSanitizer (UBSan)**.

To compile and run the test suite:
```bash
make test
```

### Clean Build Artifacts
```bash
make clean
```

---

## JSON Profile Format

Profiles are stored in standard JSON matching the following structure:

```json
{
    "name": "Grant",
    "checking": [
        1000.0,
        0.5
    ],
    "savings": [
        2500.0,
        4.0
    ],
    "retirement": [
        5000.0,
        0.0
    ],
    "portfolio": {
        "AAPL": 10,
        "GOOG": 5
    }
}
```
- Account entries represent `[balance, interestRateAPY]`.
- Portfolio maps ticker symbols to integer share quantities.
