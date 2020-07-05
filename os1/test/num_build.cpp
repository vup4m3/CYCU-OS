#include <iostream>
#include <fstream>
#include <random>

using namespace std;

int main() {
    int number;
    ofstream outFile;
    cout << "Input Number>";
    cin >> number;
    outFile.open("number.txt");
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_int_distribution<int> dis(1,number);
    for (int i = 1; i <= number; i++) {
        outFile << dis(gen) << " ";
    }
    outFile.close();
}