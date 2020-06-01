#include <iostream>
#include "SimplestTimer.h"

using namespace std;

int main() {
    Timer t = Timer();

    t.setTimeInterval([&]() {
        cout << "Hey.. After each 1s..." << endl;
    }, 1000); 

    t.setTimeout([&]() {
        cout << "Hey.. After 5.2s. But I will stop the timer!" << endl;
        t.Stop();
    }, 5200); 
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}