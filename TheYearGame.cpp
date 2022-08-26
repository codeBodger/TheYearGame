#include <iostream> //cout
#include <fstream> //ifstream, ofstream
#include <string> //stoi, etc
#include <ctime> //timeing the program
#include <cmath>

#include <math.h> //pow, sqrt ??
#include <stack> //stacks in eval

const int b = 15;
std::string dic[b] = {
  " + ", // 0
  " - ", // 1
  " * ", // 2
  " / ", // 3
  " ^ ", // 4
  "( ",  // 5
  " )",  // 6
  "2",   // 7
  "0",   // 8
  "2",   // 9
  "2",   //10
  " -",   //11
  " √",   //12
  "! ",  //13
  "!! "  //14
};

int digits = 4;
int digits_ = 3;

int* indices;

std::string equList[101];
int badnessList[101];

int yearDigits[4] = {2,0,2,2};

double fout;
double dout;
double fact(double val) {
  if (val > 128) { return INFINITY; }
  if (trunc(val) != val) { return INFINITY; }
  if (val == 0) { return 1; }
  fout = 1;
  while(val > 0)
  {
    fout = fout * val;
    val--;
  }
  return fout;
}
double dfac(double val) {
  if (val > 256) { return INFINITY; }
  if (trunc(val) != val) { return INFINITY; }
  if (val == 0) { return 1; }
  dout = 1;
  while(val > 0)
  {
    dout = dout * val;
    val--; val--;
  }
  return dout;
}

int precedence(int op) {
  if (op <= 1) { return 1; } //+-
  if (op <= 3) { return 2; } //*/
  if (op <= 4) { return 3; } //^
  if (op >=11) { return 4; } //monary operators
  return 0;
}

double eval1;
double eval2;
int op;
double applyOp() {
  // cout << "  " << eval1 << " " << op << " " << eval2 << "\n";

  switch (op) {
    case 0: return eval1 + eval2;
    case 1: return eval1 - eval2;
    case 2: return eval1 * eval2;
    case 3: return eval1 / eval2;
    case 4: return pow(eval1,eval2);

    case 11: return -eval1;
    case 12: return sqrt(eval1);
    case 13: return fact(eval1);
    case 14: return dfac(eval1);
  }
  return INFINITY;
}

int l;
int nval;
double eval() {
  std::stack <double> values;
  std::stack <int> ops;
  
  for (l = 0; l < digits; l++) {
    // cout << l << "\n";
    
    // Current token is an opening
    // brace, push it to 'ops'
    if(indices[l] == 5) {
      ops.push(indices[l]);
    }
    
    // Current token is a number, push
    // it to stack for numbers.
    else if(indices[l]>=7 && indices[l]<=10) {
      nval = 0;
       
      // There may be more than one
      // digits in number.
      while(l < digits && indices[l]>=7 && indices[l]<=10)
      {
        nval = (nval*10) + yearDigits[indices[l] -7];
        l++;
      }
       
      values.push(nval);
       
      // right now the l points to
      // the character next to the digit,
      // since the for loop also increases
      // the l, we would skip one
      //  token position; we need to
      // decrease the value of l by 1 to
      // correct the offset.
      l--;
    }
     
    // Closing brace encountered, solve
    // entire brace.
    else if(indices[l] == 6)
    {
      while(!ops.empty() && ops.top() != 5)
      {
        if (ops.top() <= 4)
        {
          eval2 = values.top();
          values.pop();
          
          eval1 = values.top();
          values.pop();
          
          op = ops.top();
          ops.pop();
          
          values.push(applyOp());
        }
        else
        {
          eval2 = 0;

          eval1 = values.top();
          values.pop();
          
          op = ops.top();
          ops.pop();
          
          values.push(applyOp());
        }
      }
       
      // pop opening brace.
      if(!ops.empty())
         ops.pop();
    }
    
    // Current token is fact/dfact
    else if (indices[l] >= 13)
    {
      eval2 = 0;

      eval1 = values.top();
      values.pop();

      op = indices[l];

      values.push(applyOp());
    }

    // Current token is -/sqrt
    else if (indices[l] >= 11)
    {
      ops.push(indices[l]);
    }

    // Current token is a binary operator.
    else
    {
      // While top of 'ops' has same or greater
      // precedence to current token, which
      // is an operator. Apply operator on top
      // of 'ops' to top two elements in values stack.
      while(!ops.empty() && precedence(ops.top()) >= precedence(indices[l])) {
        if (ops.top() <= 4)
        {
          eval2 = values.top();
          values.pop();
          
          eval1 = values.top();
          values.pop();
          
          op = ops.top();
          ops.pop();
          
          values.push(applyOp());
        }
        else
        {
          eval2 = 0;

          eval1 = values.top();
          values.pop();
          
          op = ops.top();
          ops.pop();
          
          values.push(applyOp());
        }
      }
       
      // Push current token to 'ops'.
      ops.push(indices[l]);
    }
  }
   
  // Entire expression has been parsed at this
  // point, apply remaining ops to remaining
  // values.
  while(!ops.empty())
  {
    if (ops.top() <= 4)
    {
      eval2 = values.top();
      values.pop();

      eval1 = values.top();
      values.pop();

      op = ops.top();
      ops.pop();
         
      values.push(applyOp());
    }
    else
    {
      eval2 = 0;

      eval1 = values.top();
      values.pop();
      
      op = ops.top();
      ops.pop();
      
      values.push(applyOp());
    }
  }
   
  // Top of 'values' contains result, return it.
  return values.top();
}


