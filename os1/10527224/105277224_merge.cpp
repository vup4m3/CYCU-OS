#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>

using namespace std;

void generate( char *argv[], int &first, int &second) {
  first = stoi(argv[1]);
  second = stoi(argv[2]);
}

vector<int> merge(vector<int> left_array, vector<int> right_array) {
  int MAX = 2147483647;
  int array_max = left_array.size() + right_array.size();
  left_array.push_back(MAX);
  right_array.push_back(MAX);
  vector<int> array;
  int idxLeft = 0, idxRight = 0;
  for (int i = 0; i < array_max; i++) {
    if (left_array[idxLeft] <= right_array[idxRight]) {
        array.push_back(left_array[idxLeft]);
        idxLeft++;
    }
    else {
        array.push_back(right_array[idxRight]);
        idxRight++;
    }

  }
  left_array.clear();
  right_array.clear();

  return array;
}

vector<int> inputData(string file_name) {
  ifstream infile;
  infile.open(file_name);
  vector<int> num_array;
  int num;
  while (infile >> num)
    num_array.push_back(num);
  
  infile.close();
  remove(file_name.c_str());
  
  return num_array;
}

void outPutData(vector<int> num_array, int front) {
  ofstream ofile;
  string flag = to_string(front) + ".txt";
  ofile.open(flag);
  for (int i = 0; i < num_array.size(); i++) {
    ofile << num_array[i] << " ";
  }
  ofile.close();
}

int main(int argc, char *argv[]) {
  // argv should be "fileName.txt front end"
  int first, second;
  vector<int> data, left_array, right_array;
  generate(argv, first, second);
  string str = to_string(first) + ".txt";
  left_array = inputData(str);
  str = to_string(second) + ".txt";
  right_array = inputData(str);
  data = merge(left_array, right_array);
  outPutData(data, first);

  return 0;
}