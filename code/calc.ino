/*

///////////////////////Funkctions:///////////////////////

String calculate(String expr[], String Ans, int angle_unit, bool defalutFraction = true) {
  return "";
}

String roundResult(String Result, int display_parameter, int display_mode) {
  return "";
}

*/

#include <math.h>
#include <stdlib.h>

enum ResultType { REAL, FRACTION, DMS };
ResultType currentResultType = REAL;

String rawTokens[100];
String tokens[150];
String outputQueue[150];
String opStack[150];
double evalStack[50];

long gcd(long a, long b) {
  while (b != 0) {
    long t = b;
    b = a % b;
    a = t;
  }
  return abs(a);
}

double parseDMS(String s);

double parseAnsValue(String Ans) {
  if (Ans.length() == 0 || Ans == "Syntax ERROR" || Ans == "Math ERROR") return 0.0;
  Ans.trim();
  
  if (Ans.indexOf(',') != -1) {
    return parseDMS(Ans);
  }
  
  if (Ans.indexOf('/') != -1) {
    int firstSlash = Ans.indexOf('/');
    int secondSlash = Ans.indexOf('/', firstSlash + 1);
    
    if (secondSlash != -1) {
      double whole = Ans.substring(0, firstSlash).toDouble();
      double num = Ans.substring(firstSlash + 1, secondSlash).toDouble();
      double den = Ans.substring(secondSlash + 1).toDouble();
      if (den == 0) return 0.0;
      return (whole >= 0) ? (whole + (num / den)) : (whole - (num / den));
    } else {
      double num = Ans.substring(0, firstSlash).toDouble();
      double den = Ans.substring(firstSlash + 1).toDouble();
      if (den == 0) return 0.0;
      return num / den;
    }
  }
  
  return Ans.toDouble();
}

String decimalToFractionString(double val, bool defaultFraction) {
  if (isnan(val) || isinf(val)) return "Math ERROR";
  
  bool neg = val < 0;
  val = abs(val);
  
  if (abs(val - round(val)) < 1e-9) {
    long cjelobrojni = round(val);
    return neg ? "-" + String(cjelobrojni) : String(cjelobrojni);
  }

  double m[2][2];
  double x = val;
  long max_den = 10000;
  
  m[0][0] = m[1][1] = 1;
  m[0][1] = m[1][0] = 0;
  
  while (m[1][0] * (long)x + m[1][1] <= max_den) {
    long a = (long)x;
    double t = m[0][0] * a + m[0][1];
    m[0][1] = m[0][0]; m[0][0] = t;
    
    t = m[1][0] * a + m[1][1];
    m[1][1] = m[1][0]; m[1][0] = t;
    
    if (abs(x - a) < 1e-9) break;
    x = 1.0 / (x - a);
  }
  
  long num = round(m[0][0]);
  long den = round(m[1][0]);
  
  if (den == 0 || abs(val - ((double)num / den)) > 1e-9) {
    char fallbackBuf[32];
    sprintf(fallbackBuf, "%.14g", neg ? -val : val);
    return String(fallbackBuf);
  }
  
  long g = gcd(num, den);
  if (g != 0) {
    num /= g;
    den /= g;
  }

  if (den == 1) {
    return neg ? "-" + String(num) : String(num);
  }

  String res = "";
  if (neg) res += "-";

  if (defaultFraction && num > den) {
    long cijeliDio = num / den;
    long ostatak = num % den;
    if (ostatak == 0) {
      res += String(cijeliDio);
    } else {
      res += String(cijeliDio) + "/" + String(ostatak) + "/" + String(den);
    }
  } else {
    res += String(num) + "/" + String(den);
  }

  return res;
}

bool isNumber(String s) {
  if (s.length() == 0) return false;
  s.trim();
  if (s == "Ans" || s == "0xad" || s == "0xb1" || s == "Ran#") return true;
  
  if (s.startsWith("0x") && s != "0xad" && s != "0xb1" && s != "0x90" && s.indexOf("0x90") == -1) {
    return false;
  }

  String temp = s;
  while (temp.indexOf("0x90") != -1) {
    int idx = temp.indexOf("0x90");
    temp = temp.substring(0, idx) + temp.substring(idx + 4);
  }

  int start = 0;
  if (temp.length() > 0 && temp[0] == '-') start = 1;
  bool dot = false;
  bool expSeen = false;
  
  if (temp.length() == start) return s.indexOf("0x90") != -1; 

  for (int i = start; i < temp.length(); i++) {
    if (temp[i] == '.') {
      if (dot || expSeen) return false;
      dot = true;
    } else if (temp[i] == 'e' || temp[i] == 'E') {
      if (expSeen || i == start || i == temp.length() - 1) return false;
      expSeen = true;
      if (i + 1 < temp.length() && (temp[i+1] == '+' || temp[i+1] == '-')) {
        i++;
        if (i == temp.length() - 1) return false;
      }
    } else if (!isdigit(temp[i])) {
      return false;
    }
  }
  return true;
}

double toRadians(double val, int unit) {
  if (unit == 0) return val * 3.14159265358979323846 / 180.0;
  if (unit == 2) return val * 3.14159265358979323846 / 200.0;
  return val;
}

double fromRadians(double val, int unit) {
  if (unit == 0) return val * 180.0 / 3.14159265358979323846;
  if (unit == 2) return val * 200.0 / 3.14159265358979323846;
  return val;
}

double parseDMS(String s) {
  double deg = 0, min = 0, sec = 0;
  bool neg = false;
  s.trim();
  if (s.startsWith("-")) {
    neg = true;
    s = s.substring(1);
  }
  
  int idx1 = s.indexOf("0x90");
  if (idx1 != -1) {
    deg = s.substring(0, idx1).toDouble();
    int idx2 = s.indexOf("0x90", idx1 + 4);
    if (idx2 != -1) {
      min = s.substring(idx1 + 4, idx2).toDouble();
      int idx3 = s.indexOf("0x90", idx2 + 4);
      if (idx3 != -1) {
        sec = s.substring(idx2 + 4, idx3).toDouble();
      } else {
        sec = s.substring(idx2 + 4).toDouble();
      }
    } else {
      if (s.substring(idx1 + 4).length() > 0) {
        min = s.substring(idx1 + 4).toDouble();
      }
    }
  } else {
    if (s.indexOf(',') != -1) {
      int c1 = s.indexOf(',');
      int c2 = s.indexOf(',', c1 + 1);
      deg = s.substring(0, c1).toDouble();
      if (c2 != -1) {
        min = s.substring(c1 + 1, c2).toDouble();
        sec = s.substring(c2 + 1).toDouble();
      } else {
        min = s.substring(c1 + 1).toDouble();
      }
    } else {
      deg = s.toDouble();
    }
  }
  double finalVal = deg + (min / 60.0) + (sec / 3600.0);
  return neg ? -finalVal : finalVal;
}

