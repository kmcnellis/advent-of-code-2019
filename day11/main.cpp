#include <string>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <streambuf>
#include <exception>
#include <algorithm>
#include <map>
#include <mutex>
#include <queue>
#include <cstdlib>
#include <thread>
#include <curses.h>
#include <set>
#include <chrono>

#define INPUT 3
#define SIZE 148
using namespace std;

template <class T>
class MutexQueue;

void printProgram(map<long,long> &program, const long &n);
long runProgram(map<long,long> program, const long n, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue);
void insert(map<long,long> &program, const long &pos, const long &val);
long getValue(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
long getPos(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
void printMap(int hull[SIZE][SIZE]);
void updatePaint(int x, int y, int color);
void updateRobot(int x, int y, int dir);

void runRobot(MutexQueue<int>& cameraQueue, MutexQueue<int>& instructionQueue);

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
  if (argc < 2) {
      // Tell the user how to run the program
      std::cerr << "Usage: " << argv[0] << " <FILE.txt>" << std::endl;
      return 1;
  }

  // Input from file
    ifstream t(argv[1]);
    string file((istreambuf_iterator<char>(t)),
                     istreambuf_iterator<char>());

    size_t n = count(file.begin(), file.end(), ',');

    map<long,long> program;
    long last = 0, next = 0;
    string curr;
    // cout << "file: "<<file<<endl<<endl;
    for (long i = 0; i < n+1; i++){
      // find next
      next = file.find(',', last);
      curr = file.substr(last,next-last);
      // convert to long
      insert(program, i, stol(curr));
      // program[i] = stoi(curr);
      // cout << ":"<<last<<","<<next<<"="<<curr<<" : "<<program[i]<<endl;
      //increment
      last = next+1;
    }

    // Adjust program to error state
    // program[1]=71;
    // program[2]=95;
    // printProgram(program, n);

    MutexQueue<int> cameraQueue;
    MutexQueue<int> instructionQueue;
    thread computer, robot;

    computer = thread(runProgram, program, n, std::ref(cameraQueue), std::ref(instructionQueue));
    robot = thread(runRobot, std::ref(cameraQueue), std::ref(instructionQueue));

    computer.join();
    robot.join();
    return 0;
}

void insert(map<long,long> &program, const long &pos, const long &val){
  auto mem = program.find(pos);
  if (mem != program.end())
    mem->second = val;
  else
    program.emplace(pos, val);
}

long getValue(map<long,long> &program, const int &mode, const long &param, const long &relativeBase){
    long position = 0;
    if (mode==0){
      position = program[param];
    }
    else if (mode==1){
      position = param;
    }
    else{
      position = program[param]+relativeBase;
    }
    return program[position];

}
long getPos(map<long,long> &program, const int &mode, const long &param, const long &relativeBase){
    if (mode==0){
      return program[param];
    }
    else if (mode==1){
      return param;
    }
    else{
      return program[param]+relativeBase;
    }
}

void runRobot(MutexQueue<int>& cameraQueue, MutexQueue<int>& instructionQueue){
  initscr();
  cbreak();
  noecho();
  curs_set(0);

  int hull[SIZE][SIZE];
  int x = 50, y = 110, dir = 0;
  set <int, greater <int> > points;


  // Directions.  0 = up, 1 = right, 2 = down, 3 = left
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      hull[i][j] = 0;
    }
  }
  hull[x][y] = 1;

  printMap(hull);
    mvaddstr(3, 0, ("max: "+to_string(LINES)+" "+to_string(COLS)).c_str());
  while(true){
    // send camera signal
    cameraQueue.addMessage(hull[x][y]);
    // Wait for instruction
    int paint = instructionQueue.retrieveMessage();
    // Check for halt
    if (paint == 99) {
      mvaddstr(0, 0, ("num painted: "+to_string(points.size())).c_str());
      mvaddstr(1, 0, ("max: "+to_string(LINES)+" "+to_string(COLS)).c_str());
      refresh();
      getch();
      endwin();
      return;
    }
    int move = instructionQueue.retrieveMessage();

    int oldx=x, oldy=y, olddir=dir;
    hull[x][y] = paint;
    points.insert((x*10000)+y);

    if (move == 0) dir -=1;
    else if (move == 1) dir+=1;

    dir = dir%4;
    if (dir < 0) dir +=4;

    switch (dir) {
      case 0: //up
        y -= 1;
        break;
      case 1: //right
        x += 1;
        break;
      case 2: //down
        y += 1;
        break;
      case 3: //left
        x -= 1;
        break;

    }
    if (x >= SIZE || x < 0 || y >= SIZE || y < 0 ){
        endwin();
        cout << "ERROR ("<< x<<" , "<<y<<")"<<endl;
        return;
    }

    switch (dir) {
      case 0: //up
        // mvaddstr(1, 0, ("("+to_string(x)+","+to_string(y)+")").c_str());
        // mvaddstr(2, 0, (paint==0?"white":"black"));
        break;
      case 1: //right
        // mvaddstr(1, 0, ("("+to_string(x)+","+to_string(y)+")").c_str());
        // mvaddstr(2, 0, (paint==0?"white":"black"));
        break;
      case 2: //down
        // mvaddstr(1, 0, ("("+to_string(x)+","+to_string(y)+")").c_str());
        // mvaddstr(2, 0, (paint==0?"white":"black"));
        break;
      case 3: //left
        // mvaddstr(1, 0, ("("+to_string(x)+","+to_string(y)+")").c_str());
        // mvaddstr(2, 0, (paint==0?"white":"black"));
        break;
    }
    updateRobot(x, y, olddir);
    updatePaint(oldx, oldy, paint);
    refresh();
    // std::this_thread::sleep_for(std::chrono::milliseconds(5));
    // switch (dir) {
    //   case 0: //up
    //     mvaddstr(0, 0, "up   ");
    //     break;
    //   case 1: //right
    //     mvaddstr(0, 0, "right");
    //     break;
    //   case 2: //down
    //     mvaddstr(0, 0, "down ");
    //     break;
    //   case 3: //left
    //     mvaddstr(0, 0, "left ");
    //     break;
    // }
    // mvaddstr(0, 0, ("num: "+to_string(points.size())).c_str());

    updateRobot(x, y, dir);
    refresh();
    // std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void printMap(int hull[SIZE][SIZE]){
  clear();
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      if (hull[i][j] == 0){
        mvaddch(i, j,' ');
      }
      else{
        mvaddch(i, j,' '|A_REVERSE);
        // mvaddch(i, j,ACS_DIAMOND);

      }
    }
  }
  refresh();
}

