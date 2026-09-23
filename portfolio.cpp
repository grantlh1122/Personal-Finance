//going to need libcurl for markets, and a json parser
//the idea is to let them choose a historical date. get the data based on their current weigted average 
//of their portfolio by dollar amount. then get the unit return (yearly) and apply it to future values
//also allow an annual contribution. The price will increase every year based on the return rate.
//
//ex: user has $1000 in stock A and $2000 in stock B. Get the historical price of stock A and B on the date they choose. 
//stock a weight is .33, b is .66. get the return rate for each stock over the last x years.
//multiply the return rate by the weight and add them together to get a weighted average return rate.
//then finally get the yearly input and increase the cost of it each year by the return rate. loop through the years and 
//calculate the total value of the portfolio at the end of the time frame. 
//just do it based on assuming cash is going into the exact stocks they 
//have at the same weight, so just increase the dollar amount of the stocks in the alg and do the daily 
//calculations. allowed to do for any number of YEARS.
//this function is simpler because you dont have to worry about as much compounding
//instead just calculate how much money you will have put in


//This function will contain the portfolio class which holds a hashmap?? of stock tickers and quantities. 
//You should be able to call a function and get the dollar value of each stock.