String decimalToDMSString(double val) {
  bool neg = val < 0;
  val = abs(val);
  long deg = (long)val;
  double remMin = (val - deg) * 60.0;
  long min = (long)remMin;
  double remSec = (remMin - min) * 60.0;
  
  double sec = round(remSec * 100.0) / 100.0; 
  
  if (sec >= 60) { sec -= 60; min++; }
  if (min >= 60) { min -= 60; deg++; }
  
  String secStr = String(sec);
  if (secStr.endsWith(".00")) secStr = secStr.substring(0, secStr.length() - 3);
  else if (secStr.indexOf('.') != -1 && secStr.endsWith("0")) secStr.remove(secStr.length() - 1);

  String res = "";
  if (neg) res += "-";
  res += String(deg) + "," + String(min) + "," + secStr;
  return res;
}

bool isFunction(String t) {
  return (t == "sin" || t == "cos" || t == "tan" || t == "log" || t == "In" || 
          t == "0xab" || t == "0xb5" || t == "0xb2" || t == "sin0xaa" || 
          t == "cos0xaa" || t == "tan0xaa" || t == "0x930xab" || t == "Rnd" ||
          t == "sinh" || t == "cosh" || t == "tanh" || t == "asinh" || t == "acosh" || t == "atanh");
}

double evaluateNumber(String s, double Ans, int angle_unit) {
  s.trim();
  if (s == "Ans") return Ans;
  if (s == "0xad") return 3.14159265358979323846;
  if (s == "0xb1") return 2.71828182845904523536;
  if (s == "Ran#") return (double)rand() / (double)RAND_MAX;

  double val = 0;
  if (s.indexOf("0x90") != -1) {
    val = parseDMS(s);
  } else {
    val = s.toDouble();
  }
  return val;
}

double c_factorial(double n) {
  if (n < 0 || n > 69 || floor(n) != n) return -1;
  double res = 1.0;
  for (int i = 1; i <= (int)n; i++) res *= i;
  return res;
}

