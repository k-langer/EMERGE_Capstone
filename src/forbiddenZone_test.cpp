/**/
#include <iostream>
#include <string>
#include "forbiddenZone.h"
#include "cartesian.h"

using namespace std;

int main(){
    ForbiddenZone zone;
    if(zone.contains(Cartesian(1,1,1))){
            cout << "true" << endl;
    }else{
        cout << "false" << endl;
    }
}