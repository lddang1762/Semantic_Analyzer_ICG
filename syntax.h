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

  T_ERR // error
};

stack<Symbols> s_stack;

enum Operations{
  PUSHI,
  PUSHM,
  POPM,
  ADD,
  SUB,
  MUL,
  DIV
};

struct i_table_symbol{
  int address;
  Operations op;
  int operand;
};

vector<i_table_symbol> instr_table;

struct s_table_symbol{
  token tok;
  int memory;
  string type;
};

vector<s_table_symbol> symbol_table;
s_table_symbol save;

void parse(vector<token> &statement);
void rule(int ruleNum, token tok);
Symbols tok_to_sym(token tok);
void print_instr_table();
void add_instr_table();
void gen_instr(int ruleNum);
void print_symbol_table();
void add_symbol_table(token tok);
bool check_symbol_table(token tok);

void parse(vector<token> &statement){
  int statement_ptr = 0;
  //bool print = true;
  s_stack.push(N_S);
  while(!s_stack.empty()){
    Symbols TOS = s_stack.top();
    token i = statement[statement_ptr];
    if(i.lex == "int"){ recent_type = 1; }
    else if(i.lex == "float"){ recent_type = 2; }
    else if(i.lex == "bool"){ recent_type = 3; }
    // if(print){
    //   cout << endl;
    //   //outFile << endl;
    //   i.print();
    //   print = false;
    // }
    if(TOS >= 13 && TOS <= 38){ // if terminal
      if(TOS == tok_to_sym(i)){
        s_stack.pop();
        statement_ptr++;
        //print = true;
      }
      else{
        cout << TOS << ", " <<  tok_to_sym(i)- 13 << endl;
        string err = "Error: Expected token not found.\n";
        throw err;
        break;
      }
    }
    else{
      if(parse_table[TOS][tok_to_sym(i) - 13] != -1){ // subtract 13 in order to match table index
        s_stack.pop();
        // if(parse_table[TOS][tok_to_sym(i) - 13] == 0){
        //   if(TOS == N_E_PRIME){ cout << "\t<Expression Prime> -> "; outFile << "\t<Expression Prime> -> "; }
        //   else if(TOS == N_T_PRIME){ cout << "\t<Term Prime> -> "; outFile << "\t<Term Prime> -> "; }
        //   else if(TOS == N_D){ cout << "\t<Declarative> -> "; outFile << "\t<Declarative> -> "; }
        //   else if(TOS == N_M){ cout << "\t<MoreIds> -> "; outFile << "\t<MoreIds> -> "; }
        //   else if(TOS == N_MS){ cout << "\t<MoreStatements> -> "; outFile << "\t<MoreStatements> -> "; }
        // }
        rule(parse_table[TOS][tok_to_sym(i) - 13], i);
        if(i.tok_type == 2){
          if(i.lex != "true" && i.lex != "false"){
            add_symbol_table(i);
          }
          // if(symbol_table.find(i.lex) == symbol_table.end()){
          //   symbol_table[i.lex] = MEMORY_ADDRESS++;
          // }
        }
        gen_instr(parse_table[TOS][tok_to_sym(i) - 13]);
      }
      else{
        string err = "Error: Incorrect syntax. 2\n";
        // cout << TOS << endl;
        // cout << tok_to_sym(i) - 13 << endl;
        throw err;
        break;
      }
    }
  }
}

void print_instr_table(){
  string op = "", operand = "";
  for(int x = 0; x < instr_table.size(); x++){
    i_table_symbol ts = instr_table[x];
    operand = to_string(ts.operand);
    if(ts.op == PUSHI ){ op = "PUSHI"; }
    else if(ts.op == PUSHM ){ op = "PUSHM"; }
    else if(ts.op == POPM ){ op = "POPM"; }
    else if(ts.op == ADD ){ op = "ADD"; operand = "";}
    else if(ts.op == SUB ){ op = "SUB"; operand = ""; }
    else if(ts.op == MUL ){ op = "MUL"; operand = ""; }
    else if(ts.op == DIV ){ op = "DIV"; operand = ""; }
    cout << left << setw(10) << ts.address<< setw(15) << op << setw(15) << operand << endl;
    outFile << left << setw(10) << ts.address<< setw(15) << op << setw(15) << operand << endl;
  }
  cout << endl;
  outFile << endl;
}

