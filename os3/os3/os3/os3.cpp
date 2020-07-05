#include "pch.h"
#include <iostream>

using namespace std;

enum Type
{
  kFIFo = 0,
  kLRU,
  kARB,
  kSCP,
  kLFUPR,
  kMFUPR,
};

class Frame {
public:
  Frame() {
	page_ = 0;
	fault_ = false;
  }
  Frame(char page, vector<char> vPage,  bool fault) {
	vPage_ = vPage;
	page_ = page;
	fault_ = fault;
  }
  vector<char> vPage_;
  char page_;
  bool fault_;
};

class PagingRecord {
public:
  PagingRecord(int page_frames) {
	page_faults_ = 0;
	page_replaces_ = 0;
	page_frames_ = page_frames;

  }

  void push_back(Frame new_frame) {
	table_record_.push_back(new_frame);
	if (new_frame.fault_) 
	  page_faults_++;
	
	page_replaces_ = page_faults_ - page_frames_;
	if (page_replaces_ < 0)
	  page_replaces_ == 0;
  }

  void output(ofstream &outfile) {
	for (int i = 0; i <table_record_.size(); i++) {
	  outfile << table_record_[i].page_ << "\t";
	  for (int j = table_record_[i].vPage_.size()-1; j >= 0; j--) 
		outfile << table_record_[i].vPage_[j];
	  outfile << "\t";
	  if (table_record_[i].fault_)
		outfile << "F";

	  outfile << endl;

	}
	outfile << "Page Fault = " << page_faults_ << "  Page Replaces = " << page_replaces_ << "  Page Frames = " << page_frames_ << endl << endl;

  }

private:
  int page_faults_, page_replaces_, page_frames_;
  vector<Frame> table_record_;

};

class Paging {
public:
  Paging(int type, int page_frames) {
	type_ = type;
	page_frames_ = page_frames;
	time_ = 0;
  } // Paging

  void pageIn(char new_page) {
	if (type_ == kFIFo) 
	  fifo(new_page);
	else if (type_ == kLRU)
	  lru(new_page);
	else if (type_ == kARB)
	  arb(new_page);
	else if (type_ == kSCP)
	  scp(new_page);
	else if (type_ == kLFUPR)
	  lfu(new_page);
	else if (type_ == kMFUPR)
	  mfu(new_page);
  }
 

  Frame get_record() {
	return Frame(page_, vPage_, page_fault_mark_);
  }

private:
  int  search(char new_page) {
	for (int i = 0; i < vPage_.size(); i++) {
	  if(vPage_[i] == new_page)
		return i;
	}

	return -1;
  } 

  void fifo(char new_page) {
	page_ = new_page;
	page_fault_mark_ = false;
	if (search(new_page) == -1) {
	  page_fault_++;
	  page_fault_mark_ = true;
	  if (vPage_.size() == page_frames_) 
		pageOut(0);
		
	  vPage_.push_back(new_page);
	}
  }

  void lru(char new_page) {
	page_ = new_page;
	page_fault_mark_ = false;
	int index = search(new_page);
	if ( index != -1) 
	  pageOut(index);
	else {
	  page_fault_++;
	  page_fault_mark_ = true;
	  if (vPage_.size() == page_frames_) 
		pageOut(0);
	  
	}
	vPage_.push_back(new_page);
  }

  void arb(char new_page) {
	page_ = new_page;
	page_fault_mark_ = false;
	int index = search(new_page);
	if (index != -1) 
	  vReference_bit[index] = 1;
	else {
	  page_fault_++;
	  page_fault_mark_ = true;
	  if (vPage_.size() == page_frames_) { // page repalce
		int value = 0x8001, rm_index;
		for (index = 0; index < vRegister.size(); index++) {
		  if (vRegister[index] < value) {
			value = vRegister[index];
			rm_index = index;
		  } // if
		} // for
		pageOut(rm_index);

	  }
	
	  vPage_.push_back(new_page);
	  vReference_bit.push_back(1);
	  vRegister.push_back(0);
	} // else

	for (index = 0; index < vReference_bit.size(); index++) { // shift the register
	  vRegister[index] = vRegister[index] >> 1;
	  if (vReference_bit[index] == 1) {
		vReference_bit[index] = 0;
		vRegister[index] += 0x8000;
	  }
		
	} // for
	
  }

  void scp(char new_page) {
	page_ = new_page;
	page_fault_mark_ = false;
	int index = search(new_page);
	if ( index != -1) 
	  vReference_bit[index] = 1;
	else {
	  page_fault_++;
	  page_fault_mark_ = true;
	  while (vPage_.size() == page_frames_) {
		if (vReference_bit[0] == 0) {
		  pageOut(0);
		  break;
		}
		else {
		  vPage_.push_back(vPage_[0]);
		  vReference_bit.push_back(0);
		  pageOut(0);
		}
	  }
	  vPage_.push_back(new_page);
	  vReference_bit.push_back(1);
	}
	
  }

