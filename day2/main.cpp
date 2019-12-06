#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <exception>
#include <algorithm>
#include <vector>

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
    program[1]=71;
    program[2]=95;

    // 100*71+95
    // Find 2 numbers that add to 19690720
    // for (int i = 0; i < 10; i++){
    //   program[1]=i;
    //   for (int j = 0; j < 10; j++){
    //     program[2]=j;
        int result = runProgram(program,n);
        // 270000*A + 520625 + B
        int algebra = (270000*program[1]) + 520625 + program[2];
        cout<< "("<< program[1] << "," << program[2] << ") =" << result;
        cout<<" = " << algebra<<(result==algebra?"":" NO")<<endl;
        cout<<endl;
    //   }
    // }

    // printProgram(program, n);

    // cout<<endl<< "pos 0 = "<<program[0]<<endl;
    return 0;
}

int runProgram(vector<int> program, int n)
{
  // Run program
  for (int i = 0; i < n+1; ){
    if (program[i] == 99) break; // end opcode

    int input1 = program[program[i+1]];
    int input2 = program[program[i+2]];
    int outputPos = program[i+3];
    if (program[i] == 1)
    {
      // cout << "+"<<program[i+1]<<","<<program[i+2]<<": "<<input1<<" + "<<input2<<" = "<<input1+input2;
      // cout <<" store  "<<outputPos<<endl;
      // add
      program[outputPos] = input1+input2;
    }
    else if (program[i] == 2)
    {
      // cout <<"*"<<program[i+1]<<","<<program[i+2]<< ": "<<input1<<" * "<<input2<<" = "<<input1*input2;
      // cout <<" store  "<<outputPos<<endl;
      // multiply
      program[outputPos] = input1*input2;
    }

    i += 4;
  }

  // printProgram(program, n);
  return program[0];
}

void printProgram(vector<int> program, int n)
{
  cout <<"---------"<<endl;
  for (int i = 0; i < n+1; i+=4){
    if (program[i] == 99) {
      cout << program[i] << endl;
    }
    else{
      cout << program[i] << " ";
      cout << program[i+1] << " ";
      cout << program[i+2] << " ";
      cout << program[i+3] << endl;
    }
  }
  cout <<endl;
  cout <<"---------"<<endl;

}
