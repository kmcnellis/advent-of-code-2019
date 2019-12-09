#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <streambuf>
#include <string>
#include <string>
#include <thread>
#include <vector>
#include <cstdlib>

using namespace std;

template <class T>
class MutexQueue;

void printProgram(vector<int> program, int n);
int runProgram(vector<int> program, int n, int id, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue);

template <class T>
class MutexQueue
{
  public:
    queue<T> message;
    mutex messageMutex;

    void addMessage(T m){
      lock_guard<mutex> lockGuard (messageMutex);
      message.push(m);
    }

    T retrieveMessage(){
      lock_guard<mutex> lockGuard (messageMutex);
      while (message.size() == 0){
        messageMutex.unlock();
        this_thread::yield();
        messageMutex.lock();
        // wait
      }
      T m = message.front();
      message.pop();
      return m;
    }
};
int main(int argc, char** argv)
{
  if (argc < 3) {
      // Tell the user how to run the program
      cerr << "Usage: " << argv[0] << " <FILE.txt>" << " <INITIAL CONDITIONS>"<< endl;
      return 1;
  }
  // Input from file
    ifstream t(argv[1]);
    string file((istreambuf_iterator<char>(t)),
                     istreambuf_iterator<char>());

    size_t n = count(file.begin(), file.end(), ',');

    vector<int> program (n+1,0);
    int last = 0, next = 0;
    string curr;
    for (int i = 0; i < n+1; i++){
      // find next
      next = file.find(',', last);
      curr = file.substr(last,next-last);
      // convert to int
      program[i] = stoi(curr);
      //increment
      last = next+1;
    }

    string initialConditions = argv[2];
    if (initialConditions.length() < 5) {
        // Tell the user how to run the program
        cerr << "Usage: " << argv[0] << " <FILE.txt>" << " <INITIAL CONDITIONS>"<< endl;
        cerr << "INITIAL CONDITIONS must be 5 numbers"<< endl;
        return 1;
    }
    MutexQueue<int> queue[5];
    vector<thread> threads;
    // cout<<"initialConditions: "<<initialConditions<<endl;

    for (int i = 0; i < 5; i++){
      int param = stoi(string(1, initialConditions[i]));
      queue[i].addMessage(param);
      // cout << "Start "<< i <<endl;

      if (i != 4){
        threads.push_back(thread(runProgram,program,n,i,std::ref(queue[i]), std::ref(queue[i+1])));
      }
      else{
        threads.push_back(thread(runProgram,program,n,i,std::ref(queue[i]), std::ref(queue[0])));
      }
    }
    // Add the input of 0 to amp 1
    queue[0].addMessage(0);

    for (int i = 0; i < 5; i++){
      // cout << "wait on "<< i <<endl;
        threads[i].join();
    }

    int final = queue[0].retrieveMessage();
    // cout << "final: "<<final <<endl;
    cout <<final <<endl;

    return 0;
}

int getValue(vector<int> program, bool immediateMode, int pos){
    if (immediateMode){
      return program[pos];
    }
    else{
      return program[program[pos]];
    }
}

int runProgram(vector<int> program, int n, int id, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue)
{
  bool hasDoneInput = false;
  // Run program
  for (int i = 0; i < n+1; ){
    bool immediateMode[2];
    int mode = program[i] / 100;
    immediateMode[1] = mode / 10;
    immediateMode[0] = mode % 10;

    int instruction = program[i] % 100;
    // cout<<"instruction: "<<instruction<<endl;
    if (instruction == 99) return program[0]; // end opcode
    if (instruction == 1 || instruction == 2 || instruction == 7 || instruction == 8){
      int param1 = getValue(program, immediateMode[0], i+1);
      int param2 = getValue(program, immediateMode[1], i+2);
      int outputPos = program[i+3];
      if (instruction == 1) // add
      {
        // cout<<"add"<<endl;
        program[outputPos] = param1+param2;
      }
      else if (instruction == 2) //multiply
      {
        // cout<<"multiply"<<endl;
        program[outputPos] = param1*param2;
      }
      else if (instruction == 7) //less than
      {
        // cout<<"less than"<<endl;
        if (param1<param2 ) program[outputPos] = 1;
        else program[outputPos] = 0;
      }
      else if (instruction == 8) //multiply
      {
        // cout<<"multiply"<<endl;
        if (param1==param2 ) program[outputPos] = 1;
        else program[outputPos] = 0;
      }

      i += 4;
    }
    else if (instruction == 3){ // input
      // cout<<id<<" input"<<endl;
      int outputPos = program[i+1];
      int input = inQueue.retrieveMessage();
      // cout<<id<<" got "<<input<<endl;

      program[outputPos] = input;

      i += 2;
    }
    else if (instruction == 4){ // print
      // cout<<"print"<<endl;
      // cout<<id<<" output"<<endl;

      int param1 = getValue(program, immediateMode[0], i+1);
      // cout<<id<<" added "<<param1<<endl;

      // cout << "output:"<< param1 << endl;
      outQueue.addMessage(param1);
      i += 2;
    }
    else if (instruction == 5 || instruction == 6){ // jump
      // cout<<"jump"<<endl;
      int param1 = getValue(program, immediateMode[0], i+1);
      int param2 = getValue(program, immediateMode[1], i+2);
      if ((param1 != 0 && instruction == 5) || (param1 == 0 && instruction == 6)){
        i = param2;
      }
      else{
        i += 3;
      }
    }
  }

  // printProgram(program, n);
  return program[0];
}

void printProgram(vector<int> program, int n)
{
  cout <<"---------"<<endl;
  for (int i = 0; i < n+1; ){
    if (program[i] == 99) {
      cout << program[i] << endl;
      i+=1;
    }
    if (program[i] == 3) {
      cout << program[i] << " ";
      cout << program[i+1] << endl;
      i+=2;
    }
    if (program[i] == 4) {
      cout << program[i] << " ";
      cout << program[i+1] << endl;
      i+=2;
    }
    else{
      cout << program[i] << " ";
      cout << program[i+1] << " ";
      cout << program[i+2] << " ";
      cout << program[i+3] << endl;
      i+=4;
    }
  }
  cout <<endl;
  cout <<"---------"<<endl;

}
