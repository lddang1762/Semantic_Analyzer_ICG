#include <stack>
#include <map>

int MEMORY_ADDRESS = 2000;
int recent_type = -1; // 0 = int, 1 = float, 2 = bool
int instr_address = 1;


                      //     0   1   2   3   4  5    6   7   8   9   10  11  12  13 14  15  16  17  18  19  20  21  22  23   24  25  26
//                           id  =   +  -   *    /   (   )   ;   ,  int  fl  bo  $  if  th  el  ei  do  wh  whe be  en  <    >  num  t/f
int parse_table[13][27] = { {1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  2,  2,  2, -1, 21, -1, -1, -1, -1, 22, -1, 23, -1, -1, -1, -1, -1},
                            {3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                            {4, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4,  4},
                            {1, -1,  5,  6, -1, -1, -1,  0,  0, -1, -1, -1, -1,  0, -1,  0, -1, -1,  0, -1, -1, -1, -1,  0,  0, -1, -1},
                            {8, -1, -1, -1, -1, -1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  8,  8},
                            {-1, -1,  0,  0,  9, 10, -1,  0,  0, -1, -1, -1, -1,  0, -1,  0, -1, -1,  0, -1, -1, -1, -1,  0,  0, -1, -1},
                            {12,-1, -1, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 31},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14, 14, 14,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, 17, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1},
                            {-1, -1, -1, -1, -1, -1, -1, -1,  0, 19, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                            {26, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                            {-1, -1, -1, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                            {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 28, 29, -1, -1} };

enum Symbols{
  //Non-terminal Symbols:
  N_S,  // S  0
  N_A,  // A  1
  N_E,  // E  2
  N_E_PRIME,  // E' 3
  N_T,  // T  4
  N_T_PRIME,  // T' 5
  N_F,  // F  6
  N_D,  // D  7
  N_TYPE, // Ty 8
  N_M,  // M  9
  N_C,  // C  10
  N_MS, // MS 11
  N_R,  // R  12

  //Terminal Symbols:

  T_ID, // id 13
  T_EQ, // =  14
  T_PLUS, // +  15
  T_MINUS,  // -  16
  T_MUL,  // *  17
  T_DIV,  // /  18
  T_L_PARENS, // (  19
  T_R_PARENS, // )  20
  T_S_COLON, // ; 21
  T_COMMA,  // ,  22
  T_INT,  // int  23
  T_FLOAT, // float   24
  T_BOOL,  // bool  25
  T_EOS, // $ 26
  T_IF, // if   27
  T_THEN, //  28
  T_ELSE, //  29
  T_ENDIF, // 30
  T_DO, //  31
  T_WHILE,  //  32
  T_WHILEEND, //
  T_BEGIN,
  T_END,
  T_L_BRACK,  // <
  T_R_BRACK,  // >
  T_NUM, // any number
  T_TF, // true or false

  PUSHI,
  PUSHM,
  POPM,
  ADD,
  SUB,
  MUL,
  DIV,

  T_ERR // error
};

stack<Symbols> s_stack;

struct i_table_symbol{
  int address;
  string op;
  string operand;
};

vector<i_table_symbol> instr_table;

struct s_table_symbol{
  token tok;
  int memory;
  string type;
};

vector<s_table_symbol> symbol_table;

stack<int> operand;
stack<int> addresses;

void parse(vector<token> &statement);
void rule(int ruleNum, token tok);
Symbols tok_to_sym(token tok);
vector<token> getStatement(vector<token> &statement);
void print_instr_table();
void add_instr_table(Symbols TOS);
void print_symbol_table();
void add_symbol_table(token tok);
bool check_symbol_table(token tok);
int symbol_table_lookup(token tok);

void parse(vector<token> &statement){
  int statement_ptr = 0;
  s_stack.push(N_S);
  while(!s_stack.empty()){
    Symbols TOS = s_stack.top();
    token i = statement[statement_ptr];
    if(i.lex == "int"){ recent_type = 1; }
    else if(i.lex == "float"){ recent_type = 2; }
    else if(i.lex == "bool"){ recent_type = 3; }
    if(TOS >= 13 && TOS <= 38){ // if terminal
      if(TOS == tok_to_sym(i)){
        s_stack.pop();
        statement_ptr++;
      }
      else{
        cout << TOS << ", " <<  tok_to_sym(i)- 13 << endl;
        string err = "Error: Expected token not found.\n";
        throw err;
        break;
      }
    }
    else if(TOS >= 0 && TOS <= 12){
      if(parse_table[TOS][tok_to_sym(i) - 13] != -1){ // subtract 13 in order to match table index
        s_stack.pop();
        if(i.tok_type == 2){
          if(i.lex != "true" && i.lex != "false"){
            add_symbol_table(i);
          }
        }
        else if(i.tok_type == 3){
          addresses.push(stoi(i.lex));
        }
        rule(parse_table[TOS][tok_to_sym(i) - 13], i);
      }
      else{
        string err = "Error: Incorrect syntax. 2\n";
        throw err;
        break;
      }
    }
    else{
      add_instr_table(TOS);

      s_stack.pop();
    }
  }
}

void rule(int ruleNum, token tok){
  switch(ruleNum){
    case 0:
    break;
    case 1:
      s_stack.push(N_A);
      break;
    case 2:
      s_stack.push(N_D);
      break;
    case 3:
      s_stack.push(T_S_COLON);
      s_stack.push(POPM);
      s_stack.push(N_E);
      s_stack.push(T_EQ);
      s_stack.push(T_ID);
      break;
    case 4:
      s_stack.push(N_E_PRIME);
      s_stack.push(N_T);
      break;
    case 5:
      s_stack.push(N_E_PRIME);
      s_stack.push(ADD);
      s_stack.push(N_T);
      s_stack.push(T_PLUS);
      break;
    case 6:
      s_stack.push(N_E_PRIME);
      s_stack.push(SUB);
      s_stack.push(N_T);
      s_stack.push(T_MINUS);
      break;
    case 8:
      s_stack.push(N_T_PRIME);
      s_stack.push(N_F);
      break;
    case 9:
      s_stack.push(N_T_PRIME);
      s_stack.push(MUL);
      s_stack.push(N_F);
      s_stack.push(T_MUL);
      break;
    case 10:
      s_stack.push(N_T_PRIME);
      s_stack.push(DIV);
      s_stack.push(N_F);
      s_stack.push(T_DIV);
      break;
    case 12:
      s_stack.push(PUSHM);
      s_stack.push(T_ID);
      break;
    case 13:
      s_stack.push(T_R_PARENS);
      s_stack.push(N_E);
      s_stack.push(T_L_PARENS);
      break;
    case 14:
      s_stack.push(T_S_COLON);
      s_stack.push(N_M);
      s_stack.push(T_ID);
      s_stack.push(N_TYPE);
      break;
    case 16:
      s_stack.push(T_INT);
      break;
    case 17:
      s_stack.push(T_FLOAT);
      break;
    case 18:
      s_stack.push(T_BOOL);
      break;
    case 19:
      s_stack.push(N_M);
      s_stack.push(T_ID);
      s_stack.push(T_COMMA);
      break;
    case 21:
      s_stack.push(T_ENDIF);
      s_stack.push(N_S);
      s_stack.push(T_ELSE);
      s_stack.push(N_S);
      s_stack.push(T_THEN);
      s_stack.push(N_C);
      s_stack.push(T_IF);
      break;
    case 22:
      s_stack.push(T_ENDIF);
      s_stack.push(N_S);
      s_stack.push(T_DO);
      s_stack.push(N_E);
      s_stack.push(T_WHILE);
      break;
    case 23:
      s_stack.push(T_END);
      s_stack.push(N_MS);
      s_stack.push(N_S);
      s_stack.push(T_BEGIN);
      break;
    case 24:
      s_stack.push(N_MS);
      s_stack.push(N_S);
      s_stack.push(T_S_COLON);
      break;
    case 26:
      s_stack.push(N_E);
      s_stack.push(N_R);
      s_stack.push(N_E);
      break;
    case 28:
      s_stack.push(T_L_BRACK);
      break;
    case 29:
      s_stack.push(T_R_BRACK);
      break;
    case 30:
      s_stack.push(PUSHI);
      s_stack.push(T_NUM);
      break;
    default:
      cout << "Error: Big Oops.\n";
      break;
  }
}

Symbols tok_to_sym(token tok){
  if(tok.tok_type == 2){ return T_ID; }
  if(tok.lex == "="){ return T_EQ; }
  if(tok.lex == "+"){ return T_PLUS; }
  if(tok.lex == "-"){ return T_MINUS; }
  if(tok.lex == "*"){ return T_MUL; }
  if(tok.lex == "/"){ return T_DIV; }
  if(tok.lex == "("){ return T_L_PARENS; }
  if(tok.lex == ")"){ return T_R_PARENS; }
  if(tok.lex == ";"){ return T_S_COLON; }
  if(tok.lex == ","){ return T_COMMA; }
  if(tok.lex == "int"){ return T_INT; }
  if(tok.lex == "float"){ return T_FLOAT; }
  if(tok.lex == "bool"){ return T_BOOL; }
  if(tok.lex == "if"){ return T_IF; }
  if(tok.lex == "then"){ return T_THEN; }
  if(tok.lex == "else"){ return T_ELSE; }
  if(tok.lex == "endif"){ return T_ENDIF; }
  if(tok.lex == "do"){ return T_DO; }
  if(tok.lex == "while"){ return T_WHILE; }
  if(tok.lex == "whileend"){ return T_WHILEEND; }
  if(tok.lex == "begin"){ return T_BEGIN; }
  if(tok.lex == "end"){ return T_END; }
  if(tok.lex == "<"){ return T_L_BRACK; }
  if(tok.lex == ">"){ return T_R_BRACK; }
  if(tok.tok_type == 3){ return T_NUM; }
  return T_ERR;
}

vector<token> getStatement(vector<token> &statement){
  while(stream_ptr != tok_stream.size() && tok_stream[stream_ptr].lex != ";"){
    statement.push_back(tok_stream[stream_ptr++]);
  }
  if(stream_ptr != tok_stream.size()){
    statement.push_back(tok_stream[stream_ptr++]);
  }
  return statement;
}

void print_instr_table(){
  string operand = "";
  cout << left << setw(15) << "Address" << setw(15) << "Operation" << setw(15) << "Operand" << endl;
  outFile << left << setw(15) << "Address" << setw(15) << "Operation" << setw(15) << "Operand" << endl;
  for(int x = 0; x < instr_table.size(); x++){
    i_table_symbol ts = instr_table[x];
    operand = ts.operand;
    if(ts.op != "PUSHI" && ts.op != "PUSHM" && ts.op!= "POPM"){ operand = ""; }
    cout << left << setw(15) << ts.address << setw(15) << ts.op << setw(15) << operand << endl;
    outFile << left << setw(15) << ts.address << setw(15) << ts.op << setw(15) << operand << endl;
  }
  cout << endl;
  outFile << endl;
}

void add_instr_table(Symbols TOS){
  i_table_symbol ts;
  switch(TOS){
    case PUSHI:
    ts.address = instr_address++;
    ts.op = "PUSHI";
    ts.operand = to_string(addresses.top());
    instr_table.push_back(ts);
    addresses.pop();
    break;
    case PUSHM:
    ts.address = instr_address++;
    ts.op = "PUSHM";
    ts.operand = to_string(operand.top());
    instr_table.push_back(ts);
    operand.pop();
    break;
    case POPM:
    ts.address = instr_address++;
    ts.op = "POPM";
    ts.operand = to_string(operand.top());
    instr_table.push_back(ts);
    operand.pop();
    break;
    case ADD:
    ts.address = instr_address++;
    ts.op = "ADD";
    ts.operand = "nil";
    instr_table.push_back(ts);
    break;
    case SUB:
    ts.address = instr_address++;
    ts.op = "SUB";
    ts.operand = "nil";
    instr_table.push_back(ts);
    break;
    case MUL:
    ts.address = instr_address++;
    ts.op = "MUL";
    ts.operand = "nil";
    instr_table.push_back(ts);
    break;
    case DIV:
    ts.address = instr_address++;
    ts.op = "DIV";
    ts.operand = "nil";
    instr_table.push_back(ts);
    break;
    default:
    break;
  }
}

void print_symbol_table(){
  cout << left << setw(25) << "Identifier" << setw(35) << "Memory Location" << setw(25) << "Type" << endl;
  outFile << left << setw(25) << "Identifier" << setw(35) <<"Memory Location" << setw(25) << "Type" << endl;
  for(int x = 0; x < symbol_table.size(); x++){
    s_table_symbol ts = symbol_table[x];
    cout << left << setw(25) << ts.tok.lex << setw(35) << ts.memory << setw(25) << ts.type << endl;
    outFile << left << setw(25) << ts.tok.lex << setw(35) << ts.memory << setw(25) << ts.type << endl;
  }
}

void add_symbol_table(token tok){
  if(check_symbol_table(tok)){
    s_table_symbol ts;
    ts.tok = tok;
    ts.memory = MEMORY_ADDRESS++;
    operand.push(ts.memory);
    if(recent_type == 1){ ts.type = "integer"; }
    else if(recent_type == 2){ ts.type = "float"; }
    else if(recent_type == 3){ ts.type = "boolean"; }
    else{ ts.type = "error"; }
    symbol_table.push_back(ts);
  }
  else{
    operand.push(symbol_table_lookup(tok));
  }
}

bool check_symbol_table(token tok){
  for(int x = 0; x < symbol_table.size(); x++){
    if(tok.lex == symbol_table[x].tok.lex){
      return false;
    }
  }
  return true;
}

int symbol_table_lookup(token tok){
  for(int x = 0; x < symbol_table.size(); x++){
    if(tok.lex == symbol_table[x].tok.lex){
      return symbol_table[x].memory;
    }
  }
  return -1;
}
