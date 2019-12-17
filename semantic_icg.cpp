#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <iomanip>
#include "lexer.h"
#include "syntax.h"

int main(int argc, char* argv[]){
  readfile();
  lexer();
  while(stream_ptr != tok_stream.size()){
    vector<token> statement;
    getStatement(statement);
    try{
      parse(statement);
    }
    catch(string e){
      cout << e;
      exit(0);
    }
  }
  //print_tokens();
  print_instr_table();
  print_symbol_table();
  inFile.close();
  outFile.close();
  return 0;
}

void readfile(){
  string ifname = "", ofname = "";
  // cout << "Enter input file name: ";
  // cin >> ifname;
  try{
    //inFile.open(ifname);
    inFile.open("a.txt");
    if(!inFile.is_open()){
      string err = "Error: File failed to open.\n";
      throw err;
    }
  }
  catch(string e){
    cout << e;
    exit(0);
  }
  // cout << "Enter output file name: ";
  // cin >> ofname;
  // outFile.open(ofname);
  outFile.open("o.txt");
}
