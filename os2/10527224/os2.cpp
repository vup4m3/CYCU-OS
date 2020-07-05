#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

enum ConditionType
{
  kArrivalThanId = 0,
  kArrivalThanPriority,
  kArrivalThanRemainTime,
  kRemainTimeThanUsedThanArrivalThanID,
  kProrityThanUsedThanArrivalThanID,
  kSjf,
  kPp
};

class Data {
public:
  int d_id, d_cpu_burst, d_arrival_time, d_priority, d_remain_burst;

  Data() {
	d_id = -1;
	d_cpu_burst = -1;
	d_arrival_time = -999;
	d_priority = -1;
	d_remain_burst = d_cpu_burst;
  } // Data()

  Data(int id, int cpu_burst, int arrival_time, int priority) {
	d_id = id;
	d_cpu_burst = cpu_burst;
	d_arrival_time = arrival_time;
	d_priority = priority;
	d_remain_burst = d_cpu_burst;
  } // Data()
};

class TimeRecordObj {
public:
  int tId, tWaiting_time, tTurnaround_time;
  TimeRecordObj(int id, int waiting_time, int  turnaround_time) {
	tId = id;
	tWaiting_time = waiting_time;
	tTurnaround_time = turnaround_time;
  }

  TimeRecordObj() {
	tId = -1;
	tWaiting_time = -1;
	tTurnaround_time = -1;
  }
};

class TimeRecord {
public:
  TimeRecord() {
	time_record_.clear();
  }

  void clear() {
	time_record_.clear();
	gant_record_.clear();
  }

  bool isEmpty() {
	return time_record_.size() == 0;
  }

  int size() {
	return time_record_.size();
  }

  int gant_record_size() {
	return gant_record_.size();
  }

  int get_gant_record(int i) {
	return gant_record_[i];
  }

  int get_id(int i) {
	return time_record_[i].tId;
  }

  int get_waiting_time(int i) {
	return time_record_[i].tWaiting_time;
  }

  int get_turnarround_time(int i) {
	return time_record_[i].tTurnaround_time;
  }

  void insert(TimeRecordObj new_obj) {
	if (time_record_.size() == 0 || time_record_[time_record_.size()-1].tId < new_obj.tId) 
	  time_record_.push_back(new_obj);
	else {
	  for (int i = 0; i < time_record_.size(); i++) {
		if (new_obj.tId < time_record_[i].tId) {
		  time_record_.insert(time_record_.begin() + i, new_obj);
		  break;
		} // if

	  } // for
	} // else

  } // insert()

  void packageGantRecord(vector<int> gant_record) {
	gant_record_ = gant_record;
  } // packageGantRecord()

private:
  vector<TimeRecordObj> time_record_;
  vector<int> gant_record_;
  
};

class Queue {
public:
  Queue(vector<Data> vData, int type) {
	queue_ = vData;
	quikSort(0, queue_.size()-1, type);
	arrival_time_ = queue_[0].d_arrival_time;
	burst_remain_ = queue_[0].d_remain_burst;
	priority_ = queue_[0].d_priority;

  } // ArrivalQueue()

  Queue() {
	arrival_time_ = -1;
	burst_remain_ = 999;
  } // Queue()

  int next_arrival_time() {
	return arrival_time_;
  } // next_arrival_time()

  int next_burst_remain() {
	return burst_remain_;
  } // next_burst_remain()

  int next_priority() {
	return priority_;
  }

  Data pop_front() {
	Data temp = queue_[0];
	queue_.erase(queue_.begin());
	if (!queue_.empty())  {
	  arrival_time_ = queue_[0].d_arrival_time;
	  burst_remain_ = queue_[0].d_remain_burst;
	  priority_ = queue_[0].d_priority;
	}
	else {
	  arrival_time_ = -1;
	  burst_remain_ = 999;
	  priority_ = 999;
	} 

	return temp;
  } // pop_front()

  bool empty() {
	return queue_.size() == 0;
  } // empty()

  void push_back(Data data) {
	queue_.push_back(data);
  } // push_back()

