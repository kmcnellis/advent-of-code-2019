#include <string>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <streambuf>
#include <exception>
#include <algorithm>
#include <map>

#define INPUT 3
using namespace std;

void printProgram(map<long,long> &program, const long &n);
long runProgram(map<long,long> &program, const long &n);
void insert(map<long,long> &program, const long &pos, const long &val);
long getValue(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);
long getPos(map<long,long> &program, const int &mode, const long &param, const long &relativeBase);

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

    long result = runProgram(program,n);
    // 270000*A + 520625 + B
    // cout<< "result: "<< result;
    // cout<<endl;

    // printProgram(program, n);

    // cout<<endl<< "pos 0 = "<<program[0]<<endl;
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

long runProgram(map<long,long> &program, const long &n)
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
    if (instruction == 99) return program[0]; // end opcode
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
      cout<< program[i] <<" "<< program[i+1] << " "<<mode[0] << " "<<relativeBase << " "<< outputPos<<endl;
      long input = 0;
      cout << "Enter input: "<<endl;
      cin >> input;
      // cout << "Entered: " << input <<endl;

      insert(program, outputPos, input);
      program[outputPos] = input;


      i += 2;
    }
    else if (instruction == 4){ // print
      // cout<<"print"<<endl;
      long param1 = getValue(program, mode[0], i+1, relativeBase);
      cout << "output:"<< param1 << endl;
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
