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
#define SIZE 80
using namespace std;

template <class T>
class MutexQueue;

void printProgram(map<long,long> &program, const long &n);
long runProgram(map<long,long> program, const long n, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue);
void insert(map<long,long> &program, const long &pos, const long &val);
long getValue(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
long getPos(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
void printMap(int screen[SIZE][SIZE]);
void updatePaint(int x, int y, int color);
void updateScreen(int x, int y, int dir);

void runScreen(MutexQueue<int>& inputQueue, MutexQueue<int>& instructionQueue);

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

mutex cursesMutex;

void printStr(int row, int col, string string){
    cursesMutex.lock();
    mvaddstr(row, col, string.c_str());
    cursesMutex.unlock();
}

void printCh(int row, int col, unsigned int ch){
    cursesMutex.lock();
    mvaddch(row, col, ch);
    cursesMutex.unlock();
}

unsigned int input(){
    flushinp();
    unsigned int c =  getch();
    return c;
}

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
      last = next+1;
    }

    // Adjust program to free play
    program[0]=2;

    MutexQueue<int> inputQueue;
    MutexQueue<int> instructionQueue;
    thread computer, robot;

  	initscr();
  	clear();
  	noecho();
  	cbreak();	/* Line buffering disabled. pass on everything */
    curs_set(0);

    computer = thread(runProgram, program, n, std::ref(inputQueue), std::ref(instructionQueue));
    robot = thread(runScreen, std::ref(inputQueue), std::ref(instructionQueue));

    computer.join();
    robot.join();

    printStr(LINES-1, 0, ("max: "+to_string(LINES)+" "+to_string(COLS)));
    refresh();
    input();
    endwin();

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

void runScreen(MutexQueue<int>& inputQueue, MutexQueue<int>& instructionQueue){
  int screen[SIZE][SIZE];

  // Directions.  0 = up, 1 = right, 2 = down, 3 = left
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      screen[i][j] = 0;
    }
  }

  printMap(screen);
  printStr(LINES-1, 0, ("max: "+to_string(LINES)+" "+to_string(COLS)));
  int count = 0;
  int ballX = 0;
  int paddleX = 0;
  while(true){
    // send camera signal
    // inputQueue.addMessage(screen[x][y]);
    // Wait for instruction
    int x = instructionQueue.retrieveMessage();
    // Check for halt
    if (x == 999) {
      int blocks = 0;
      for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
          if (screen[i][j] == 2) { // if a block
            blocks +=1;
          }

        }
      }
      printStr(LINES-2, 0, ("block count: "+to_string(blocks)));

      return;
    }
    else if (x == 901){ // check for input needed

      if (ballX > paddleX) inputQueue.addMessage(1);
      else if (ballX < paddleX) inputQueue.addMessage(-1);
      else inputQueue.addMessage(0);

    }
    else{
      int y = instructionQueue.retrieveMessage();
      int id = instructionQueue.retrieveMessage();

      if (id == 4){  //ball
        ballX = x;
      }
      else if (id == 3){ //horizontal paddle
        paddleX = x;
      }
      if (x >= SIZE || x < -1 || y >= SIZE || y < -1 ){
          endwin();
          cout << "ERROR ("<< x<<" , "<<y<<")"<<endl;
          return;
      }
      if (x == -1 && y == 0 ){
        printStr(LINES-4, 0, ("score: "+to_string(id)));
      }
      else{
        screen[x][y] = id;

        updateScreen(x, y, id);
        refresh();
        if (count > 945 ) std::this_thread::sleep_for(std::chrono::milliseconds(10)); // ignore first draw
        else std::this_thread::sleep_for(std::chrono::milliseconds(1)); // ignore first draw
        printStr(LINES-2, 0, ("count: "+to_string(count)));
        count++;
      }

    }
  }
}

void printMap(int screen[SIZE][SIZE]){
  clear();
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      if (screen[i][j] == 0){
        printCh(i+1, j+1,' ');
      }
      else{
        printCh(i+1, j+1,' '|A_REVERSE);
        // printCh(i, j,ACS_DIAMOND);

      }
    }
  }
  refresh();
}

void updateScreen(int x, int y, int id){
  switch (id) {
    case 0: //empty
      printCh(y, x,' ');
      break;
    case 1: //wall
      printCh(y, x,ACS_PLUS);
      break;
    case 2: //block
      printCh(y, x,' '|A_REVERSE);
      break;
    case 3: //horizontal paddle
      printCh(y, x,ACS_HLINE);
      break;
    case 4: //ball
      printCh(y, x,ACS_DIAMOND);
      break;
  }
}

// wmove(win, y, x);


long runProgram(map<long,long> program, const long n, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue)
{
  long relativeBase = 0;
  // Run program
  for (long i = 0; i < n+1; ){

    int modeVar = program[i] / 100;
    int mode[3];
    mode[2] = modeVar / 100;
    mode[1] = (modeVar / 10) % 10;
    mode[0] = modeVar % 10;

    long instruction = program[i] % 100;
    if (instruction == 99) {
      // Tell robot to quit
      outQueue.addMessage(999);

      return program[0]; // end opcode
    }
    if (instruction == 1 || instruction == 2 || instruction == 7 || instruction == 8){
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      long param2 = getValue(program, mode[1], i+2, relativeBase);
      long outputPos = getPos(program, mode[2], i+3, relativeBase);
      if (instruction == 1) // add
      {
        insert(program, outputPos, (param1+param2));
      }
      else if (instruction == 2) //multiply
      {
        insert(program, outputPos, (param1*param2));
      }
      else if (instruction == 7) //less than
      {
        if (param1<param2 ) {
          insert(program, outputPos, 1);
        }
        else {
          insert(program, outputPos, 0);
        }
      }
      else if (instruction == 8) //equals
      {
        if (param1==param2 ) {
          insert(program, outputPos, 1);
        }
        else {
          insert(program, outputPos, 0);
        }
      }
      i += 4;
    }
    else if (instruction == 3){ // input
      long outputPos = getPos(program, mode[0], i+1, relativeBase);
      // cin Input
      // long input = 0;
      // cout << "Enter input: "<<endl;
      // cin >> input;

      // Queue Input
      // cout << "output:"<< param1 << endl;
      // Ask for input
      outQueue.addMessage(901);

      long input = inQueue.retrieveMessage();
      // cout << "Entered: " << input <<endl;

      // Arrow Input
      // printStr(LINES-3, 0, "Input: ");
      // refresh();
      // char key = input();
      // int val = 0;
      // if (int(key) == KEY_LEFT || key == 'a'){
      //   val = -1;
      //   // printStr(LINES-3, 8, "Left  ");
      // }
      // else if (int(key) == KEY_RIGHT || key == 'd'){
      //   val = 1;
      //   // printStr(LINES-3, 8, "Right ");
      // }
      // else{
      //   // printStr(LINES-3, 8, "None  ");
      // }
      //   printCh(LINES-3, 15, key);

      insert(program, outputPos, input);

      i += 2;
    }
    else if (instruction == 4){ // print
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      // cout << "output:"<< param1 << endl;
      outQueue.addMessage(param1);

      // cout << param1 << endl;
      i += 2;
    }
    else if (instruction == 5 || instruction == 6){ // jump
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      long param2 = getValue(program, mode[1], i+2, relativeBase);
      if ((param1 != 0 && instruction == 5) || (param1 == 0 && instruction == 6)){
        i = param2;
      }
      else{
        i += 3;
      }
    }
    else if (instruction == 9){ // offset relativeBase
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      relativeBase += param1;
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
