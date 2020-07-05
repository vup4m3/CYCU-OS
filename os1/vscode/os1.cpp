#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<thread>
#include <time.h>
#include <atomic>

using namespace std;

void bubble_sort(vector<int> &num_array, int front, int end) {
  int i, j;
  for (i = end - front; i > 0; i--)
	for (j = front; j - front <= i - 1; j++)
	  if (num_array[j] > num_array[j + 1])
		swap(num_array[j], num_array[j + 1]);

}

void merge(vector<int> &Array, int front, int mid, int end) {
  vector<int> left_array(Array.begin() + front, Array.begin() + mid + 1), right_array(Array.begin() + mid + 1, Array.begin() + end + 1);
  int MAX = 2147483647;
  left_array.push_back(MAX);
  right_array.push_back(MAX);
  int idxLeft = 0, idxRight = 0;
  for (int i = front; i <= end; i++) {
	if (left_array[idxLeft] <= right_array[idxRight]) {
	  Array[i] = left_array[idxLeft];
	  idxLeft++;
	}
	else {
	  Array[i] = right_array[idxRight];
	  idxRight++;
	}

  }
  left_array.clear();
  right_array.clear();
}

void mergeSort(vector<int> &array, int k, int piece) {
  int front = 0, mid = piece - 1, end, i, x = k;
  vector<thread> vThreads;
  k %= 2;
  while (x > 1) {
	if (x == 2) {
	  end = array.size() - 1;
	}
	else
	  end = mid + piece;
	vThreads.push_back(std::thread(merge, ref(array), front, mid, end));
	front = end + 1;
	mid = front + piece - 1;
	x -= 2;
	k++;
  }

  for (auto it = vThreads.begin(); it != vThreads.end(); ++it)
	it->join();

  if (k > 1)
	mergeSort(array, k, piece * 2);

}

void multiThread(vector<int> &num_array, int k) {
  int piece = num_array.size() / k;
  int front = 0, end = -1, i;
  vector<thread> vThreads;
  for (i = k; i > 0; i--) {
	if (i == 1)
	  end = num_array.size() - 1;
	else
	  end = front + piece - 1;

	vThreads.push_back(std::thread(bubble_sort, ref(num_array), front, end));
	front = end + 1;
  }

  for (auto it = vThreads.begin(); it != vThreads.end(); ++it)
	it->join();

}

void outPutToTXT(vector<int> array, string file_name, float cost_time) {
  ofstream fout;
  fout.open(file_name);
  fout << "Cost Time:" << cost_time << "s" << endl;
  for (int i = 0; i < array.size(); i++)
	fout << array[i] << " ";

  fout.close();
}

int main() {
  ifstream fin;
  string file_name;
  int num, k, mode;
  vector<int> num_array; 
  float costTime = 0;
  do {
	cout << "input file:\n>";
	cin >> file_name;
	cout << "How many cut\n>";
	cin >> k;
	fin.open(file_name);
	if (fin.is_open()) {
	  fin >> mode;
	  while (fin >> num)
		num_array.push_back(num);

	  fin.close();

	  file_name = "output_" + file_name;
	  clock_t start, end;
	  if (mode == 1) {
		start = clock();
		bubble_sort(ref(num_array), 0, num_array.size() - 1);
		end = clock();
		costTime = (float)(end - start) / CLK_TCK;
	  }
	  else if (mode == 2) {
		start = clock();
		multiThread(num_array, k);
		mergeSort(num_array, k, num_array.size() / k);
		end = clock();
		costTime = (float)(end - start) / CLK_TCK;
	  }

	  outPutToTXT(num_array, file_name, costTime);

	}
	else
	  cout << "Error opening " << file_name << endl;

	cout << "input \"quit\" to quit" << endl << ">";
  } while (file_name != "quit");

  system("pause");
}