String calculate(String expr[], String Ans, int angle_unit, bool defaultFraction) {
  currentResultType = REAL; 
  double ansVal = parseAnsValue(Ans);
  
  char ansBuffer[32];
  sprintf(ansBuffer, "%.14e", ansVal);
  String ansStrDec = String(ansBuffer);

  int len = 0;
  while (len < 50 && expr[len] != "") len++;
  if (len == 0) return "0";

  for (int i = 0; i < 100; i++) rawTokens[i] = "";
  for (int i = 0; i < 150; i++) { tokens[i] = ""; outputQueue[i] = ""; opStack[i] = ""; }

  int rCount = 0;
  String tempExpr[100];
  int tLen = 0;
  String currentNum = "";

  for (int i = 0; i < len; i++) {
    String current = expr[i];
    current.trim();
    if (current == "" || current == " ") continue;

    if (current == "Ans") {
      if (currentNum != "") { tempExpr[tLen++] = currentNum; currentNum = ""; }
      tempExpr[tLen++] = ansStrDec;
      continue;
    }
    if (current == "0x90") { currentNum += "0x90"; continue; }

    bool isHexOp = current.startsWith("0x") && current != "0xad" && current != "0xb1";

    if (!isHexOp && (isdigit(current[0]) || current == "." || current == "e" || current == "E")) {
      currentNum += current;
      if (current == "e" || current == "E") {
        if (i + 1 < len && (expr[i+1] == "+" || expr[i+1] == "-")) {
          currentNum += expr[i+1];
          i++;
        }
      }
    } else {
      if (currentNum != "") { tempExpr[tLen++] = currentNum; currentNum = ""; }
      if (current == "0xaf") { currentNum = "-"; continue; }
      tempExpr[tLen++] = current;
    }
  }
  if (currentNum != "") tempExpr[tLen++] = currentNum;

  for (int i = 0; i < tLen; i++) {
    if (i + 4 < tLen && isNumber(tempExpr[i]) && (tempExpr[i+1] == "0xae" || tempExpr[i+1] == "0xaf") && isNumber(tempExpr[i+2]) && (tempExpr[i+3] == "0xae" || tempExpr[i+3] == "0xaf") && isNumber(tempExpr[i+4])) {
      if (!defaultFraction) return "Syntax ERROR";

      double A = evaluateNumber(tempExpr[i], ansVal, angle_unit);
      double B = evaluateNumber(tempExpr[i+2], ansVal, angle_unit);
      double C = evaluateNumber(tempExpr[i+4], ansVal, angle_unit);
      
      if (C == 0) return "Math ERROR";
      double val = (A >= 0) ? (A + (B / C)) : (A - (B / C));
      
      char buf[32];
      sprintf(buf, "%.14g", val);
      rawTokens[rCount++] = String(buf);
      currentResultType = FRACTION;
      i += 4;
    }
    else if (i + 2 < tLen && isNumber(tempExpr[i]) && (tempExpr[i+1] == "0xae" || tempExpr[i+1] == "0xaf") && isNumber(tempExpr[i+2])) {
      double B = evaluateNumber(tempExpr[i], ansVal, angle_unit);
      double C = evaluateNumber(tempExpr[i+2], ansVal, angle_unit);
      
      if (C == 0) return "Math ERROR";
      double val = B / C;
      
      char buf[32];
      sprintf(buf, "%.14g", val);
      rawTokens[rCount++] = String(buf);
      currentResultType = FRACTION;
      i += 2;
    }
    else {
      if (tempExpr[i].indexOf("0x90") != -1) currentResultType = DMS;
      rawTokens[rCount++] = tempExpr[i];
    }
  }

  int tCount = 0;
  int openBrackets = 0;
  int closeBrackets = 0;

  for (int i = 0; i < rCount; i++) {
    String t = rawTokens[i];
    if (t == "(" || t == "Pol(" || t == "Rec(") openBrackets++;
    if (t == ")") closeBrackets++;

    if (tCount > 0) {
      String prev = tokens[tCount - 1];
      if (isNumber(prev) || prev == ")") {
        if (t == "(" || t == "Pol(" || t == "Rec(" || isFunction(t)) {
          tokens[tCount++] = "0xb7"; 
        }
      }
    }
    tokens[tCount++] = t;

    if (isFunction(t)) {
      if (i + 1 < rCount && rawTokens[i + 1] != "(") {
        tokens[tCount++] = "(";
        openBrackets++;
        tokens[tCount++] = rawTokens[i + 1]; 
        i++; 
        tokens[tCount++] = ")";
        closeBrackets++;
      }
    }
  }

  while (openBrackets > closeBrackets) {
    tokens[tCount++] = ")";
    openBrackets--;
  }

  int outIdx = 0;
  int stackIdx = 0;

  for (int i = 0; i < tCount; i++) {
    String t = tokens[i];

    if (isNumber(t)) {
      outputQueue[outIdx++] = t;
    } 
    else if (isFunction(t) || t == "Pol(" || t == "Rec(") {
      opStack[stackIdx++] = t;
    } 
    else if (t == "," || t == ";") {
      while (stackIdx > 0 && opStack[stackIdx - 1] != "(" && opStack[stackIdx - 1] != "Pol(" && opStack[stackIdx - 1] != "Rec(") {
        outputQueue[outIdx++] = opStack[--stackIdx];
      }
      if (stackIdx == 0) return "Syntax ERROR";
    }
    else if (t == "+" || t == "-" || t == "0xb7" || t == "0xd7" || t == "^" || 
             t == "0xb3" || t == "0xb4" || t == "0x92" || t == "0x93" || 
             t == "0xaa" || t == "!" || t == "%" || t == "x0xab" || t == "0xb0" ||
             t == "d" || t == "r" || t == "g") {
      int p = 0;
      bool rAssoc = false;
      if (t == "+" || t == "-") { p = 1; }
      else if (t == "0xb7" || t == "0xd7") { p = 2; } 
      else if (t == "0xb3" || t == "0xb4" || t == "x0xab") { p = 3; } 
      else if (t == "0xb0") { p = 4; rAssoc = true; } 
      else if (t == "^" || t == "0x92" || t == "0x93" || t == "0xaa" || t == "!" || t == "%" || t == "d" || t == "r" || t == "g") { p = 5; rAssoc = true; } 
      
      while (stackIdx > 0) {
        String top = opStack[stackIdx - 1];
        if (top == "(" || top == "Pol(" || top == "Rec(") break;
        
        int topP = 0;
        if (top == "+" || top == "-") topP = 1;
        else if (top == "0xb7" || top == "0xd7") topP = 2;
        else if (top == "0xb3" || top == "0xb4" || top == "x0xab") topP = 3;
        else if (top == "0xb0") topP = 4;
        else if (top == "^" || top == "0x92" || top == "0x93" || top == "0xaa" || top == "!" || top == "%" || top == "d" || top == "r" || top == "g") topP = 5;
        else topP = 6; 

        if ((!rAssoc && p <= topP) || (rAssoc && p < topP)) {
          outputQueue[outIdx++] = opStack[--stackIdx];
        } else {
          break;
        }
      }
      opStack[stackIdx++] = t;
    } 
    else if (t == "(") {
      opStack[stackIdx++] = t;
    } 
    else if (t == ")") {
      while (stackIdx > 0 && opStack[stackIdx - 1] != "(" && opStack[stackIdx - 1] != "Pol(" && opStack[stackIdx - 1] != "Rec(") {
        outputQueue[outIdx++] = opStack[--stackIdx];
      }
      if (stackIdx == 0) return "Syntax ERROR";
      
      String matchingL = opStack[--stackIdx]; 
      if (matchingL == "Pol(" || matchingL == "Rec(") {
        outputQueue[outIdx++] = matchingL;
      }
      
      if (stackIdx > 0) {
        String top = opStack[stackIdx - 1];
        if (isFunction(top)) {
          outputQueue[outIdx++] = opStack[--stackIdx];
        }
      }
    }
  }

  while (stackIdx > 0) {
    if (opStack[stackIdx - 1] == "(" || opStack[stackIdx - 1] == "Pol(" || opStack[stackIdx - 1] == "Rec(") return "Syntax ERROR";
    outputQueue[outIdx++] = opStack[--stackIdx];
  }

  int evIdx = 0;

  for (int i = 0; i < outIdx; i++) {
    String t = outputQueue[i];

    if (isNumber(t)) {
      evalStack[evIdx++] = evaluateNumber(t, ansVal, angle_unit);
    } 
    else {
      if (t == "0x92" || t == "0x93" || t == "0xaa" || t == "!" || t == "%" || t == "d" || t == "r" || t == "g") {
        if (evIdx < 1) return "Syntax ERROR";
        double val = evalStack[--evIdx];
        
        if (t == "0x92") evalStack[evIdx++] = pow(val, 2);
        else if (t == "0x93") evalStack[evIdx++] = pow(val, 3);
        else if (t == "0xaa") evalStack[evIdx++] = pow(val, -1);
        else if (t == "!") {
          double f = c_factorial(val);
          if (f == -1) return "Math ERROR";
          evalStack[evIdx++] = f;
        }
        else if (t == "%") evalStack[evIdx++] = val / 100.0;
        else if (t == "d") {
          double rad = val * 3.14159265358979323846 / 180.0;
          evalStack[evIdx++] = fromRadians(rad, angle_unit);
        }
        else if (t == "r") {
          double rad = val;
          evalStack[evIdx++] = fromRadians(rad, angle_unit);
        }
        else if (t == "g") {
          double rad = val * 3.14159265358979323846 / 200.0;
          evalStack[evIdx++] = fromRadians(rad, angle_unit);
        }
      }
      else if (isFunction(t)) {
        if (evIdx < 1) return "Syntax ERROR";
        double val = evalStack[--evIdx];

        if (t == "sin") { double res = sin(toRadians(val, angle_unit)); if (abs(res) < 1e-9) res = 0; evalStack[evIdx++] = res; }
        else if (t == "cos") { double res = cos(toRadians(val, angle_unit)); if (abs(res) < 1e-9) res = 0; evalStack[evIdx++] = res; }
        else if (t == "tan") { double rad = toRadians(val, angle_unit); if (abs(cos(rad)) < 1e-9) return "Math ERROR"; double res = tan(rad); if (abs(res - 1.0) < 1e-6) res = 1.0; evalStack[evIdx++] = res; }
        
        else if (t == "sinh") evalStack[evIdx++] = sinh(val);
        else if (t == "cosh") evalStack[evIdx++] = cosh(val);
        else if (t == "tanh") evalStack[evIdx++] = tanh(val);
        else if (t == "asinh") evalStack[evIdx++] = asinh(val);
        else if (t == "acosh") { if (val < 1) return "Math ERROR"; evalStack[evIdx++] = acosh(val); }
        else if (t == "atanh") { if (val <= -1 || val >= 1) return "Math ERROR"; evalStack[evIdx++] = atanh(val); }
        
        else if (t == "sin0xaa") { if (val < -1 || val > 1) return "Math ERROR"; evalStack[evIdx++] = fromRadians(asin(val), angle_unit); }
        else if (t == "cos0xaa") { if (val < -1 || val > 1) return "Math ERROR"; evalStack[evIdx++] = fromRadians(acos(val), angle_unit); }
        else if (t == "tan0xaa") { evalStack[evIdx++] = fromRadians(atan(val), angle_unit); }
        else if (t == "log") { if (val <= 0) return "Math ERROR"; evalStack[evIdx++] = log10(val); }
        else if (t == "In") { if (val <= 0) return "Math ERROR"; evalStack[evIdx++] = log(val); }
        else if (t == "0xab") { if (val < 0) return "Math ERROR"; evalStack[evIdx++] = sqrt(val); }
        else if (t == "0x930xab") evalStack[evIdx++] = cbrt(val);
        else if (t == "0xb5") evalStack[evIdx++] = pow(10, val);
        else if (t == "0xb2") evalStack[evIdx++] = exp(val);
        else if (t == "Rnd") evalStack[evIdx++] = round(val * 1e9) / 1e9;
      }
      else if (t == "Pol(" || t == "Rec(") {
        if (evIdx < 2) return "Syntax ERROR";
        double y_p = evalStack[--evIdx]; 
        double x_p = evalStack[--evIdx]; 

        if (t == "Pol(") { evalStack[evIdx++] = sqrt(x_p * x_p + y_p * y_p); } 
        else { evalStack[evIdx++] = x_p * cos(toRadians(y_p, angle_unit)); }
      }
      else {
        if (evIdx < 2) return "Syntax ERROR";
        double b = evalStack[--evIdx];
        double a = evalStack[--evIdx];

        if (t == "+") evalStack[evIdx++] = a + b;
        else if (t == "-") evalStack[evIdx++] = a - b;
        else if (t == "0xb7") evalStack[evIdx++] = a * b;
        else if (t == "0xd7") { if (b == 0) return "Math ERROR"; evalStack[evIdx++] = a / b; }
        else if (t == "^") { if (a < 0 && floor(b) != b) return "Math ERROR"; evalStack[evIdx++] = pow(a, b); }
        else if (t == "x0xab") { if (a == 0) return "Math ERROR"; if (b < 0 && (int)a % 2 == 0) return "Math ERROR"; evalStack[evIdx++] = pow(b, 1.0 / a); }
        else if (t == "0xb0") evalStack[evIdx++] = a * pow(10, b);
        else if (t == "0xb3") { if (a < 0 || b < 0 || b > a || floor(a)!=a || floor(b)!=b) return "Math ERROR"; evalStack[evIdx++] = c_factorial(a) / (c_factorial(b) * c_factorial(a - b)); }
        else if (t == "0xb4") { if (a < 0 || b < 0 || b > a || floor(a)!=a || floor(b)!=b) return "Math ERROR"; evalStack[evIdx++] = c_factorial(a) / c_factorial(a - b); }
      }
    }
  }

  if (evIdx != 1) return "Syntax ERROR";

  double finalResult = evalStack[0];
  if (isnan(finalResult) || isinf(finalResult)) return "Math ERROR";
  
  if (currentResultType == DMS) {
    return decimalToDMSString(finalResult);
  }

  if (currentResultType == FRACTION) {
    return decimalToFractionString(finalResult, defaultFraction);
  }

  char outBuffer[32];
  if (abs(finalResult) < 1e-4 && finalResult != 0.0 || abs(finalResult) >= 1e10) {
    sprintf(outBuffer, "%.9e", finalResult);
  } else {
    sprintf(outBuffer, "%.14g", finalResult);
  }
  return String(outBuffer);
}