  void insert(int type, Data newData) {
	queue_.push_back(newData);
	if (queue_.size() >= 1) {
	  for (int i = 0; i < queue_.size(); i++) {
		if (condition(queue_.size()-1, i, type)) {
		  queue_.insert(queue_.begin() + i, queue_[queue_.size()-1]);
		  queue_.pop_back();
		  break;
		} // if

	  } // for

	} // if
	arrival_time_ = queue_[0].d_arrival_time;
	burst_remain_ = queue_[0].d_remain_burst;
	priority_ = queue_[0].d_priority;
  } // insert()

  void quikSort(int front, int end, int type) {
	if (front < end) {
	  int pivot = partition(front, end, type);
	  quikSort(front, pivot-1, type);
	  quikSort(pivot+1, end, type);
	} // if

  } // quikSort

  int size() {
	return queue_.size();
  } // size()

private:
  vector<Data> queue_;
  int arrival_time_, burst_remain_, priority_;

  int partition(int front, int end, int type) {
	int i = front - 1;
	for (int j = front; j < end; j++) {
	  if (condition(j, end, type)) {
		i++;
		swap(queue_[i], queue_[j]);
	  } // if

	} // for 
	i++;
	swap(queue_[i], queue_[end]);
	return i;
  } // partition()

  bool condition(int first, int second, int type) {
	if (type == kArrivalThanId) {
	  if (queue_[first].d_arrival_time < queue_[second].d_arrival_time || (queue_[first].d_arrival_time == queue_[second].d_arrival_time && queue_[first].d_id < queue_[second].d_id) )
		return true;
	  else return false;

	} // if
	else if (type == kArrivalThanPriority) {
	  if (queue_[first].d_arrival_time < queue_[second].d_arrival_time || (queue_[first].d_arrival_time == queue_[second].d_arrival_time && queue_[first].d_priority < queue_[second].d_priority) )
		return true;
	  else return false;

	} // else
	else if (type == kArrivalThanRemainTime) {
	  if (queue_[first].d_arrival_time < queue_[second].d_arrival_time || (queue_[first].d_arrival_time == queue_[second].d_arrival_time && queue_[first].d_remain_burst < queue_[second].d_remain_burst) )
		return true;
	  else return false;

	} // else
	else if (type == kRemainTimeThanUsedThanArrivalThanID) {
	  if (queue_[first].d_remain_burst < queue_[second].d_remain_burst) 
		return true;
	  else if (queue_[first].d_remain_burst == queue_[second].d_remain_burst) {
		if (queue_[first].d_remain_burst == queue_[first].d_cpu_burst && queue_[second].d_remain_burst <  queue_[second].d_cpu_burst)
		  return true;
		else if (queue_[first].d_remain_burst < queue_[first].d_cpu_burst && queue_[second].d_remain_burst ==  queue_[second].d_cpu_burst) 
		  return false;
		else if (queue_[first].d_arrival_time < queue_[second].d_arrival_time)
		  return true;
		else if (queue_[first].d_arrival_time == queue_[second].d_arrival_time && queue_[first].d_id < queue_[second].d_id )
		  return true;
		else return false;

	  } 
	  else return false;

	} // else
	else if (type == kProrityThanUsedThanArrivalThanID) {
	  if (queue_[first].d_priority < queue_[second].d_priority) 
		return true;
	  else if (queue_[first].d_priority == queue_[second].d_priority) {
		if (queue_[first].d_remain_burst == queue_[first].d_cpu_burst && queue_[second].d_remain_burst <  queue_[second].d_cpu_burst)
		  return true;
		else if (queue_[first].d_remain_burst < queue_[first].d_cpu_burst && queue_[second].d_remain_burst ==  queue_[second].d_cpu_burst) 
		  return false;
		else if (queue_[first].d_arrival_time < queue_[second].d_arrival_time)
		  return true;
		else if (queue_[first].d_arrival_time == queue_[second].d_arrival_time && queue_[first].d_id < queue_[second].d_id )
		  return true;
		else return false;

	  }
	  else return false;

	}
	else return false;

  } // condition()

};

class CPU {
public:
  bool isProcessing, isBlocking;
  CPU(int time_slice) {
	time_slice_ = time_slice;
	isProcessing = false;
	isBlocking = false;
  } // 

  void dispatch(Data process) {
	currrent_process_ = process;
	isBlocking = true;
	isProcessing = true;
	if (currrent_process_.d_remain_burst < time_slice_) {
	  process_slice_ = currrent_process_.d_remain_burst;
	} // if 
	else {
	  process_slice_ = time_slice_;
	} // else

  } // process_in