template <typename T> int sgn(T num) {
  return (T(0) < num) - (num < T(0));
}

int errAt;
int now;
int nums[4];
int opens = 0;
int closeds = 0;
int next;
bool nextBad;
int i;
int invalidAt() { //Function to see if the un-decoded array follows the rules and is probably executable

  errAt = digits; //The place in the array where the error is
  next = indices[0];
  now = next;
  //Dump for illegal first digits
  if (now ==  0) { return 0; } // + 
  if (now ==  1) { return 0; } // - 
  if (now ==  2) { return 0; } // * 
  if (now ==  3) { return 0; } // / 
  if (now ==  4) { return 0; } // ^ 
  if (now ==  6) { return 0; } // ) 
  if (now == 13) { return 0; } //! 
  if (now == 14) { return 0; } //!! 

  nums[0] = 0; nums[1] = 0; nums[2] = 0; nums[3] = 0;
  opens = 0;
  closeds = 0;
  nextBad = false;
  for (i = 0; i < digits_; i++)
  {
    if (i == errAt) { return errAt; }
    now = next;
    next = indices[i+1];
    switch(now) {
    
      case  0: //+
      case  1: //-
      case  2: //*
      case  3: ///
      case  4:  //^
        if (next ==  0) { nextBad = true; break; } // +
        if (next ==  1) { nextBad = true; break; } // -
        if (next ==  2) { nextBad = true; break; } // *
        if (next ==  3) { nextBad = true; break; } // /
        if (next ==  4) { nextBad = true; break; } // ^
        if (next ==  6) { nextBad = true; break; } // )
        if (next == 13) { nextBad = true; break; } // .f()
        if (next == 14) { nextBad = true; break; } // .d()
      break;
      
      case  5: //(
        opens++;
        if (next ==  0) { nextBad = true; break; } // +
        if (next ==  1) { nextBad = true; break; } // -
        if (next ==  2) { nextBad = true; break; } // *
        if (next ==  3) { nextBad = true; break; } // /
        if (next ==  4) { nextBad = true; break; } // ^
        if (next ==  6) { nextBad = true; break; } // )
        if (next == 13) { nextBad = true; break; } // .f()
        if (next == 14) { nextBad = true; break; } // .d()
      break;
      
      case  6: //)
        closeds++;
        if (next ==  5) { nextBad = true; break; } // (
        if (next>=7 && next<=10) { nextBad = true; break; } // nums
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;
      
      case  7: //T
        nums[0]++;
        if (next ==  5) { nextBad = true; break; } // (
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;

      case  8: //Z
        nums[1]++;
        if (next ==  5) { nextBad = true; break; } // (
        if (next>=7 && next!=8 && next<=10) {      // T
          if (i == 0) { return 1; } //To avoid StringIndexOutOfBoundsException on next line
          if (indices[i-1] !=  8) { nextBad = true; break; } //special case for only if the preceding digit is also 2
        }
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;

      case  9: //T
        nums[2]++;
        if (next ==  5) { nextBad = true; break; } // (
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;

      case 10: //T
        nums[3]++;
        if (next ==  5) { nextBad = true; break; } // (
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;
      
      case 11:  //-
      case 12:  //s()
        if (next ==  0) { nextBad = true; break; } // +
        if (next ==  1) { nextBad = true; break; } // -
        if (next ==  2) { nextBad = true; break; } // *
        if (next ==  3) { nextBad = true; break; } // /
        if (next ==  4) { nextBad = true; break; } // ^
        if (next ==  6) { nextBad = true; break; } // )
        if (next == 11) { nextBad = true; break; } // -
        if (next == 13) { nextBad = true; break; } //.f()
        if (next == 14) { nextBad = true; break; } //.d()
      break;

      case 13:  //.f()
      case 14:  //.d()
        if (next ==  5) { nextBad = true; break; } // (
        if (next>=7 && next<=10) { nextBad = true; break; } // nums
        if (next == 11) { nextBad = true; break; } // -
        if (next == 12) { nextBad = true; break; } // sqrt
      break;
    }
    
    if (closeds > opens) { return i; }
    if (nums[0] > 1    ) { return i; }
    if (nums[1] > 1    ) { return i; }
    if (nums[2] > 1    ) { return i; }
    if (nums[3] > 1    ) { return i; }
    if (nextBad) { return i+1; }
  }

  switch(next) {
    case  5: //(
      opens++;
    break;

    case  6: //)
      closeds++;
    break;
    
    case  7: //T
    case  8: //Z
    case  9: //T
    case 10: //T
      nums[next-7]++;
    break;
  }
  if (closeds > opens) { return digits_; }
  if (nums[0] != 1   ) { return digits_; }
  if (nums[1] != 1   ) { return digits_; }
  if (nums[2] != 1   ) { return digits_; }
  if (nums[3] != 1   ) { return digits_; }

  errAt = std::min(errAt, digits-sgn(opens-closeds)); //these only dump last vals
  errAt = std::min(errAt, digits-sgn(nums[0] -1)); //these might be able to work in a better way
  errAt = std::min(errAt, digits-sgn(nums[1] -1));
  errAt = std::min(errAt, digits-sgn(nums[2] -1));
  errAt = std::min(errAt, digits-sgn(nums[3] -1));

  //Keep only for legal last digits
  now = indices[digits_];
  if (now !=  5) { //)
  if (now !=  6) { //0
  if (now !=  7) { //2
  if (now !=  8) { //.s()
  if (now !=  9) { //.f()
  if (now != 10) { //.d()
    errAt = std::min(errAt, digits_); }}}}}}
  
  return errAt;
}

