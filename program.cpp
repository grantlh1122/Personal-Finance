#include "User.hpp"
#include "Account.hpp"
#define CALCULATIONS_NO_MAIN
#include "calculations.cpp"

using namespace std;

int main(){
    string userChoice;

    cout << "Type 1 to import an account via json, and 2 to create a new account." << endl;
    cin >> userChoice;
    while (stoi(userChoice) != 1 && stoi(userChoice) != 2){
        cout << "Invalid choice. Type 1 to import an account via json, and 2 to create a new account." << endl;
        cin >> userChoice; 
    }

    User user;
    if(stoi(userChoice) == 1){
        bool loaded = false;
        while (!loaded){
            cout << "Enter filename: " << endl;
            string filename;
            cin >> filename;
            try {
                user = User(filename);
                loaded = true;
            } catch (const exception &e){
                cout << e.what() << " Try again." << endl;
            }
        }
    }
}
