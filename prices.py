import sys, json, yfinance as yf

symbol, years = sys.argv[1], int(sys.argv[2])
period = "1d" if years == 0 else f"{years}y"
history = yf.Ticker(symbol).history(period=period)

print(json.dumps({
    "price": float(history["Close"].dropna().iloc[0]),
    "date": str(history.index[0].date()),
}))