void updatePaint(int x, int y, int color){
  if (color == 0){
    mvaddch(y, x,' ');
  }
  else{
    // mvaddch(y, x,ACS_DIAMOND);
    mvaddch(y, x,' '|A_REVERSE);
  }
}

void updateRobot(int x, int y, int dir){
  switch (dir) {
    case 0: //up
      mvaddch(y, x,ACS_UARROW);
      break;
    case 1: //right
      mvaddch(y, x,ACS_RARROW);
      break;
    case 2: //down
      mvaddch(y, x,ACS_DARROW);
      break;
    case 3: //left
      mvaddch(y, x,ACS_LARROW);
      break;
  }
}

// wmove(win, y, x);


long runProgram(map<long,long> program, const long n, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue)
{
  long relativeBase = 0;
  // Run program
  for (long i = 0; i < n+1; ){
    // cout<<"i:"<<i<<" ";

    int modeVar = program[i] / 100;
    int mode[3];
    mode[2] = modeVar / 100;
    mode[1] = (modeVar / 10) % 10;
    mode[0] = modeVar % 10;

    long instruction = program[i] % 100;
    // cout<<"instruction: "<<instruction<<endl;
    if (instruction == 99) {
      // Tell robot to quit
      outQueue.addMessage(99);

      return program[0]; // end opcode
    }
    if (instruction == 1 || instruction == 2 || instruction == 7 || instruction == 8){
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      long param2 = getValue(program, mode[1], i+2, relativeBase);
      long outputPos = getPos(program, mode[2], i+3, relativeBase);
      if (instruction == 1) // add
      {
        // cout<<"add"<<endl;
        // cout<<param1<<" "<<param2<<" in "<<outputPos<<" = "<<(param1+param2)<<endl;
        insert(program, outputPos, (param1+param2));
        // program[outputPos] = param1+param2;

      }
      else if (instruction == 2) //multiply
      {
        // cout<<"multiply"<<endl;
        // cout<<param1<<" "<<param2<<" in "<<outputPos<<" = "<<(param1*param2)<<endl;
        insert(program, outputPos, (param1*param2));
        // program[outputPos] = param1*param2;

      }
      else if (instruction == 7) //less than
      {
        // cout<<"less than"<<endl;
        // cout<<param1<<" "<<param2<<" in "<<outputPos<<" = "<<(param1<param2)<<endl;

        if (param1<param2 ) {
          insert(program, outputPos, 1);
          // program[outputPos] = 1;

        }
        else {
          insert(program, outputPos, 0);
          // program[outputPos] = 0;
        }
      }
      else if (instruction == 8) //equals
      {
        // cout<<"equals"<<endl;
        // cout<<param1<<" "<<param2<<" in "<<outputPos<<" = "<<(param1==param2)<<endl;

        if (param1==param2 ) {
          insert(program, outputPos, 1);
          // program[outputPos] = 1;
        }
        else {
          insert(program, outputPos, 0);
          // program[outputPos] = 0;
        }
      }
      i += 4;
    }
    else if (instruction == 3){ // input
      // cout<<"input" <<endl;
      long outputPos = getPos(program, mode[0], i+1, relativeBase);
      // cout<< program[i] <<" "<< program[i+1] << " "<<mode[0] << " "<<relativeBase << " "<< outputPos<<endl;
      // long input = 0;
      // cout << "Enter input: "<<endl;
      // cin >> input;
      long input = inQueue.retrieveMessage();
      // cout << "Entered: " << input <<endl;

      insert(program, outputPos, input);
      program[outputPos] = input;


      i += 2;
    }
    else if (instruction == 4){ // print
      // cout<<"print"<<endl;
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      // cout << "output:"<< param1 << endl;
      outQueue.addMessage(param1);

      // cout << param1 << endl;
      i += 2;
    }
    else if (instruction == 5 || instruction == 6){ // jump
      // cout<<"jump" << ((instruction==5)?" if true":" if false")<<endl;
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      long param2 = getValue(program, mode[1], i+2, relativeBase);
      // cout << param1 << " " <<param2<< endl;
      if ((param1 != 0 && instruction == 5) || (param1 == 0 && instruction == 6)){
        i = param2;
      }
      else{
        i += 3;
      }
    }
    else if (instruction == 9){ // offset relativeBase
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      // cout<<"offset "<<endl;
      relativeBase += param1;
      // cout<<param1<<" "<<relativeBase<<endl;
      i += 2;
    }
  }

  return program[0];
}

void printProgram(map<long,long> &program, const long &n)
{
  cout <<"---------"<<endl;
  for (long i = 0; i < n+1; ){
    long instruction = program[i] % 100;

    if (instruction == 99) {
      cout << std::setw(6) << program[i] << endl;
      i+=1;
    }
    if (instruction == 3 || instruction == 4 || instruction == 9) {
      cout << std::setw(6) << program[i] << " ";
      cout << std::setw(6) << program[i+1] << endl;
      i+=2;
    }
    if (instruction == 5 || instruction == 6) {
      cout << std::setw(6) << program[i] << " ";
      cout << std::setw(6) << program[i+1] << " ";
      cout << std::setw(6) << program[i+2] << endl;
      i+=3;
    }
    else{ // 1, 2, 7, 8
      cout << std::setw(6) << program[i] << " ";
      cout << std::setw(6) << program[i+1] << " ";
      cout << std::setw(6) << program[i+2] << " ";
      cout << std::setw(6) << program[i+3] << endl;
      i+=4;
    }
  }
  cout <<endl;
  cout <<"---------"<<endl;

}
