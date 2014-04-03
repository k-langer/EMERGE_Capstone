/**/
#include <iostream>
#include <string>

using namespace std;


class Helper{
public:
    static string formatString(string amount, int strlen);
};

string Helper::formatString(string amount, int strlen){
    while(amount.length() < strlen){
        amount = "0" + amount;
    }
    return amount;
}