  void process() {
	if (isBlocking) {
	  process_slice_--;
	  currrent_process_.d_remain_burst--;
	  if (process_slice_ == 0)
		isProcessing = false;

	  gant_record_.push_back(currrent_process_.d_id);
	} // if
	
  } // proccess()

  Data get_process() {
	isBlocking = false;
	return currrent_process_; 
  } // time_out()

  vector<int> get_gant() {
	return gant_record_;
  }

  int get_burst_remain() {
	return currrent_process_.d_remain_burst;
  }
  
  bool preemptive(int type, int comparator) { // type == kSjf, comparator should be other remain_burst
	if (isBlocking) {
	  if (type == kSjf) {
		if (currrent_process_.d_remain_burst > comparator)
		  return true;
		else return false;

	  } // if
	  else if (type == kPp) {
		if (currrent_process_.d_priority > comparator)
		  return true;
		else return false;
	  }
	}
	else return false;
	
  } // preemptive

private:
  int time_slice_, finish_time, process_slice_;
  Data currrent_process_;
  vector<int> gant_record_;
};

char to_char(int integer) {
  char ch = 0;
  if (0 <= integer && integer <= 9) 
	ch = integer + '0';
  else {
	ch = integer + 55;
  }

  return ch; 
} // to_char()

bool input(vector<Data> &vData, string file_name, int &method, int &slice) {
  ifstream infile;
  string skip;
  int i, id, cpu_burst, arrival_time, priority;
  infile.open(file_name);
  if (infile) {
	infile >> method ;
	infile >> slice;
	for (i = 0; i < 6; i++) 
	  infile >> skip;

	infile >> id;
	while (!infile.eof()) {
	  infile >> cpu_burst;
	  infile >> arrival_time;
	  infile >> priority;
	  vData.push_back(Data(id, cpu_burst, arrival_time, priority));
	  infile >> id;
	} // while

	return true;
  } // if
  else 
	return false;

} // input()

void gant(ofstream &outfile, TimeRecord time_record) {
  outfile << "-";
  for (int i = 0; i < time_record.gant_record_size(); i++) 
	outfile << to_char(time_record.get_gant_record(i));
  outfile << endl;
}

void gantOutput(int method, ofstream &outfile, TimeRecord fcfs, TimeRecord rr, TimeRecord psjf, TimeRecord nsjf, TimeRecord pp) {
  if (method == 1|| method == 6) {
	outfile << "==    FCFS==" << endl;
	gant(outfile, fcfs);
  } 
  if (method == 2|| method == 6) {
	outfile << "==      RR==" << endl;
	gant(outfile, rr);
  }

  if (method == 3|| method == 6) {
	outfile << "==    PSJF==" << endl;
	gant(outfile, psjf);
  }
  if (method == 4|| method == 6) {
	outfile << "==Non-PSJF==" << endl;
	gant(outfile, nsjf);
  }
  if (method == 5 || method == 6) {
	outfile << "== Priority==" << endl;
	gant(outfile, pp);
  }
  
  outfile << "===========================================================" << endl << endl;
} // gantOutput()