  void lfu(char new_page) {
	time_++;
	page_ = new_page;
	page_fault_mark_ = false;
	int index = search(new_page);
	if (index != -1) {
	  vReference_bit[index]++;
	  vTime_stamp[index] = time_;
	}
	else {
	  page_fault_++;
	  page_fault_mark_ = true;
	  if (vPage_.size() == page_frames_) { // page repalce
		int value = 999, rm_index = 0, time_stamp = 999;
		for (index = 0; index < vReference_bit.size(); index++) {
		  if (vReference_bit[index] < value || (vReference_bit[index] == value && vTime_stamp[index] < time_stamp) ) {
			value = vReference_bit[index];
			time_stamp = vTime_stamp[index];
			rm_index = index;
		  } // if
		} // for
		pageOut(rm_index);

	  }

	  vPage_.push_back(new_page);
	  vReference_bit.push_back(1);
	  vTime_stamp.push_back(time_);
	} // else
  }

  void mfu(char new_page) {
	time_++;
	page_ = new_page;
	page_fault_mark_ = false;
	int index = search(new_page);
	if (index != -1) {
	  vReference_bit[index]++;
	  vTime_stamp[index] = time_;
	}
	else {
	  page_fault_++;
	  page_fault_mark_ = true;
	  if (vPage_.size() == page_frames_) { // page repalce
		int value = -1, rm_index = 0, time_stamp = 999;
		for (index = 0; index < vReference_bit.size(); index++) {
		  if (vReference_bit[index] > value || (vReference_bit[index] == value && vTime_stamp[index] < time_stamp)) {
			value = vReference_bit[index];
			time_stamp = vTime_stamp[index];
			rm_index = index;
		  } // if
		} // for
		pageOut(rm_index);

	  }

	  vPage_.push_back(new_page);
	  vReference_bit.push_back(1);
	  vTime_stamp.push_back(time_);
	} // else
  }

  void pageOut(int index) {
	vPage_.erase(vPage_.begin() + index);
	if (vReference_bit.size() != 0) 
	  vReference_bit.erase(vReference_bit.begin() + index);
	if (vRegister.size() != 0) 
	  vRegister.erase(vRegister.begin() + index);
	if (vTime_stamp.size() != 0)
	  vTime_stamp.erase(vTime_stamp.begin() + index);
  }

  char page_;
  int type_, page_fault_, page_frames_, time_;
  bool page_fault_mark_;
  vector<char> vPage_;
  vector<int> vReference_bit;
  vector<int> vRegister; 
  vector<int> vTime_stamp;

};

bool inputTxt(string file_name, int &page_frame, vector<char> &vPage) {
  ifstream infile;
  infile.open(file_name);
  char page;
  if (infile) {
	infile >> page_frame;
	//infile >> page; // for \n
	infile >> page;
	while (!infile.eof()) {
	  vPage.push_back(page);
	  infile >> page;
	} // while

	return true;
  } // if
  else 
	return false;
}

void outputTxt(string file_name, PagingRecord fifo, PagingRecord lru, PagingRecord arb, PagingRecord scp, PagingRecord lfupr, PagingRecord mfupr) {
  ofstream outfile;
  string str = "output_" + file_name;
  outfile.open(str);
  outfile << "--------------FIFO-----------------------" << endl;
  fifo.output(outfile);
  outfile << "--------------LRU--------------------------" << endl;
  lru.output(outfile);
  outfile << "--------------Additional Reference Bits-------------------" << endl;
  arb.output(outfile);
  outfile << "--------------Second chance Page-------------------------" << endl;
  scp.output(outfile);
  outfile << "--------------Least Frequently Used Page Replacement --------------------" << endl;
  lfupr.output(outfile);
  outfile << "--------------Most Frequently Used Page Replacement ------------------------" << endl;
  mfupr.output(outfile);
  outfile.close();
}

void paging(vector<char> vPage, int page_frame, string file_name) {
  PagingRecord fifo_record(page_frame), lru_record(page_frame), arb_record(page_frame), scp_record(page_frame), lfupr_record(page_frame), mfupr_record(page_frame);
  Paging fifo(kFIFo, page_frame), lru(kLRU, page_frame), arb(kARB, page_frame), scp(kSCP, page_frame), lfu(kLFUPR, page_frame), mfu(kMFUPR, page_frame);
  char new_page;
  for (int i = 0; i < vPage.size(); i++) {
	new_page = vPage[i];
	fifo.pageIn(new_page);
	fifo_record.push_back(fifo.get_record());
	lru.pageIn(new_page);
	lru_record.push_back(lru.get_record());
	arb.pageIn(new_page);
	arb_record.push_back(arb.get_record());
	scp.pageIn(new_page);
	scp_record.push_back(scp.get_record());
	lfu.pageIn(new_page);
	lfupr_record.push_back(lfu.get_record());
	mfu.pageIn(new_page);
	mfupr_record.push_back(mfu.get_record());
  }

  outputTxt(file_name, fifo_record, lru_record, arb_record, scp_record, lfupr_record, mfupr_record);
}

int main()
{
  string file_name;
  int page_frame;
  vector<char> vPage;
  do {
	cout << "input the file name:\n>";
	cin >> file_name;
	if (inputTxt(file_name, page_frame, vPage)) {
	  paging(vPage, page_frame, file_name);
	}
	else 
	  cout << "Can't found " << file_name << "!!!" << endl;

	vPage.clear();
	cout << "input \"quit\" to quit:\n>"; 
	cin >> file_name;
  } while (file_name != "quit");
}