void add_instr_table(){
  i_table_symbol ts;
}

void gen_instr(int ruleNum){
  i_table_symbol ts;
  // int addr, operand;
  // Operations op;

  switch(ruleNum){
    case 3:
     // addr = instr_address++;
     // op = POPM;
     // operand = save.memory;
     ts.address = instr_address++;
     ts.op = POPM;
     ts.operand = save.memory;
     instr_table.push_back(ts);
    break;

    case 5:
    ts.address = instr_address++;
    ts.op = ADD;
    ts.operand = save.memory;
    instr_table.push_back(ts);
    break;

    case 6:
    ts.address = instr_address++;
    ts.op = SUB;
    ts.operand = save.memory;
    instr_table.push_back(ts);
    break;

    case 9:
    ts.address = instr_address++;
    ts.op = MUL;
    ts.operand = save.memory;
    instr_table.push_back(ts);
    break;

    case 10:
    ts.address = instr_address++;
    ts.op = DIV;
    ts.operand = save.memory;
    instr_table.push_back(ts);
    break;

    case 12:
    ts.address = instr_address++;
    ts.op = PUSHM;
    ts.operand = save.memory;
    instr_table.push_back(ts);
    break;

    case 30:
    ts.address = instr_address++;
    ts.op = PUSHI;
    ts.operand = 0;
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
    if(recent_type == 1){ ts.type = "integer"; }
    else if(recent_type == 2){ ts.type = "float"; }
    else if(recent_type == 3){ ts.type = "boolean"; }
    else{ ts.type = "error"; }
    symbol_table.push_back(ts);
    save = ts;
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

void rule(int ruleNum, token tok){
  switch(ruleNum){
    case 0:
    // cout << "<Epsilon>\n";
    // outFile << "<Epsilon>\n";
    break;
    case 1:
      s_stack.push(N_A);
      // cout << "\t<Statement> -> <Assign>\n";
      // outFile << "\t<Statement> -> <Assign>\n";
      break;
    case 2:
      s_stack.push(N_D);
      // cout << "\t<Statement> -> <Declarative>\n";
      // outFile << "\t<Statement> -> <Declarative>\n";
      break;
    case 3:
      s_stack.push(T_S_COLON);
      s_stack.push(N_E);
      s_stack.push(T_EQ);
      s_stack.push(T_ID);
      // cout << "\t<Assign> -> <Identifier> = <Expression> ;\n";
      // outFile << "\t<Assign> -> <Identifier> = <Expression> ;\n";
      break;
    case 4:
      s_stack.push(N_E_PRIME);
      s_stack.push(N_T);
      // cout << "\t<Expression> -> <Term> <Expression Prime>\n";
      // outFile << "\t<Expression> -> <Term> <Expression Prime>\n";
      break;
    case 5:
      s_stack.push(N_E_PRIME);
      s_stack.push(N_T);
      s_stack.push(T_PLUS);
      // cout << "\t<Expression Prime> -> + <Term> <Expression Prime>\n";
      // outFile << "\t<Expression Prime> -> + <Term> <Expression Prime>\n";
      break;
    case 6:
      s_stack.push(N_E_PRIME);
      s_stack.push(N_T);
      s_stack.push(T_MINUS);
      // cout << "\t<Expression Prime> -> - <Term> <Expression Prime>\n";
      // outFile << "\t<Expression Prime> -> - <Term> <Expression Prime>\n";
      break;
    case 8:
      s_stack.push(N_T_PRIME);
      s_stack.push(N_F);
      // cout << "\t<Term> -> <Factor> <Term Prime>\n";
      // outFile << "\t<Term> -> <Factor> <Term Prime>\n";
      break;
    case 9:
      s_stack.push(N_T_PRIME);
      s_stack.push(N_F);
      s_stack.push(T_MUL);
      // cout << "\t<Term Prime> -> * <Factor> <Term Prime>\n";
      // outFile << "\t<Term Prime> -> * <Factor> <Term Prime>\n";
      break;
    case 10:
      s_stack.push(N_T_PRIME);
      s_stack.push(N_F);
      s_stack.push(T_DIV);
      // cout << "\t<Term Prime> -> / <Factor> <Term Prime>\n";
      // outFile << "\t<Term Prime> -> / <Factor> <Term Prime>\n";
      break;
    case 12:
      s_stack.push(T_ID);
      // cout << "\t<Factor> -> <Identifier>\n";
      // outFile << "\t<Factor> -> <Identifier>\n";
      break;
    case 13:
      s_stack.push(T_R_PARENS);
      s_stack.push(N_E);
      s_stack.push(T_L_PARENS);
      // cout << "\t<Factor> -> ( <Expression> )\n";
      // outFile << "\t<Factor> -> ( <Expression> )\n";
      break;
    case 14:
      s_stack.push(T_S_COLON);
      s_stack.push(N_M);
      s_stack.push(T_ID);
      s_stack.push(N_TYPE);
      //tok.print();
      // cout << "\t<Declarative> -> <Type> <Identifier> <MoreIds>;\n";
      // outFile << "\t<Declarative> -> <Type> <Identifier> <MoreIds>;\n";
      break;
    case 16:
      s_stack.push(T_INT);
      //recent_type = 0;
      // cout << "\t<Type> -> int\n";
      // outFile << "\t<Type> -> int\n";
      break;
    case 17:
      s_stack.push(T_FLOAT);
      //recent_type = 1;
      // cout << "\t<Type> -> float\n";
      // outFile << "\t<Type> -> float\n";
      break;
    case 18:
      s_stack.push(T_BOOL);
      //recent_type = 2;
      // cout << "\t<Type> -> bool\n";
      // outFile << "\t<Type> -> bool\n";
      break;
    case 19:
      s_stack.push(N_M);
      s_stack.push(T_ID);
      s_stack.push(T_COMMA);
      // cout << "\t<MoreIds> -> , <Identifier> <MoreIds>\n";
      // outFile << "\t<MoreIds> -> , <Identifier> <MoreIds>\n";
      break;
    case 21:
      s_stack.push(T_ENDIF);
      s_stack.push(N_S);
      s_stack.push(T_ELSE);
      s_stack.push(N_S);
      s_stack.push(T_THEN);
      s_stack.push(N_C);
      s_stack.push(T_IF);
      // cout << "\t<Statement> -> if <Conditional> then <Statement> else <Statement> endif\n";
      // outFile << "\t<Statement> -> if <Conditional> then <Statement> else <Statement> endif\n";
      break;
    case 22:
      s_stack.push(T_ENDIF);
      s_stack.push(N_S);
      s_stack.push(T_DO);
      s_stack.push(N_E);
      s_stack.push(T_WHILE);
      // cout << "\t<Statement> -> while <Expression> do <Statement> whileend\n";
      // outFile << "\t<Statement> -> while <Expression> do <Statement> whileend\n";
      break;
    case 23:
      s_stack.push(T_END);
      s_stack.push(N_MS);
      s_stack.push(N_S);
      s_stack.push(T_BEGIN);
      // cout << "\t<Statement> -> begin <Statement> <MoreStatements> end\n";
      // outFile << "\t<Statement> -> begin <Statement> <MoreStatements> end\n";
      break;
    case 24:
      s_stack.push(N_MS);
      s_stack.push(N_S);
      s_stack.push(T_S_COLON);
      // cout << "\t<MoreStatements> -> ; <Statement> <MoreStatements>\n";
      // outFile << "\t<MoreStatements> -> ; <Statement> <MoreStatements>\n";
      break;
    case 26:
      s_stack.push(N_E);
      s_stack.push(N_R);
      s_stack.push(N_E);
      // cout << "\t<Conditional> -> <Expression> <Relop> <Expression>\n";
      // outFile << "\t<Conditional> -> <Expression> <Relop> <Expression>\n";
      break;
    case 28:
      s_stack.push(T_L_BRACK);
      // cout << "\t<Relop> -> <\n";
      // outFile << "\t<Relop> -> <\n";
      break;
    case 29:
      s_stack.push(T_R_BRACK);
      // cout << "\t<Relop> -> > \n";
      // outFile << "\t<Relop> -> > \n";
      break;
    case 30:
      s_stack.push(T_NUM);
      // cout << "\t<Factor> -> <Number>\n";
      // outFile << "\t<Factor> -> <Number>\n";
      break;
    // case 31:
    //   s_stack.push(T_TF);
    //   cout << "\t<Factor> -> <True/False>n";
    //   outFile << "\t<Factor> -> <True/False>n";
    //   break;
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
