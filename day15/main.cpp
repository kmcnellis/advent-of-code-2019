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
#define OFFSET 40
using namespace std;

template <class T>
class MutexQueue;

void printProgram(map<long,long> &program, const long &n);
long runProgram(map<long,long> program, const long n, MutexQueue<int>& inQueue, MutexQueue<int>& outQueue);
void insert(map<long,long> &program, const long &pos, const long &val);
void insert(map<long,map<long,long>> &grid, const long &x, const long &y, const long &val);
bool visited(map<long,map<long,long>> &grid, const long &x, const long &y);
long getValue(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
long getPos(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
void printMap(int screen[SIZE][SIZE]);
void updatePaint(int x, int y, int color);
void updateScreen(int x, int y, int dir);

void runScreen(MutexQueue<int>& inQueue, MutexQueue<int>& instructionQueue, map<long,map<long,long>> &grid, int &finalX, int &finalY);
int fillGrid(map<long,map<long,long>> &grid, const int &finalX, const int &finalY);

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
    refresh();
    cursesMutex.unlock();
}

void printCh(int row, int col, unsigned int ch){
    cursesMutex.lock();
    mvaddch(row, col, ch);
    refresh();
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

    MutexQueue<int> inQueue;
    MutexQueue<int> instructionQueue;
    thread computer, robot;

  	initscr();
  	clear();
  	noecho();
  	cbreak();	/* Line buffering disabled. pass on everything */
    curs_set(0);

    // printStr(0, 0, ("max: "+to_string(LINES)+" "+to_string(COLS)));
    map<long,map<long,long>> grid;
    int x = 0, y = 0;
    computer = thread(runProgram, program, n, std::ref(inQueue), std::ref(instructionQueue));
    robot = thread(runScreen, std::ref(inQueue), std::ref(instructionQueue), std::ref(grid), std::ref(x), std::ref(y));

    computer.join();
    robot.join();

    printStr(LINES-3, 0, "FOUND oxygen at " + to_string(x)+", "+to_string(y) );
    printStr(LINES-2, 0, "DONE");

    int result = fillGrid(grid, x, y);
    printStr(LINES-4, 0, "Final Cycles " + to_string(result)+"                          ");

    refresh();
    input();
    endwin();

    return 0;
}

void insert(map<long,long> &program, const long &pos, const long &val){
  std::map<long,long>::iterator mem = program.find(pos);
  if (mem != program.end())
    mem->second = val;
  else
    program.emplace(pos, val);
  // program[pos] = val;
}

void insert(map<long,map<long,long>> &grid, const long &x, const long &y, const long &val){
  std::map<long,map<long,long>>::iterator row = grid.find(y);
  if (row != grid.end()){
    std::map<long,long>::iterator col = row->second.find(x);
    if (col != row->second.end()){
      col->second = val;
    }
    else{
      (row->second).emplace(x, val);
    }
  }
  else{
    map<long,long> newRow;
    // std::pair<std::map<long,long>::iterator,bool>
    auto result = grid.emplace(y, newRow);
    result.first->second.emplace(x, val);
  }
}

bool visited(map<long,map<long,long>> &grid, const long &x, const long &y){
  std::map<long,map<long,long>>::iterator row = grid.find(y);
  if (row != grid.end()){
    std::map<long,long>::iterator col = row->second.find(x);
    if (col != row->second.end()){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    return false;
  }
}

bool get(map<long,map<long,long>> &grid, const long &x, const long &y){
  std::map<long,map<long,long>>::iterator row = grid.find(y);
  if (row != grid.end()){
    std::map<long,long>::iterator col = row->second.find(x);
    if (col != row->second.end()){
      return col->second;
    }
  }
  return -1;
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

void runScreen(MutexQueue<int>& inQueue, MutexQueue<int>& instructionQueue, map<long,map<long,long>> &grid, int &finalX, int &finalY){
  vector<int> moves;

  int x = 0, y = 0;
  int lastMove = 0;
  printStr(LINES-2, 0, "Waiting...");
  updateScreen(x,y, 2);

  while(true){
    // Wait for instruction
    int output = instructionQueue.retrieveMessage();
    printStr(LINES-2, 0, "Message " + to_string(output));
    // Check for halt
    if (output == 999) {
      return;
    }
    else if (output == 901){ // check for input needed

      // Arrow Input
      // printStr(LINES-1, 0, "Input: ");
      // refresh();
      // char key = input();
      // switch (key) {
      //   case 'w': // north (1)
      //     lastMove = 1;
      //     break;
      //   case 's': // south (2)
      //     lastMove = 2;
      //     break;
      //   case 'a': // west (3)
      //     lastMove = 3;
      //     break;
      //   case 'd': // east (4)
      //     lastMove = 4;
      //     break;
      // }
      // printStr(LINES-1, 8, "    ");
      // printCh(LINES-1, 8, key);

      // Algo input
      printStr(LINES-2, 0, "MOVE           " );
      if(!visited(grid, x, y-1)){ // north
        lastMove = 1;
        moves.push_back(lastMove);
      }
      else if(!visited(grid, x, y+1)){ // south
        lastMove = 2;
        moves.push_back(lastMove);
      }
      else if(!visited(grid, x-1, y)){ // west
        lastMove = 3;
        moves.push_back(lastMove);
      }
      else if(!visited(grid, x+1, y)){ // east
        lastMove = 4;
        moves.push_back(lastMove);
      }
      else if (moves.size() > 0){  // undo
          int previous = moves.back();
          printStr(LINES-2, 0, "Undo " + to_string(previous));
          moves.pop_back();
          switch (previous) {
            case 1: // north (1)
              lastMove = 2;
              break;
            case 2: // south (2)
              lastMove = 1;
              break;
            case 3: // west (3)
              lastMove = 4;
              break;
            case 4: // east (4)
              lastMove = 3;
              break;
          }
      }
      else{ // done exploring
        inQueue.addMessage(999);
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      inQueue.addMessage(lastMove);

    }
    else{
      if (output == 0){  //wall, didn't move
        switch (lastMove) {
          case 1: // north (1)
            insert(grid, x, y-1, 1);
            updateScreen(x,y-1, 1);
            break;
          case 2: // south (2)
            insert(grid, x, y+1, 1);
            updateScreen(x,y+1, 1);
            break;
          case 3: // west (3)
            insert(grid, x-1, y, 1);
            updateScreen(x-1,y, 1);
            break;
          case 4: // east (4)
            insert(grid, x+1, y, 1);
            updateScreen(x+1,y, 1);
            break;
        }
        moves.pop_back();
        updateScreen(x,y, 2);
      }
      else if (output == 1 || output == 2){ //moved
        insert(grid, x, y, 0);
        updateScreen(x,y, 0);
        switch (lastMove) {
          case 1: // north (1)
            y -= 1;
            break;
          case 2: // south (2)
            y += 1;
            break;
          case 3: // west (3)
            x -= 1;
            break;
          case 4: // east (4)
            x += 1;
            break;
        }
        updateScreen(x,y, 2);
        if (output == 2){ //arrived
          finalX = x;
          finalY = y;
          // printStr(LINES-3, 0, "FOUND at " + to_string(x)+", "+to_string(y) );
          // part 1: stop here
          // return;
        }
      }
      refresh();
    }
  }
}


int fillGrid(map<long,map<long,long>> &grid, const int &finalX, const int &finalY){
  vector<pair<int,int>> frontier;
  vector<pair<int,int>> next_frontier;
  int cycles = 0;

  int x = finalX, y = finalY;
  frontier.emplace_back(finalX,finalY);

  while (frontier.size() > 0){
    pair<int,int> next = frontier.back();
    x = next.first;
    y = next.second;
    frontier.pop_back();

    if (get(grid, x, y) == 0 ){// free, add neighbors
      updateScreen(x,y, 3);
      insert(grid, x, y, 3);
      next_frontier.emplace_back(x+1,y);
      next_frontier.emplace_back(x-1,y);
      next_frontier.emplace_back(x,y+1);
      next_frontier.emplace_back(x,y-1);
    }

    refresh();
    if(frontier.size() == 0){ // done with this group
      if(next_frontier.size() == 0){ // nothing in the new group
        return cycles-1;
      }
      frontier.swap(next_frontier); // next group

      cycles += 1;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  return cycles-1;
}


void printMap(int screen[SIZE][SIZE]){
  clear();
  for (int i = 0; i < SIZE; i++){
    for (int j = 0; j < SIZE; j++){
      if (screen[i][j] == 0){
        printCh(i+1+OFFSET, j+1+OFFSET,' ');
      }
      else{
        printCh(i+1+OFFSET, j+1+OFFSET,' '|A_REVERSE);
        // printCh(i, j,ACS_DIAMOND);

      }
    }
  }
  refresh();
}

void updateScreen(int x, int y, int id){
  switch (id) {
    case 0: //empty
      printCh(y+OFFSET, x+OFFSET,' ');
      break;
    case 1: //wall
      printCh(y+OFFSET, x+OFFSET,' '|A_REVERSE);
      break;
    case 2: //ball
      printCh(y+OFFSET, x+OFFSET,ACS_DIAMOND);
      break;
    case 3: //source
      printCh(y+OFFSET, x+OFFSET,'x');
      break;
    case 4: //oxygen
      printCh(y+OFFSET, x+OFFSET,'.');
      break;
  }
}

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
      outQueue.addMessage(901);
      long input = inQueue.retrieveMessage();
      if (input == 999) {// done
        return 0;
      }
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
