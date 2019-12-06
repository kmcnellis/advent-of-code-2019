#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <exception>
#include <algorithm>
#include <vector>

#define INPUT 3
using namespace std;

void printProgram(vector<int> program, int n);
int runProgram(vector<int> program, int n);

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

    vector<int> program (n+1,0);
    int last = 0, next = 0;
    string curr;
    cout << "file: "<<file<<endl<<endl;
    for (int i = 0; i < n+1; i++){
      // find next
      next = file.find(',', last);
      curr = file.substr(last,next-last);
      // convert to int
      program[i] = stoi(curr);
      // cout << ":"<<last<<","<<next<<"="<<curr<<" : "<<program[i]<<endl;
      //increment
      last = next+1;
    }

    // Adjust program to error state
    // program[1]=71;
    // program[2]=95;

    int result = runProgram(program,n);
    // 270000*A + 520625 + B
    // cout<< "result: "<< result;
    // cout<<endl;

    // printProgram(program, n);

    // cout<<endl<< "pos 0 = "<<program[0]<<endl;
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
int runProgram(vector<int> program, int n)
{
  // Run program
  for (int i = 0; i < n+1; ){
    bool immediateMode[2];
    int mode = program[i] / 100;
    immediateMode[1] = mode / 10;
    immediateMode[0] = mode % 10;

    int instruction = program[i] % 100;
    cout<<"instruction: "<<instruction<<endl;
    if (instruction == 99) return program[0]; // end opcode
    if (instruction == 1 || instruction == 2 || instruction == 7 || instruction == 8){
      int param1 = getValue(program, immediateMode[0], i+1);
      int param2 = getValue(program, immediateMode[1], i+2);
      int outputPos = program[i+3];
      if (instruction == 1) // add
      {
        cout<<"add"<<endl;
        program[outputPos] = param1+param2;
      }
      else if (instruction == 2) //multiply
      {
        cout<<"multiply"<<endl;
        program[outputPos] = param1*param2;
      }
      else if (instruction == 7) //less than
      {
        cout<<"less than"<<endl;
        if (param1<param2 ) program[outputPos] = 1;
        else program[outputPos] = 0;
      }
      else if (instruction == 8) //multiply
      {
        cout<<"multiply"<<endl;
        if (param1==param2 ) program[outputPos] = 1;
        else program[outputPos] = 0;
      }

      i += 4;
    }
    else if (instruction == 3){ // input
      cout<<"input"<<endl;
      int outputPos = program[i+1];
      int input = 0;
      cout << "Enter input: ";
      cin >> input;
      program[outputPos] = input;

      i += 2;
    }
    else if (instruction == 4){ // print
      cout<<"print"<<endl;
      int param1 = getValue(program, immediateMode[0], i+1);
      cout << "output:"<< param1 << endl;
      i += 2;
    }
    else if (instruction == 5 || instruction == 6){ // jump
      cout<<"jump"<<endl;
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