String roundResult(String Result, int fix_param, int sci_param, int norm_param, int display_mode) {
  if (Result == "Syntax ERROR" || Result == "Math ERROR" || Result == "0") return Result;
  if (Result.indexOf(',') != -1 || Result.indexOf('/') != -1) return Result;

  double val = Result.toDouble();
  if (isnan(val) || isinf(val)) return "Math ERROR";

  char buffer[32];

  if (display_mode == 0) {
    if (fix_param < 0) fix_param = 0;
    if (fix_param > 9) fix_param = 9;
    
    sprintf(buffer, "%.*f", fix_param, val);
    return String(buffer);
  }
  else if (display_mode == 1) {
    int digits = sci_param;
    if (digits == 0) digits = 10;
    
    int precision = digits - 1;
    if (precision < 0) precision = 0;

    sprintf(buffer, "%.*e", precision, val);
    String s = String(buffer);
    int eIdx = s.indexOf('e');
    if (eIdx != -1) {
      String numPart = s.substring(0, eIdx);
      String expPart = s.substring(eIdx + 1);
      int expVal = expPart.toInt();
      return numPart + "x10^" + String(expVal);
    }
    return s;
  }
  else {
    double absVal = abs(val);
    bool useSci = false;

    if (absVal != 0.0) {
      if (norm_param == 1) {
        if (absVal >= 1e10 || absVal < 0.01) useSci = true;
      } 
      else {
        if (absVal >= 1e10 || absVal < 1e-9) useSci = true;
      }
    }

    if (useSci) {
      sprintf(buffer, "%.9e", val);
      String s = String(buffer);
      int eIdx = s.indexOf('e');
      if (eIdx != -1) {
        String numPart = s.substring(0, eIdx);
        int expVal = s.substring(eIdx + 1).toInt();
        
        if (numPart.indexOf('.') != -1) {
          while (numPart.endsWith("0")) numPart.remove(numPart.length() - 1);
          if (numPart.endsWith(".")) numPart.remove(numPart.length() - 1);
        }
        
        if (expVal == 0) return numPart;
        return numPart + "x10^" + String(expVal);
      }
      return s;
    } 
    else {
      long intPart = (long)absVal;
      int intDigits = 0;
      if (intPart == 0) {
        intDigits = 1;
      } else {
        while (intPart > 0) {
          intDigits++;
          intPart /= 10;
        }
      }

      int precision = 10 - intDigits;
      if (precision < 0) precision = 0;
      if (precision > 9) precision = 9;

      sprintf(buffer, "%.*f", precision, val);
      String s = String(buffer);
      if (s.indexOf('.') != -1) {
        while (s.endsWith("0")) s.remove(s.length() - 1);
        if (s.endsWith(".")) s.remove(s.length() - 1);
      }
      return s;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool shouldPrependAns(String key) {
  key.trim();
  
  if (key == "+" || 
      key == "-" || 
      key == "0xb7" || 
      key == "0xd7" || 
      key == "^" || 
      key == "x0xab" || 
      key == "0xb3" || 
      key == "0xb4" || 
      key == "0xae") {
    return true;
  }
  
  return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


String formatEngString(double num, long exp) {
  String numStr = String(num, 14);
  if (numStr.indexOf('.') != -1) {
    while (numStr.endsWith("0")) numStr.remove(numStr.length() - 1);
    if (numStr.endsWith(".")) numStr.remove(numStr.length() - 1);
  }
  return numStr + "x10^" + String(exp);
}

int countDigits(String numStr) {
  int digits = 0;
  for (int i = 0; i < numStr.length(); i++) {
    if (isdigit(numStr[i])) {
      digits++;
    }
  }
  return digits;
}

String processENG(String input) {
  input.trim();
  if (input.length() == 0) return "0x10^0";

  int xLoc = input.indexOf("x10^");
  if (xLoc == -1) return input;

  double num = input.substring(0, xLoc).toDouble();
  long exp = input.substring(xLoc + 4).toInt();

  if (num == 0.0) {
    long targetExp = exp - 3;
    if (targetExp % 3 != 0) {
      if (targetExp > 0) targetExp -= (targetExp % 3);
      else targetExp -= (3 + (targetExp % 3));
    }
    return formatEngString(0.0, targetExp);
  }

  double nextNum = num * 10.0;
  long nextExp = exp - 1;

  long targetExp;
  if (nextExp >= 0) {
    targetExp = nextExp - (nextExp % 3);
  } else {
    long remainder = nextExp % 3;
    if (remainder == 0) targetExp = nextExp;
    else targetExp = nextExp - (3 + remainder);
  }

  long diff = nextExp - targetExp;
  nextNum = nextNum * pow(10, diff);

  String resultCandidate = formatEngString(nextNum, targetExp);
  int nextXLoc = resultCandidate.indexOf("x10^");
  String justNumber = resultCandidate.substring(0, nextXLoc);

  if (countDigits(justNumber) > 10) {
    return input; 
  }

  return resultCandidate;
}

String processSHIFT_ENG(String input) {
  input.trim();
  if (input.length() == 0) return "0x10^0";

  int xLoc = input.indexOf("x10^");
  if (xLoc == -1) return input;

  double num = input.substring(0, xLoc).toDouble();
  long exp = input.substring(xLoc + 4).toInt();

  if (num == 0.0) {
    long targetExp = exp + 3;
    if (targetExp % 3 != 0) {
      if (targetExp > 0) targetExp += (3 - (targetExp % 3));
      else targetExp += (-targetExp % 3);
    }
    return formatEngString(0.0, targetExp);
  }

  double nextNum = num / 10.0;
  long nextExp = exp + 1;

  long targetExp;
  if (nextExp >= 0) {
    long remainder = nextExp % 3;
    if (remainder == 0) targetExp = nextExp;
    else targetExp = nextExp + (3 - remainder);
  } else {
    targetExp = nextExp - (nextExp % 3);
  }

  long diff = nextExp - targetExp;
  nextNum = nextNum * pow(10, diff);

  String resultCandidate = formatEngString(nextNum, targetExp);
  int nextXLoc = resultCandidate.indexOf("x10^");
  String justNumber = resultCandidate.substring(0, nextXLoc);

  if (countDigits(justNumber) > 10) {
    return input; 
  }

  return resultCandidate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String toggleDegreesAndDecimal(String result) {
  result.trim();
  if (result.length() == 0 || result == "Syntax ERROR" || result == "Math ERROR") {
    return result;
  }

  if (result.indexOf(',') != -1) {
    bool neg = false;
    if (result.startsWith("-")) {
      neg = true;
      result = result.substring(1);
    }

    double deg = 0, min = 0, sec = 0;
    int c1 = result.indexOf(',');
    
    deg = result.substring(0, c1).toDouble();
    int c2 = result.indexOf(',', c1 + 1);
    
    if (c2 != -1) {
      min = result.substring(c1 + 1, c2).toDouble();
      sec = result.substring(c2 + 1).toDouble();
    } else {
      min = result.substring(c1 + 1).toDouble();
    }

    double finalVal = deg + (min / 60.0) + (sec / 3600.0);
    if (neg) finalVal = -finalVal;

    char buf[32];
    sprintf(buf, "%.14g", finalVal);
    return String(buf);
  }
  else {
    double val = result.toDouble();
    bool neg = val < 0;
    val = abs(val);

    long deg = (long)val;
    double remMin = (val - deg) * 60.0;
    long min = (long)remMin;
    double remSec = (remMin - min) * 60.0;
    
    double sec = round(remSec * 100.0) / 100.0; 
    
    if (sec >= 60) { sec -= 60; min++; }
    if (min >= 60) { min -= 60; deg++; }
    
    String secStr = String(sec);
    if (secStr.endsWith(".00")) {
      secStr = secStr.substring(0, secStr.length() - 3);
    } else if (secStr.indexOf('.') != -1) {
      while (secStr.endsWith("0")) secStr.remove(secStr.length() - 1);
      if (secStr.endsWith(".")) secStr.remove(secStr.length() - 1);
    }

    String res = "";
    if (neg) res += "-";
    res += String(deg) + "," + String(min) + "," + secStr;
    return res;
  }
}


String toggleFractionAndDecimal(String Result, bool defaultFraction) {
  Result.trim();
  if (Result.length() == 0 || Result == "Syntax ERROR" || Result == "Math ERROR" || Result == "0") {
    return Result;
  }

  if (Result.indexOf('.') == -1 && Result.indexOf('/') == -1) {
    return Result; 
  }

  if (Result.indexOf('/') != -1 && Result.indexOf('/', Result.indexOf('/') + 1) != -1) {
    int firstSlash = Result.indexOf('/');
    int secondSlash = Result.indexOf('/', firstSlash + 1);

    long whole = Result.substring(0, firstSlash).toInt();
    long num = Result.substring(firstSlash + 1, secondSlash).toInt();
    long den = Result.substring(secondSlash + 1).toInt();

    if (den == 0) return "Math ERROR";

    long newNum = (whole >= 0) ? (whole * den + num) : (whole * den - num);
    
    return String(newNum) + "/" + String(den);
  }
  else if (Result.indexOf('/') != -1) {
    int slash = Result.indexOf('/');
    double num = Result.substring(0, slash).toDouble();
    double den = Result.substring(slash + 1).toDouble();

    if (den == 0) return "Math ERROR";
    double val = num / den;

    char buf[32];
    sprintf(buf, "%.14g", val);
    return String(buf);
  }
  else {
    double val = Result.toDouble();
    bool neg = val < 0;
    val = abs(val);

    double m[2][2];
    double x = val;
    long max_den = 10000;
    
    m[0][0] = m[1][1] = 1;
    m[0][1] = m[1][0] = 0;
    
    while (m[1][0] * (long)x + m[1][1] <= max_den) {
      long a = (long)x;
      double t = m[0][0] * a + m[0][1];
      m[0][1] = m[0][0]; m[0][0] = t;
      
      t = m[1][0] * a + m[1][1];
      m[1][1] = m[1][0]; m[1][0] = t;
      
      if (abs(x - a) < 1e-9) break;
      x = 1.0 / (x - a);
    }
    
    long num = round(m[0][0]);
    long den = round(m[1][0]);
    
    if (den == 0 || abs(val - ((double)num / den)) > 1e-9) {
      return Result;
    }
    
    long g = gcd(num, den);
    if (g != 0) {
      num /= g;
      den /= g;
    }

    if (den == 1) {
      return neg ? "-" + String(num) : String(num);
    }

    String res = "";
    if (neg) res += "-";

    if (defaultFraction && num > den) {
      long cijeliDio = num / den;
      long ostatak = num % den;
      if (ostatak == 0) {
        res += String(cijeliDio);
      } else {
        res += String(cijeliDio) + "/" + String(ostatak) + "/" + String(den);
      }
    } else {
      res += String(num) + "/" + String(den);
    }

    return res;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String convertValue(String type, String fromUnit, String toUnit, String valueStr) {
  double val = 0;
  
  if (valueStr.indexOf("frac") != -1 || valueStr.indexOf('/') != -1) {
    int slash = valueStr.indexOf('/');
    if (slash == -1) slash = valueStr.indexOf("frac");
    if (slash != -1) {
      double num = valueStr.substring(0, slash).toDouble();
      double den = valueStr.substring(slash + (valueStr.substring(slash, slash+4) == "frac" ? 4 : 1)).toDouble();
      if (den != 0) val = num / den;
    }
  } else {
    val = valueStr.toDouble();
  }

  if (type == "Temperature") {
    double celsius = 0;
    if (fromUnit == "C") celsius = val;
    else if (fromUnit == "F") celsius = (val - 32.0) * 5.0 / 9.0;
    else if (fromUnit == "K") celsius = val - 273.15;

    if (toUnit == "C") return String(celsius, 7);
    else if (toUnit == "F") return String((celsius * 9.0 / 5.0) + 32.0, 7);
    else if (toUnit == "K") return String(celsius + 273.15, 7);
    return valueStr;
  }

  if (type == "Numeral") {
    long numDec = 0;
    if (fromUnit == "DEC") numDec = strtol(valueStr.c_str(), NULL, 10);
    else if (fromUnit == "BIN") numDec = strtol(valueStr.c_str(), NULL, 2);
    else if (fromUnit == "HEX") numDec = strtol(valueStr.c_str(), NULL, 16);
    else if (fromUnit == "OCT") numDec = strtol(valueStr.c_str(), NULL, 8);

    if (toUnit == "DEC") return String(numDec, 10);
    else if (toUnit == "BIN") {
      String b = "";
      while (numDec > 0) { b = String(numDec % 2) + b; numDec /= 2; }
      return b == "" ? "0" : b;
    }
    else if (toUnit == "HEX") {
      char buf[20]; sprintf(buf, "%X", (unsigned int)numDec); return String(buf);
    }
    else if (toUnit == "OCT") {
      char buf[20]; sprintf(buf, "%o", (unsigned int)numDec); return String(buf);
    }
    return valueStr;
  }

  double factorFrom = 1.0, factorTo = 1.0;

  if (type == "Length") { 
    if (fromUnit == "m") factorFrom = 1.0;
    else if (fromUnit == "cm") factorFrom = 0.01;
    else if (fromUnit == "mm") factorFrom = 0.001;
    else if (fromUnit == "um") factorFrom = 1e-6;
    else if (fromUnit == "nm") factorFrom = 1e-9;
    else if (fromUnit == "pm") factorFrom = 1e-12;
    else if (fromUnit == "km") factorFrom = 1000.0;
    else if (fromUnit == "inch") factorFrom = 0.0254;
    else if (fromUnit == "ft") factorFrom = 0.3048;
    else if (fromUnit == "yd") factorFrom = 0.9144;
    else if (fromUnit == "mile") factorFrom = 1609.344;
    else if (fromUnit == "AU") factorFrom = 149597870700.0;
    else if (fromUnit == "ly") factorFrom = 9460730472580800.0;

    if (toUnit == "m") factorTo = 1.0;
    else if (toUnit == "cm") factorTo = 0.01;
    else if (toUnit == "mm") factorTo = 0.001;
    else if (toUnit == "um") factorTo = 1e-6;
    else if (toUnit == "nm") factorTo = 1e-9;
    else if (toUnit == "pm") factorTo = 1e-12;
    else if (toUnit == "km") factorTo = 1000.0;
    else if (toUnit == "inch") factorTo = 0.0254;
    else if (toUnit == "ft") factorTo = 0.3048;
    else if (toUnit == "yd") factorTo = 0.9144;
    else if (toUnit == "mile") factorTo = 1609.344;
    else if (toUnit == "AU") factorTo = 149597870700.0;
    else if (toUnit == "ly") factorTo = 9460730472580800.0;
  }
  else if (type == "Mass") { 
    if (fromUnit == "g") factorFrom = 1.0;
    else if (fromUnit == "kg") factorFrom = 1000.0;
    else if (fromUnit == "mg") factorFrom = 0.001;
    else if (fromUnit == "ug") factorFrom = 1e-6;
    else if (fromUnit == "ng") factorFrom = 1e-9;
    else if (fromUnit == "ton") factorFrom = 1000000.0;
    else if (fromUnit == "lb") factorFrom = 453.59237;
    else if (fromUnit == "oz") factorFrom = 28.349523125;
    else if (fromUnit == "ct") factorFrom = 0.2;

    if (toUnit == "g") factorTo = 1.0;
    else if (toUnit == "kg") factorTo = 1000.0;
    else if (toUnit == "mg") factorTo = 0.001;
    else if (toUnit == "ug") factorTo = 1e-6;
    else if (toUnit == "ng") factorTo = 1e-9;
    else if (toUnit == "ton") factorTo = 1000000.0;
    else if (toUnit == "lb") factorTo = 453.59237;
    else if (toUnit == "oz") factorTo = 28.349523125;
    else if (toUnit == "ct") factorTo = 0.2;
  }
  else if (type == "Area") { 
    if (fromUnit == "m2") factorFrom = 1.0;
    else if (fromUnit == "cm2") factorFrom = 0.0001;
    else if (fromUnit == "mm2") factorFrom = 0.000001;
    else if (fromUnit == "um2") factorFrom = 1e-12;
    else if (fromUnit == "nm2") factorFrom = 1e-18;
    else if (fromUnit == "pm2") factorFrom = 1e-24;
    else if (fromUnit == "km2") factorFrom = 1000000.0;
    else if (fromUnit == "ha") factorFrom = 10000.0;
    else if (fromUnit == "acre") factorFrom = 4046.85642;
    else if (fromUnit == "sq_in") factorFrom = 0.00064516;
    else if (fromUnit == "sq_ft") factorFrom = 0.09290304;
    else if (fromUnit == "sq_mi") factorFrom = 2589988.110336;

    if (toUnit == "m2") factorTo = 1.0;
    else if (toUnit == "cm2") factorTo = 0.0001;
    else if (toUnit == "mm2") factorTo = 0.000001;
    else if (toUnit == "um2") factorTo = 1e-12;
    else if (toUnit == "nm2") factorTo = 1e-18;
    else if (toUnit == "pm2") factorTo = 1e-24;
    else if (toUnit == "km2") factorTo = 1000000.0;
    else if (toUnit == "ha") factorTo = 10000.0;
    else if (toUnit == "acre") factorTo = 4046.85642;
    else if (toUnit == "sq_in") factorTo = 0.00064516;
    else if (toUnit == "sq_ft") factorTo = 0.09290304;
    else if (toUnit == "sq_mi") factorTo = 2589988.110336;
  }
  else if (type == "Time") { 
    if (fromUnit == "s") factorFrom = 1.0;
    else if (fromUnit == "ms") factorFrom = 0.001;
    else if (fromUnit == "us") factorFrom = 1e-6;
    else if (fromUnit == "ns") factorFrom = 1e-9;
    else if (fromUnit == "ps") factorFrom = 1e-12;
    else if (fromUnit == "min") factorFrom = 60.0;
    else if (fromUnit == "hour") factorFrom = 3600.0;
    else if (fromUnit == "day") factorFrom = 86400.0;
    else if (fromUnit == "week") factorFrom = 604800.0;
    else if (fromUnit == "month") factorFrom = 2629743.83; 
    else if (fromUnit == "year") factorFrom = 31556926.0;

    if (toUnit == "s") factorTo = 1.0;
    else if (toUnit == "ms") factorTo = 0.001;
    else if (toUnit == "us") factorTo = 1e-6;
    else if (toUnit == "ns") factorTo = 1e-9;
    else if (toUnit == "ps") factorTo = 1e-12;
    else if (toUnit == "min") factorTo = 60.0;
    else if (toUnit == "hour") factorTo = 3600.0;
    else if (toUnit == "day") factorTo = 86400.0;
    else if (toUnit == "week") factorTo = 604800.0;
    else if (toUnit == "month") factorTo = 2629743.83;
    else if (toUnit == "year") factorTo = 31556926.0;
  }
  else if (type == "Data") { 
    if (fromUnit == "B") factorFrom = 1.0;
    else if (fromUnit == "KB") factorFrom = 1024.0;
    else if (fromUnit == "MB") factorFrom = 1048576.0;
    else if (fromUnit == "GB") factorFrom = 1073741824.0;
    else if (fromUnit == "TB") factorFrom = 1099511627776.0;
    else if (fromUnit == "PB") factorFrom = 1125899906842624.0;
    else if (fromUnit == "EB") factorFrom = 1152921504606846976.0;

    if (toUnit == "B") factorTo = 1.0;
    else if (toUnit == "KB") factorTo = 1024.0;
    else if (toUnit == "MB") factorTo = 1048576.0;
    else if (toUnit == "GB") factorTo = 1073741824.0;
    else if (toUnit == "TB") factorTo = 1099511627776.0;
    else if (toUnit == "PB") factorTo = 1125899906842624.0;
    else if (toUnit == "EB") factorTo = 1152921504606846976.0;
  }
  else if (type == "Volume") { 
    if (fromUnit == "L") factorFrom = 1.0;
    else if (fromUnit == "ml") factorFrom = 0.001;
    else if (fromUnit == "ul") factorFrom = 1e-6;
    else if (fromUnit == "m3") factorFrom = 1000.0;
    else if (fromUnit == "gal") factorFrom = 3.785411784;
    else if (fromUnit == "qt") factorFrom = 0.946352946;
    else if (fromUnit == "pt") factorFrom = 0.473176473;
    else if (fromUnit == "cup") factorFrom = 0.236588236;
    else if (fromUnit == "fl_oz") factorFrom = 0.0295735296;

    if (toUnit == "L") factorTo = 1.0;
    else if (toUnit == "ml") factorTo = 0.001;
    else if (toUnit == "ul") factorTo = 1e-6;
    else if (toUnit == "m3") factorTo = 1000.0;
    else if (toUnit == "gal") factorTo = 3.785411784;
    else if (toUnit == "qt") factorTo = 0.946352946;
    else if (toUnit == "pt") factorTo = 0.473176473;
    else if (toUnit == "cup") factorTo = 0.236588236;
    else if (toUnit == "fl_oz") factorTo = 0.0295735296;
  }
  else if (type == "Speed") { 
    if (fromUnit == "m/s") factorFrom = 1.0;
    else if (fromUnit == "km/h") factorFrom = 1.0 / 3.6;
    else if (fromUnit == "mph") factorFrom = 0.44704;
    else if (fromUnit == "knot") factorFrom = 0.514444;
    else if (fromUnit == "km/s") factorFrom = 1000.0;
    else if (fromUnit == "c") factorFrom = 299792458.0; 

    if (toUnit == "m/s") factorTo = 1.0;
    else if (toUnit == "km/h") factorTo = 1.0 / 3.6;
    else if (toUnit == "mph") factorTo = 0.44704;
    else if (toUnit == "knot") factorTo = 0.514444;
    else if (toUnit == "km/s") factorTo = 1000.0;
    else if (toUnit == "c") factorTo = 299792458.0;
  }

  double baseValue = val * factorFrom;
  return String(baseValue / factorTo, 7);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int charToVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

double anyBaseToDec(String numStr, int base) {
  int dotIdx = numStr.indexOf('.');
  String mainPart = (dotIdx == -1) ? numStr : numStr.substring(0, dotIdx);
  String fracPart = (dotIdx == -1) ? "" : numStr.substring(dotIdx + 1);

  double result = 0;
  double power = 1;
  for (int i = mainPart.length() - 1; i >= 0; i--) {
    int val = charToVal(mainPart[i]);
    if (val >= base || val == -1) continue;
    result += val * power;
    power *= base;
  }

  double fracPower = 1.0 / base;
  for (int i = 0; i < fracPart.length(); i++) {
    int val = charToVal(fracPart[i]);
    if (val >= base || val == -1) continue;
    result += val * fracPower;
    fracPower /= base;
  }

  return result;
}

bool isHexChar(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

double evalDecExpression(String expr) {
  std::vector<double> numbers;
  std::vector<char> ops;
  
  int i = 0;
  while (i < expr.length()) {
    if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
      ops.push_back(expr[i]);
      i++;
    } else {
      int start = i;
      if (expr[i] == '-') { i++; }
      while (i < expr.length() && ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.')) {
        i++;
      }
      numbers.push_back(expr.substring(start, i).toDouble());
    }
  }
  
  if (numbers.size() == 0) return 0;

  for (size_t j = 0; j < ops.size(); ) {
    if (ops[j] == '*' || ops[j] == '/') {
      if (ops[j] == '*') numbers[j] = numbers[j] * numbers[j+1];
      else numbers[j] = (numbers[j+1] != 0) ? numbers[j] / numbers[j+1] : 0;
      
      numbers.erase(numbers.begin() + j + 1);
      ops.erase(ops.begin() + j);
    } else {
      j++;
    }
  }

  double res = numbers[0];
  for (size_t j = 0; j < ops.size(); j++) {
    if (ops[j] == '+') res += numbers[j+1];
    else if (ops[j] == '-') res -= numbers[j+1];
  }

  return res;
}

double parseSimpleExpression(String expr) {
  while (expr.indexOf('[') != -1) {
    int start = expr.indexOf('[');
    int endIdx = expr.indexOf(']');
    if (endIdx == -1) break;
    
    int base = expr.substring(start + 1, endIdx).toInt();
    int nextOp = endIdx + 1;
    while (nextOp < expr.length() && (isHexChar(expr[nextOp]) || expr[nextOp] == '.')) {
      nextOp++;
    }
    
    String numStr = expr.substring(endIdx + 1, nextOp);
    double decVal = anyBaseToDec(numStr, base);
    expr = expr.substring(0, start) + String(decVal, 6) + expr.substring(nextOp);
  }

  return evalDecExpression(expr);
}

double evaluateExpression(String expr) {
  while (expr.indexOf('(') != -1) {
    int closeBrace = expr.indexOf(')');
    int openBrace = expr.lastIndexOf('(', closeBrace);
    if (openBrace == -1 || closeBrace == -1) break;

    String subExpr = expr.substring(openBrace + 1, closeBrace);
    double subResult = evaluateExpression(subExpr);
    expr = expr.substring(0, openBrace) + String(subResult, 6) + expr.substring(closeBrace + 1);
  }

  return parseSimpleExpression(expr);
}