void outputWaitingTime(int method, ofstream &outfile, TimeRecord fcfs, TimeRecord rr, TimeRecord psjf, TimeRecord nsjf, TimeRecord pp) {
  int i = 0;
  bool id_unprint = true;
  outfile << "Waiting Time" << endl;
  outfile << "ID\tFCFS\tRR\tPSJF\tNPSJF\tPriority" << endl;
  outfile << "===========================================================" << endl;
  for (i = 0; i < fcfs.size() || i < rr.size() ||  i < psjf.size() ||  i < nsjf.size() ||  i < pp.size(); i++) {
	if (method == 1 || method == 6) {
	  if (id_unprint) {
		outfile << fcfs.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << fcfs.get_waiting_time(i) << "\t";

	}
	if (method == 2 || method == 6) {
	  if (id_unprint) {
		outfile << rr.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << rr.get_waiting_time(i) << "\t";

	}
	if (method == 3 || method == 6) {
	  if (id_unprint) {
		outfile << psjf.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << psjf.get_waiting_time(i) << "\t";

	}
	if (method == 4 || method == 6) {
	  if (id_unprint) {
		outfile << nsjf.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << nsjf.get_waiting_time(i) << "\t";

	}
	if (method == 5 || method == 6) {
	  if (id_unprint) {
		outfile << pp.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << pp.get_waiting_time(i) << "\t";

	}
	id_unprint = true;
	outfile << endl;
  } // for

  outfile << "===========================================================" << endl;
} // outputWaitingTime() 

void outputTurnarroundTime(int method, ofstream &outfile, TimeRecord fcfs, TimeRecord rr, TimeRecord psjf, TimeRecord nsjf, TimeRecord pp) {
  int i = 0;
  bool id_unprint = true;
  outfile << "Turnaround Time" << endl;
  outfile << "ID\tFCFS\tRR\tPSJF\tNPSJF\tPriority" << endl;
  outfile << "===========================================================" << endl;
  for (i = 0; i < fcfs.size() || i < rr.size() ||  i < psjf.size() ||  i < nsjf.size() ||  i < pp.size(); i++) {
	if (method == 1 || method == 6) {
	  if (id_unprint) {
		outfile << fcfs.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << fcfs.get_turnarround_time(i) << "\t";

	}
	if (method == 2 || method == 6) {
	  if (id_unprint) {
		outfile << rr.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << rr.get_turnarround_time(i) << "\t";

	}
	if (method == 3 || method == 6) {
	  if (id_unprint) {
		outfile << psjf.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << psjf.get_turnarround_time(i) << "\t";

	}
	if (method == 4 || method == 6) {
	  if (id_unprint) {
		outfile << nsjf.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << nsjf.get_turnarround_time(i) << "\t";

	}
	if (method == 5 || method == 6) {
	  if (id_unprint) {
		outfile << pp.get_id(i) << "\t";
		id_unprint = false;
	  }
	  outfile << pp.get_turnarround_time(i) << "\t";

	}
	id_unprint = true;
	outfile << endl;
  } // for

  outfile << "===========================================================" << endl;
}

void output(int method, string file_name, TimeRecord fcfs, TimeRecord rr, TimeRecord psjf, TimeRecord nsjf, TimeRecord pp) {
  ofstream outfile;
  file_name = "output_" + file_name;
  outfile.open(file_name);
  gantOutput(method, outfile, fcfs, rr, psjf, nsjf, pp);
  outputWaitingTime(method, outfile, fcfs, rr, psjf, nsjf, pp);
  outputTurnarroundTime(method, outfile, fcfs, rr, psjf, nsjf, pp);
  outfile.close();
} // output()

void fcfsAndRr(vector<Data> origin_data, TimeRecord &time_record, int time_slice) {
  Queue arrival_queue(origin_data, kArrivalThanId);
  Queue cpu_queue;
  Data current_process;
  CPU cpu(time_slice);
  int clk = 0, w_time = 0, t_time = 0;
  do
  {
	clk++;
	while (!arrival_queue.empty() && clk == arrival_queue.next_arrival_time()) 
	  cpu_queue.push_back(arrival_queue.pop_front());// get process from arrival queue
	
	if (!cpu.isProcessing && cpu.isBlocking) { // terminate or time out
	  current_process = cpu.get_process(); // get the process should be time out or terminate
	  if (current_process.d_remain_burst == 0) { // process done
		t_time = clk - current_process.d_arrival_time; // turnarround time
		w_time = t_time - current_process.d_cpu_burst; // waiting time
		time_record.insert(TimeRecordObj(current_process.d_id, w_time, t_time));
	  } // if
	  else 
		cpu_queue.push_back(current_process); // push the process back to th queue

	} // if

	if (!cpu.isBlocking && !cpu_queue.empty()) // CPU is empty
	  cpu.dispatch(cpu_queue.pop_front()); // dispatch the first process in queue

	cpu.process();

  } while (!cpu_queue.empty() || !arrival_queue.empty() || cpu.isBlocking);

  time_record.packageGantRecord(cpu.get_gant());
} // fcfs()

void sjf(vector<Data> origin_data, TimeRecord &time_record, bool preemptive) {
  Queue arrival_queue(origin_data, kArrivalThanRemainTime);
  Queue cpu_queue;
  Data current_process;
  CPU cpu(999);
  int clk = 0, w_time = 0, t_time = 0;
  do
  {
	clk++;
	while (!arrival_queue.empty() && clk == arrival_queue.next_arrival_time()) 
	  cpu_queue.insert(kRemainTimeThanUsedThanArrivalThanID, arrival_queue.pop_front()); // get process from arrival queue and put it in the queue

	if (!cpu.isProcessing && cpu.isBlocking) { // terminate or time out
	  current_process = cpu.get_process(); // get the process should be time out or terminate
	  t_time = clk - current_process.d_arrival_time; // turnarround time
	  w_time = t_time - current_process.d_cpu_burst; // waiting time
	  time_record.insert(TimeRecordObj(current_process.d_id, w_time, t_time));

	} // if
	else if (preemptive && cpu.preemptive(kSjf, cpu_queue.next_burst_remain())) { // preemptive
	  cpu_queue.insert(kRemainTimeThanUsedThanArrivalThanID, cpu.get_process());
	  cpu.dispatch(cpu_queue.pop_front());
	}

	if (!cpu.isBlocking && !cpu_queue.empty()) // CPU is empty
	  cpu.dispatch(cpu_queue.pop_front()); // dispatch the first process in queue

	cpu.process();
  } while (!cpu_queue.empty() || !arrival_queue.empty() || cpu.isBlocking);

  time_record.packageGantRecord(cpu.get_gant());
} // sjf()

void pP(vector<Data> origin_data, TimeRecord &time_record) {
  Queue arrival_queue(origin_data, kArrivalThanPriority);
  Queue cpu_queue;
  Data current_process;
  CPU cpu(999);
  int clk = 0, w_time = 0, t_time = 0;
  do
  {
	clk++;
	while (!arrival_queue.empty() && clk == arrival_queue.next_arrival_time()) 
	  cpu_queue.insert(kProrityThanUsedThanArrivalThanID, arrival_queue.pop_front()); // get process from arrival queue and put it in the queue

	if (!cpu.isProcessing && cpu.isBlocking) { // terminate or time out
	  current_process = cpu.get_process(); // get the process should be time out or terminate
	  t_time = clk - current_process.d_arrival_time; // turnarround time
	  w_time = t_time - current_process.d_cpu_burst; // waiting time
	  time_record.insert(TimeRecordObj(current_process.d_id, w_time, t_time));

	} // if
	else if (cpu.preemptive(kPp, cpu_queue.next_priority())) {
	  cpu_queue.insert(kProrityThanUsedThanArrivalThanID, cpu.get_process());
	  cpu.dispatch(cpu_queue.pop_front());
	}

	if (!cpu.isBlocking && !cpu_queue.empty()) // CPU is empty
	  cpu.dispatch(cpu_queue.pop_front()); // dispatch the first process in queue

	cpu.process();
  } while (!cpu_queue.empty() || !arrival_queue.empty() || cpu.isBlocking);

  time_record.packageGantRecord(cpu.get_gant());
} // sjf()

int main()
{
  int method, slice;
  string file_name;
  vector<Data> vData;
  TimeRecord fcfs, rr, psjf, nsjf, pp;
  do {
	fcfs.clear();
	rr.clear();
	psjf.clear();
	nsjf.clear();
	pp.clear();
	vData.clear();
	cout << "input the file name:\n>";
	cin >> file_name;
	if (input(vData, file_name, method, slice)) {
	  if (method == 1 || method == 6) {
		fcfsAndRr(vData, fcfs, 999);
	  }
	  if (method == 2 || method == 6) {
		fcfsAndRr(vData, rr, slice);
	  }
	  if (method == 3 || method == 6) {
		sjf(vData, psjf, true); // psjf
	  }
	  if (method == 4 || method == 6) {
		sjf(vData, nsjf, false); // nsjf
	  }
	  if (method == 5 || method == 6) {
		pP(vData, pp); // pp
	  }
	  output(method, file_name, fcfs, rr, psjf, nsjf, pp);
	}
	else 
	  cout << "Can't found " << file_name << "!!!" << endl;
	
	cout << "input \"quit\" to quit:\n>"; 
	cin >> file_name;
  } while (file_name != "quit");
  
}
