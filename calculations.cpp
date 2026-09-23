#include <iostream>
#include <cmath>
#include "Account.hpp"

using namespace std;



int main(){
    SavingsAccount grantSavings = {100,5};
    cout << grantSavings.calculateInterest(5,365, 14, 30) << endl;
}