bool order;
bool doubles;
bool Nums[4];
int k;
int badness() {
  order = false;
  doubles = false;
  Nums[0] = false; Nums[1] = false; Nums[2] = false; Nums[3] = false;
  for (k = 0; k < digits_; k++)
  {
    switch (indices[k])
    {
      case  7:
        if (indices[k+1] >= 7 && indices[k+1] <= 10)
          doubles = true;
        Nums[0] = true;
      break;
      
      case  8:
        if (indices[k+1] >= 7 && indices[k+1] <= 10)
          doubles = true;
        if (!Nums[0])
          order = true;
        Nums[1] = true;
      break;
      
      case  9:
        if (indices[k+1] >= 7 && indices[k+1] <= 10)
          doubles = true;
        if (!Nums[1])
          order = true;
        Nums[2] = true;
      break;
      
      case 10:
        if (indices[k+1] >= 7 && indices[k+1] <= 10)
          doubles = true;
        if (!Nums[2])
          order = true;
        Nums[3] = true;
      break;
    }
  }

  return int(order) + int(doubles);
}


int inv;
double val = -1;
int ival;
int bad;
std::string equ;
int j;
int act() {
  inv = invalidAt();
  if (inv < digits) { return inv; }
  
  val = eval();
  if (val >= 0 && val <= 100) {
    ival = trunc(val);
  if (ival == val) {
    bad = badness();
  // std::cout << "  hi\n";
  if (bad < badnessList[ival]) {
    equ = "";
    for (j = 0; j < digits; j++) equ = equ + dic[indices[j]];
    equList[ival] = "[" + std::to_string(ival) + "]\t[" + std::to_string(bad) + "]\t" + equ;
    badnessList[ival] = bad;
  }}}

  return digits;
}
int ret;
int level;
void print() {
  std::cout << level << ":";
  for (int asdf = 0; asdf < digits; asdf++)
    std::cout << "\t" << indices[asdf];
  std::cout << "\n";
}
void nFor() {
  ret = digits;
  level = digits_;
  while (true)
  {
    // print();
    if (level == 0 && indices[0] == b) return;
    if (indices[level] < b)
    {
      if (ret < level) {
        indices[level] = 0;
        level = ret;
        ret = digits;
        indices[level]++;
        continue;
      }
      level++;
      if (level == digits)
      {
        ret = act();
        level--;
        indices[level]++;
      }
    }
    else
    {
      ret = digits;
      indices[level] = 0;
      level--;
      indices[level]++;
    }
  }
}

int m;
std::string badnessTemp = "5";
void loadData() {
  std::ifstream data("data.txt");
  std::ifstream badn("badn.txt");
  if (data.is_open() && badn.is_open())
  {
    for (m = 0; m<=100; m++)
    {
      getline (data,equList[m]);
      getline (badn,badnessTemp);
      badnessList[m] = stoi(badnessTemp);
    }
    data.close();
    badn.close();
  }
  else std::cout << "Unable to load data\n"; 
}
void saveData() {
  std::ofstream data("data.txt");
  std::ofstream badn("badn.txt");
  if (data.is_open() && badn.is_open())
  {
    for (m = 0; m<=100; m++)
    {
      data << equList[m];
      data << "\n";
      badn << badnessList[m];
      badn << "\n";
    }
    data.close();
    badn.close();
  }
  else std::cout << "Unable to save data\n";
}


int mytime; // The time in seconds
int n;
void draw() {
  mytime = clock() / CLOCKS_PER_SEC;
  loadData();
  indices = new int[digits];
  for (n = 0; n < digits; n++) {
    indices[n] = 0;
  }
  std::cout << digits << "\n";

  std::cout << "  Analysing ...\n";

  nFor();

  std::cout << "  Saving ...\n";
  saveData();

  mytime = clock() / CLOCKS_PER_SEC - mytime;
  std::cout << "  Done    " << mytime << "\n";

  delete[] indices;
  digits_ = digits;
  digits++;
}


int main() {
  // system("clear");
  std::cout << "Start\n";

  while (true)
    draw();

  std::cout << "Done\n";
return 0;}
