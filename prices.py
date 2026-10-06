import sys, json, yfinance as yf

symbols, years = sys.argv[1:-1], int(sys.argv[-1])
period = "1d" if years == 0 else f"{years}y"

prices = {}
for symbol in symbols:
    try:
        history = yf.Ticker(symbol).history(period=period).dropna(subset=["Close"])
        if history.empty:
            raise ValueError("no price data")
        prices[symbol] = float(history["Close"].iloc[0])
    except Exception as e:
        prices[symbol] = str(e)

print(json.dumps(prices))
