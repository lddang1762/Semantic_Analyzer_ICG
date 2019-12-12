#include <vector>

using namespace std;

ifstream inFile;
ofstream outFile;

vector<string> keywords = {"int", "float", "bool","else","then","if", "endif","while","whileend","do","doend",
                            "for","forend","input","output","and","or","function", "begin", "end"};
vector<char> separators = {'\'', '(', ')', '{', '}', '[', ']', ',',':',';'};
vector<char> operators = {'*', '+', '-', '=', '/', '>', '<', '%'};
int DSM_table[9][10] = { {2, 4, 8, 9, 1, 1, 6, 4, 1, 1},
                        {2, 2, 3, 3, 3, 2, 3, 3, 3, 3},
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                        {5, 4, 5, 5, 5, 5, 5, 4, 5, 5},
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                        {6, 6, 6, 6, 6, 6, 7, 6, 6, 7},
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct token{
  int tok_type; // 1 = keyword, 2 = identifier, 3 = number, 4 = separator, 5 = operator
  string lex;
  void print(){
    string token = "";
    if(tok_type == 1) {token = "KEYWORD";}
    else if(tok_type == 2) {token = "IDENTIFIER";}
    else if(tok_type == 3) {token = "NUMBER";}
    else if(tok_type == 4) {token = "SEPARATOR";}
    else if(tok_type == 5) {token = "OPERATOR";}
    cout << left << setw(25) << "Token: " + token << setw(35) <<"Lexeme:   " + lex << endl;
    outFile << left << setw(25) << "Token: " + token << setw(35) <<"Lexeme:   " + lex << endl;
  }
};

int stream_ptr = 0;
vector<token> tok_stream;

void lexer();
void readfile();
void analyzeToken(string tok, int state);
int char_to_col(char c);
int transitionState(int state, char c);
bool isAcceptedState(int state);
bool isOperator(char c);
bool isSeparator(char c);
bool isKeyword(string tok);
void print_tokens();


void lexer(){
  int currentState = 1;
  string token = "";
  char c;
  do{
    inFile.get(c);
    if(isAcceptedState(currentState)){
      inFile.unget();
      analyzeToken(token, currentState);
      currentState = 1;
      token = "";
    }
    else{
      if(isSeparator(c) || isOperator(c) || c == '!'){
        if(token.length() == 0){ token += c; }
        else{
          if(currentState != 6){ inFile.unget(); }
        }
      }
      else if((!(c == ' ' || c == '\n' || c == '\t'))){ token += c; }
      currentState = transitionState(currentState - 1, c);
    }
  }while(!inFile.eof());
}

void analyzeToken(string tok, int state){
  if(state == 3){
    if(isKeyword(tok)){
      token kw;
      kw.tok_type = 1;
      kw.lex = tok;
      tok_stream.push_back(kw);
    }
    else{
      token id;
      id.tok_type = 2;
      id.lex = tok;
      tok_stream.push_back(id);
    }
  }
  else if(state == 5){
    token num;
    num.tok_type = 3;
    num.lex = tok;
    tok_stream.push_back(num);
    // printf("%-15s %-25s\n", "NUMBER", tok.c_str());
  }
  else if(state == 7){
    //do nothing, state 7 is a comment block
  }
  else if(state == 8){
    token sep;
    sep.tok_type = 4;
    sep.lex = tok;
    tok_stream.push_back(sep);
  }
  else if(state == 9){
    token op;
    op.tok_type = 5;
    op.lex = tok;
    tok_stream.push_back(op);
  }
}

int char_to_col(char c){
  if(isalpha(c)){ return 0; }
  if(isdigit(c)){ return 1; }
  if(c == ' '){ return 4; }
  if(c == '$'){ return 5; }
  if(c == '!'){ return 6; }
  if(c == '.'){ return 7; }
  if(c == '\n' || c == '\t'){ return 8; }
  if(inFile.eof()){ return 9; }
  if(isSeparator(c)){ return 2; }
  if(isOperator(c)){ return 3; }

  return 0;
}

int transitionState(int state, char c){
  int col = char_to_col(c);
  return DSM_table[state][col];
}

bool isAcceptedState(int state){
  if(state == 3 || state == 5 || state == 7 || state == 8 || state == 9)
    return true;
  return false;
}

bool isOperator(char c){
  for(auto iter = operators.begin(); iter != operators.end(); iter++){
    if(c == *iter){
      return true;
    }
  }
  return false;
}

bool isSeparator(char c){
  for(auto iter = separators.begin(); iter != separators.end(); iter++){
    if(c == *iter){
      return true;
    }
  }
  return false;
}

bool isKeyword(string tok){
  for(auto iter = keywords.begin(); iter != keywords.end(); iter++){
    if(tok == *iter){
      return true;
    }
  }
  return false;
}

void print_tokens(){ //for reference

    for(auto iter = tok_stream.begin(); iter != tok_stream.end(); iter++){
      iter->print();
    }

}
