#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>

using namespace std;

void generate( char *argv[], string &file_name, int &front, int &end) {
  file_name = argv[1];
  front = stoi(argv[2]);
  end = atoi(argv[3]);
}

void bubble_sort(vector<int> &num_array, int front, int end) {
  int i, j;
  for (i = end - front; i > 0; i--)
		for (j = front; j - front <= i - 1; j++)
	  	if (num_array[j] > num_array[j + 1])
				swap(num_array[j], num_array[j + 1]);

}

vector<int> inputData(string file_name) {
  ifstream infile;
  infile.open(file_name);
  vector<int> num_array;
  int num;
  infile >> num;
  while (infile >> num)
    num_array.push_back(num);
  
  infile.close();
  
  return num_array;
}

void outPutData(vector<int> num_array, int front, int end) {
  ofstream ofile;
  string flag = to_string(front) + ".txt";
  ofile.open(flag);
  for (int i = front; i <= end; i++) {
    ofile << num_array[i] << " ";
  }
  ofile.close();
}

int main(int argc, char *argv[]) {
  // argv should be "fileName.txt front end"
  string file_name;
  int front, end;
  vector<int> data;
  generate(argv, file_name, front, end);
  data = inputData(file_name);
  bubble_sort(data, front, end);
  outPutData(data, front, end);
  return 0;
}