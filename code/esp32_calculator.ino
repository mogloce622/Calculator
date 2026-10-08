#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "geminiPicture.h"

#include "calcFont.h"
#include "calcFontSmall.h"
#include "Digital-7.h"
#include "digital-7-small.h"

#include <WiFi.h>
#include <WebServer.h>
#include "credentials.h"
#include <HTTPClient.h>

#include <ArduinoJson.h>
#include <EEPROM.h>
#include <LittleFS.h>
#include "driver/rtc_io.h"

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8
#define TFT_BLK 47

#define TFT_SCK  12
#define TFT_MOSI 11

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const int ROW1_NUM = 8;
const int COL1_NUM = 4;

int row1Pins[ROW1_NUM] = {3, 2, 4, 5, 6, 7, 39, 40};
int col1Pins[COL1_NUM] = {17, 18, 21, 33};

const String keys1[ROW1_NUM][COL1_NUM] = {
  {"",      "",      "",      "MODE"},
  {"SHIFT", "ALPHA", "up",    "down"},   //
  {"0xaa",  "0xae",  "0xaf",  "rcl"},    //(power of -1)     (mixed fraction)     (negative number)      (rcl)
  {"0xb3",  "0xab",  "0x90",  "eng "},   //(nCr)             (root)               (degre)                (eng)
  {"left",  "0x92",  "hyp ",  "("},      //(left)            (power of 2)         (hyp)                  (bracket ( )
  {"right", "^",     "sin ",  ")"},      //(right)           (power)              (sin)                  (bracket ) )
  {"Pol(",  "log ",  "cos ",  ","},      //(Pol( )           (log)                (cos)                  (,)
  {"0x93",  "In ",   "tan ",  "M+"}      //(power of 3)      (In)                 (tan)                  (M+)
};

const String keys1shift[ROW1_NUM][COL1_NUM] = {
  {"",         "",      "",         "CLR"}, 
  {"SHIFT",    "ALPHA", "up",       "down"}, //
  {"!",        "0xae",  "0xaf",     "sto"},  //(factorial)       (fraction)             (negative number)     (sto)
  {"0xb4",     "0xab",  "0x90",     "eng "}, //(nPr)             (root)                 (degre)               (eng)
  {"left",     "0x92",  "hyp ",     "("},    //(left)            (power of 2)           (hyp)                 (bracket ( )
  {"right",    "x0xab", "sin0xaa ", ")"},    //(right)           (x root)               (sin-1)               (bracket ) )
  {"Rec(",     "0xb5",  "cos0xaa ", ";"},    //(Rec( )           (10^x)                 (cos-1)               (;)
  {"0x930xab", "0xb2",  "tan0xaa ", "M-"}    //(third root)      (Eulers number ^ x)    (tan-1)               (M-)
};

const String keys1alpha[ROW1_NUM][COL1_NUM] = {
  {"",      "",      "",   "SPEC"},
  {"SHIFT", "ALPHA", "up", "down"},   //
  {"0xaa",  "0xae",  "A",  "rcl"},    //(power of -1)     (mixed fraction)    (A)       (rcl)
  {"0xb3",   "0xab", "B",  "eng "},   //(nCr)             (root)              (B)       (eng)
  {"left",  "0x92",  "C",  "("},      //(left)            (power of 2)        (C)       (bracket ( )
  {"right", "^",     "D",  "X"},      //(right)           (power)             (D)       (X)
  {":",     "log ",  "E",  "Y"},      //(:)               (log)               (E)       (Y)
  {"0x93",  "0xb1",  "F",  "M"}       //(power of 3)      (Eulers number)     (F)       (M)
};


unsigned long lastDebounceTime1 = 0;
const unsigned long debounceDelay1 = 150;
String lastKey1 = "";

const int ROW2_NUM = 5;
const int COL2_NUM = 4;

int row2Pins[ROW2_NUM] = {16, 15, 14, 34, 42};
int col2Pins[COL2_NUM] = {33, 21, 18, 17};

const String keys2[ROW2_NUM][COL2_NUM] = {
  {"7",   "4",    "1", "0"},   //
  {"8",   "5",    "2", "."},   //
  {"9",   "6",    "3", "0xb0"},//(exponent)
  {"DEL", "0xb7", "+", "Ans"}, //(*)
  {"AC",  "0xd7", "-", "="}    //(/)
};

const String keys2shift[ROW2_NUM][COL2_NUM] = {
  {"7",   "4",    "1", "Rnd"},  //
  {"8",   "5",    "2", "Ran#"}, //(random)
  {"9",   "6",    "3", "0xad"}, //(pi)
  {"INS", "0xb7", "+", "DRG"},  //(*)
  {"OFF", "0xd7", "-", "%"}     //(/)
};

unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay2 = 150;
String lastKey2 = "";

static unsigned long lastSpecialTime = 0;
const unsigned long debounceSpecialTime = 150;

unsigned long last = 0;

/*calculator*/
String expression[50];
int count = 0;
String result = "0";
String ans = "";
String err = "";
String newResult = "";
int dot = 0;
bool exponent2 = false;
bool isExponentNegative = false;
int exponent = 0;

bool indicator = false;
bool indicator2 = true;
int printedCharsCount = 0;
int currentIndicatorPos = 0;
bool scrolling = false;

#define MAX_LOG_TOKENS 50   
int logStart[MAX_LOG_TOKENS];
int logVisLen[MAX_LOG_TOKENS];
int logFootprint[MAX_LOG_TOKENS];
int logCount = 0;

int lastCount = 0;
int selectedLogIdx = -1;
int windowFirstLog = 0;
int lastPrintedLogIdx = -999;

#define MAX_HISTORY 20
String expressionHistory[MAX_HISTORY][50];
int historyCount[MAX_HISTORY];
bool scrolling2 = false;
int historyEntries = 0;
int historyIndex = -1;

bool alpha = false;
bool shift = false;
bool ins = false;
bool hyp = false;
bool rcl = false;
bool sto = false;
bool M = false;
bool spec = false;

//modes
int calcMode = 0; //0-COMP  1-SD  2-REG
int angleUnit = 0; //0-Deg  1-Rad  2-Gra
int displayMode = 2; //0-Fix  1-Sci  2-Norm
int displayParameterFix = 2; //Fix(0-9)
int displayParameterSci = 2; //Sci(0-9)
int displayParameterNorm = 2; //Norm(1-2)
bool defaultFrac = true; //true->mixed , false->proper

//special
int battery = 0;
const int adcPin = 1;
const int numSamples = 20;
#define WAKEUP_PIN GPIO_NUM_13

bool wifiStatus = false;
int specialMenuState = 0;
String menuItems[9] = {"Notes", "Formulas", "Convert", "Numeral", "Games", "AI", "Camera", "Edit", "WiFi"};

//notes
int activeNote = -1; 
int selectedNoteIdx = 0;
String noteTitles[30] = {};
String noteTexts[30] = {};

//formulas
int activeFormula = -1;
int selectedFormulaIdx = 0;
String formulaTitles[30] = {};
String formulaTexts[30] = {};

//converte
int convStep = 0;
int activeCatIdx = 0;
int fromUnitIdx = 0;
int toUnitIdx = 0;
String convInput = "";
String convResult = "";

String convCategories[9] = {"Length", "Mass", "Area", "Time", "Data", "Volume", "Numeral", "Speed", "Temperature"};

String unitsMatrix[9][15] = {
  {"m", "cm", "mm", "um", "nm", "pm", "km", "inch", "ft", "yd", "mile", "AU", "ly", "", ""},
  {"g", "kg", "mg", "ug", "ng", "ton", "lb", "oz", "ct", "", "", "", "", "", ""},
  {"m2", "cm2", "mm2", "um2", "nm2", "pm2", "km2", "ha", "acre", "sq_in", "sq_ft", "sq_mi", "", "", ""},
  {"s", "ms", "us", "ns", "ps", "min", "hour", "day", "week", "month", "year", "", "", "", ""},
  {"B", "KB", "MB", "GB", "TB", "PB", "EB", "", "", "", "", "", "", "", ""},
  {"L", "ml", "ul", "m3", "gal", "qt", "pt", "cup", "fl_oz", "", "", "", "", "", ""},
  {"DEC", "BIN", "HEX", "OCT", "", "", "", "", "", "", "", "", "", "", ""},
  {"m/s", "km/h", "mph", "knot", "km/s", "c", "", "", "", "", "", "", "", "", ""},
  {"C", "F", "K", "", "", "", "", "", "", "", "", "", "", "", ""}
};

//Numeral
String numExpr = "";       
String numDisplay = "";    
String currentBaseStr = "";
String numResult = "";
bool enteringBase = true;
bool showCursor = true;
bool engMode = false;

//games
int gameSelect = 0;
int gameState = 0;

int snakeX[50];
int snakeY[50];
int snakeLen = 0;
int snakeDir = 0;
int foodX = 0;
int foodY = 0;
unsigned long lastSnakeMove = 0;

int board[9];
int cursorXOX = 4;
int playerXOX = 1;
int cell   = 34;
int thick  = 3;      
int small  = 32;
int grid_w = cell * 3 + thick * 2;        
int grid_x = (320 - grid_w) / 2;          
int grid_y = 115;

int p1Y = 150;
int p2Y = 150;
int ballX = 160;
int ballY = 170;
int ballVX = 3;
int ballVY = 2;
unsigned long lastPongMove = 0;

//ai
int keyRow = 0;
int keyCol = 0;
const String aiPrompttConst = "Answer the question in the language it was asked in, use only ASCII characters for the response, and keep the answer concise: ";
String aiPrompt = "";
String aiResponse = "";
bool aiViewingResponse = false;
bool aiIntro = true;

//edit(web)
WebServer server(80);
bool isServerRunning = false;

void setup() {
  gpio_hold_dis((gpio_num_t)TFT_BLK);
  gpio_deep_sleep_hold_dis();
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, LOW);

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);

  tft.begin();
  tft.setRotation(3);
  //tft.fillScreen(tft.color565(150, 175, 130));
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));

  digitalWrite(TFT_BLK, HIGH);

  Serial.begin(115200);
  delay(1000);

  for (int i = 0; i < ROW1_NUM; i++) {
    pinMode(row1Pins[i], OUTPUT);
    digitalWrite(row1Pins[i], HIGH);
  }

  for (int i = 0; i < COL1_NUM; i++) {
    pinMode(col1Pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < ROW2_NUM; i++) {
    pinMode(row2Pins[i], OUTPUT);
    digitalWrite(row2Pins[i], HIGH);
  }

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(WAKEUP_PIN, INPUT_PULLUP);

  analogSetPinAttenuation(adcPin, ADC_11db); 

  setupServerRoutes();

  EEPROM.begin(256);
  if (EEPROM.read(0) == 42) {
    readFromEEPROM();
  }
  initLittleFS();
  readArrays();
}

void loop() {
  unsigned long now = millis();

  String pressedBtn = scanFunctionKeypad();
  if (pressedBtn == "") pressedBtn = scanNumberKeypad();
  if (pressedBtn == "") pressedBtn = scanSpecialKeypad();
  
  if (pressedBtn != "") Serial.println(pressedBtn);

  if(spec) {
    special(pressedBtn);
    return;
  }

  if(pressedBtn == "=") {
    result = calculate(expression, ans, angleUnit, defaultFrac);
    if(result == "Syntax ERROR" || result == "Math ERROR") {
      err = result;
      result = ans;
    } else {
      ans = result;
      result = roundResult(result, displayParameterFix, displayParameterSci, displayParameterNorm, displayMode);

      Serial.print("result: ");
      Serial.println(result);

      if (result.indexOf("x10^") != -1) {
        int expIdx = result.indexOf("x10^");
        String expStr = result.substring(expIdx + 4);
        int rawExponent = expStr.toInt();

        if (rawExponent == 0) {
          result = result.substring(0, expIdx);
          exponent2 = false;
          isExponentNegative = false;
          exponent = 0;
        } else {
          exponent2 = true;
          result = result.substring(0, expIdx);
          if (rawExponent < 0) {
            isExponentNegative = true;
            exponent = -rawExponent;
          } else {
            isExponentNegative = false;
            exponent = rawExponent;
          }
        }
      } else {
        exponent2 = false;
        isExponentNegative = false;
        exponent = 0;
      }
      Serial.print("ans: ");
      Serial.println(ans);

      int size = 10;
      for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
      result = result.substring(0, size);
    }

    shift = false;
    alpha = false;
    hyp = false;
    indicator2 = false;
    newResult = "";
    dot = 0;
    ins = false;
    pushHistory();
  } 
  else if(pressedBtn == "SHIFT") {
    shift = !shift;
    alpha = false;
    tft.fillRect(0, 110, 320, 17, tft.color565(150, 175, 130));
  } 
  else if(pressedBtn == "ALPHA") {
    alpha = !alpha;
    shift = false;
    tft.fillRect(0, 110, 320, 17, tft.color565(150, 175, 130));
  } 
  else if(pressedBtn == "left" || pressedBtn == "right" || pressedBtn == "up" || pressedBtn == "down") {
    if(pressedBtn == "left") {
      if(err == "") {
        if (!scrolling) {
          selectedLogIdx = logCount - 1;
          scrolling = (logCount > 0);
        } else if (selectedLogIdx > 0) {
          selectedLogIdx--;
        }
        tft.fillRect(0, 125, 320, 65, tft.color565(150, 175, 130)); 
        indicator2 = true;
      } else err = "";
    }
    else if(pressedBtn == "right") {
      if(err == "") {
        if (scrolling) {
          if (selectedLogIdx < logCount - 1) {
            selectedLogIdx++;
          } else {
            scrolling = false;
            selectedLogIdx = -1;
          }
        }
        tft.fillRect(0, 125, 320, 65, tft.color565(150, 175, 130)); 
        indicator2 = true;
      } else err = "";
    }

    else if(pressedBtn == "up" && !indicator2 && err == ""){
      if (historyEntries > 0) {
        if (historyIndex == -1) historyIndex = historyEntries - 1;
        else if (historyIndex > 0) historyIndex--;
        scrolling2 = true;
        loadFromHistory(historyIndex);
      }
    }
    else if(pressedBtn == "down" && !indicator2 && err == ""){
      if (historyEntries > 0 && historyIndex != -1) {
        if (historyIndex < historyEntries - 1) {
          historyIndex++;
          loadFromHistory(historyIndex);
        } else {
          historyIndex = -1;
          scrolling2 = false;
          count = 0;
          for (int i = 0; i < 50; i++) expression[i] = "";
          tft.fillRect(0, 125, 320, 65, tft.color565(150, 175, 130));
        }
      }
    }
  }
  else if(pressedBtn == "ON") {
    for(int i = 0; i < count; i++) expression[i] = "";
    clearHistory();
    count = 0;
    lastCount = 0;
    result = "0";
    ans = "";
    dot = 0;
    exponent = 0;
    exponent2 = false;
    isExponentNegative = false;
    shift = false;
    alpha = false;
    hyp = false;
    err = "";
    indicator2 = true;
    newResult = "";
    ins = false;
    spec = false;
    scrolling = false;
    selectedLogIdx = -1;
    windowFirstLog = 0;
    lastPrintedLogIdx = -999;
  }
  else if(pressedBtn == "AC") {
    for(int i = 0; i < count; i++) expression[i] = "";
    count = 0;
    lastCount = 0;
    result = "0";
    dot = 0;
    exponent = 0;
    exponent2 = false;
    isExponentNegative = false;
    shift = false;
    alpha = false;
    hyp = false;
    err = "";
    indicator2 = true;
    newResult = "";
    ins = false;
    scrolling = false;
    selectedLogIdx = -1;
    windowFirstLog = 0;
    lastPrintedLogIdx = -999;
    historyIndex = -1;
    scrolling2 = false;
  } 
  else if(pressedBtn == "OFF") {
    for(int i = 0; i < count; i++) expression[i] = "";
    clearHistory();
    count = 0;
    lastCount = 0;
    result = "0";
    ans = "";
    dot = 0;
    exponent = 0;
    exponent2 = false;
    isExponentNegative = false;
    shift = false;
    alpha = false;
    hyp = false;
    err = "";
    indicator2 = true;
    newResult = "";
    ins = false;
    spec = false;
    scrolling = false;
    selectedLogIdx = -1;
    windowFirstLog = 0;
    lastPrintedLogIdx = -999;
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    digitalWrite(TFT_BLK, LOW);

    gpio_hold_en((gpio_num_t)TFT_BLK);
    gpio_deep_sleep_hold_en();

    tft.writeCommand(0x10);
    delay(120);

    esp_sleep_enable_ext1_wakeup(1ULL << WAKEUP_PIN, ESP_EXT1_WAKEUP_ANY_LOW);
    rtc_gpio_pullup_en(WAKEUP_PIN);
    rtc_gpio_pulldown_dis(WAKEUP_PIN);

    esp_deep_sleep_start();
  }
  else if(pressedBtn == "DEL" && err == "") {
    if(!indicator2) indicator2 = true;
    else if(scrolling && selectedLogIdx >= 0){
      int pos = logStart[selectedLogIdx];
      int removeCount = 1;
      if(pos+1 < count && expression[pos+1] == " ") removeCount = 2;

      for(int k = pos; k + removeCount < count; k++) {
        expression[k] = expression[k + removeCount];
      }
      for(int k = count - removeCount; k < count; k++) {
        expression[k] = "";
      }
      count -= removeCount;

      if(count <= 0) {
        count = 0;
        scrolling = false;
        selectedLogIdx = -1;
        windowFirstLog = 0;
        lastPrintedLogIdx = -999;
      } else {
        lastPrintedLogIdx = -999;
      }
    } 
    else {
      int pos = count<=0? 0 : count-1;
      if(expression[pos] == " ") expression[--count] = "";
      expression[--count] = "";
      if(count <= 0) {
        count = 0;
        scrolling = false;
        selectedLogIdx = -1;
        windowFirstLog = 0;
        lastPrintedLogIdx = -999;
      }
    }
  }
  else if(pressedBtn == "INS") {
    ins = !ins;
    shift = false;
    alpha = false;
    hyp = false;
  }
  else if(pressedBtn == "MODE") {
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    shift = false;
    alpha = false;
    hyp = false;

    tft.setTextSize(1);
    tft.setTextColor(ILI9341_BLACK);

    tft.setFont(&calcFont);
    tft.setCursor(0, 155);
    tft.println("COMP SD REG");

    tft.setFont(&Digital_736pt8b);
    tft.setCursor(0, 216);
    tft.println("1    2  3");

    int modeNum = 0;

    do {
      pressedBtn = scanFunctionKeypad();
      if (pressedBtn == "") pressedBtn = scanNumberKeypad();
      if (pressedBtn != "") Serial.println(pressedBtn);

      if(pressedBtn == "MODE") {
        modeNum++;
        if(modeNum > 3) break;
        tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
      }

      int selected = 0;
      if(isNumber(pressedBtn)) {
        selected =  pressedBtn.toInt();
        if(selected < 1) selected=1;
        else if(selected > 3) selected=3;
      }

      if(modeNum == 0) {
        tft.setFont(&calcFont);
        tft.setCursor(0, 155);
        tft.println("COMP SD REG");
        tft.setFont(&Digital_736pt8b);
        tft.setCursor(0, 216);
        tft.println("1    2  3");
        if(selected > 0) calcMode = selected-1;
      } else if(modeNum == 1) {
        tft.setFont(&calcFont);
        tft.setCursor(0, 155);
        tft.println("Deg Rad Gra");
        tft.setFont(&Digital_736pt8b);
        tft.setCursor(0, 216);
        tft.println("1   2   3");
        if(selected > 0) angleUnit = selected-1;
      } else if(modeNum == 2) {
        tft.setFont(&calcFont);
        tft.setCursor(0, 155);
        tft.println("Fix Sci Norm");
        tft.setFont(&Digital_736pt8b);
        tft.setCursor(0, 216);
        tft.println("1   2   3");
        if(selected > 0) {
          displayMode = selected-1;

          tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
          tft.setFont(&calcFont);
          tft.setCursor(0, 155);

          if(displayMode == 0) {
            tft.println("Fix 0~9?");
          } else if(displayMode == 1){
            tft.println("Sci 0~9?");
          } else{
            tft.println("Norm 1~2?");
          }

          do {
            pressedBtn = scanNumberKeypad();
          } while(pressedBtn == "" || !isNumber(pressedBtn));
          int dec =  pressedBtn.toInt();

          if(displayMode == 2 && dec<1) dec = 1; 
          else if(displayMode == 2 && dec>2) dec = 2; 

          if(displayMode == 0) displayParameterFix = dec;
          else if(displayMode == 1) displayParameterSci = dec;
          else displayParameterNorm = dec;

          if(result != "0"){
            result = roundResult(ans, displayParameterFix, displayParameterSci, displayParameterNorm, displayMode);

            Serial.print("result: ");
            Serial.println(result);

            if (result.indexOf("x10^") != -1) {
              int expIdx = result.indexOf("x10^");
              String expStr = result.substring(expIdx + 4);
              int rawExponent = expStr.toInt();

              if (rawExponent == 0) {
                result = result.substring(0, expIdx);
                exponent2 = false;
                isExponentNegative = false;
                exponent = 0;
              } else {
                exponent2 = true;
                result = result.substring(0, expIdx);
                if (rawExponent < 0) {
                  isExponentNegative = true;
                  exponent = -rawExponent;
                } else {
                  isExponentNegative = false;
                  exponent = rawExponent;
                }
              }
            } else {
              exponent2 = false;
              isExponentNegative = false;
              exponent = 0;
            }
            Serial.print("ans: ");
            Serial.println(ans);

            int size = 10;
            for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
            result = result.substring(0, size);
          }
          dot = 0;
        }

      } else if(modeNum == 3) {
        tft.setFont(&calcFont);
        tft.setCursor(0, 155);
        tft.println("Disp");
        tft.setFont(&Digital_736pt8b);
        tft.setCursor(0, 216);
        tft.println("1");
        if(selected > 0) {
          tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
          tft.setFont(&calcFont);
          tft.setCursor(0, 155);
          tft.println("ab/c d/c");
          tft.setFont(&Digital_736pt8b);
          tft.setCursor(0, 216);
          tft.println("1    2");

          do {
            pressedBtn = scanNumberKeypad();
          } while(pressedBtn == "" || !isNumber(pressedBtn));
          int fracType =  pressedBtn.toInt();

          if(fracType<1) fracType = 1; 
          else if(fracType>2) fracType = 2; 

          defaultFrac = !(fracType-1);

          if(result != "0"){
            result = calculate(expression, ans, angleUnit, defaultFrac);
            ans = result;
            result = roundResult(result, displayParameterFix, displayParameterSci, displayParameterNorm, displayMode);

            Serial.print("result: ");
            Serial.println(result);

            if (result.indexOf("x10^") != -1) {
              int expIdx = result.indexOf("x10^");
              String expStr = result.substring(expIdx + 4);
              int rawExponent = expStr.toInt();

              if (rawExponent == 0) {
                result = result.substring(0, expIdx);
                exponent2 = false;
                isExponentNegative = false;
                exponent = 0;
              } else {
                exponent2 = true;
                result = result.substring(0, expIdx);
                if (rawExponent < 0) {
                  isExponentNegative = true;
                  exponent = -rawExponent;
                } else {
                  isExponentNegative = false;
                  exponent = rawExponent;
                }
              }
            } else {
              exponent2 = false;
              isExponentNegative = false;
              exponent = 0;
            }
            Serial.print("ans: ");
            Serial.println(ans);

            int size = 10;
            for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
            result = result.substring(0, size);
          }
          dot = 0;
        }
      }

    } while(pressedBtn == "" || !isNumber(pressedBtn) || pressedBtn == "MODE");

    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    saveToEEPROM();
  }
  else if (pressedBtn == "CLR") {
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    shift = false;
    alpha = false;
    hyp = false;
    tft.setTextSize(1);
    tft.setFont(&calcFont);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(0, 155);
    tft.println("Mcl Mode All");
    tft.setFont(&Digital_736pt8b);
    tft.setCursor(0, 216);
    tft.println("1   2   3");

    int selected;
    pressedBtn = "";

    do {
      if(!isNumber(pressedBtn)) {
        do {
          pressedBtn = scanNumberKeypad();
        } while(pressedBtn == "" || !isNumber(pressedBtn));
      }
      selected =  pressedBtn.toInt();
      do {
        pressedBtn = scanNumberKeypad();
      } while(pressedBtn == "");
    } while(pressedBtn == "" || pressedBtn != "=");

    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));

    if(selected < 1) selected=1;
    else if(selected > 3) selected=3;

    String selectedName;
    if(selected==1) {
      //memory
      selectedName = "Mcl";
    }
    else if(selected==2) {
      //mode
      calcMode = 0;
      angleUnit = 0;
      displayMode = 2;
      displayParameterFix = 2;
      displayParameterSci = 2;
      displayParameterNorm = 2;
      defaultFrac = true;
      selectedName = "Mode";
    }
    else {
      //memory+mode
      calcMode = 0;
      angleUnit = 0;
      displayMode = 2;
      displayParameterFix = 2;
      displayParameterSci = 2;
      displayParameterNorm = 2;
      defaultFrac = true;
      selectedName = "All";
    }

    tft.setFont(&calcFont);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(0, 155);
    tft.print("Reset ");
    tft.println(selectedName);
    tft.setFont(&Digital_736pt8b);
    tft.setCursor(0, 216);
    tft.println("----------");

    if(result != "0"){
      result = roundResult(ans, displayParameterFix, displayParameterSci, displayParameterNorm, displayMode);

      Serial.print("result: ");
      Serial.println(result);

      if (result.indexOf("x10^") != -1) {
        int expIdx = result.indexOf("x10^");
        String expStr = result.substring(expIdx + 4);
        int rawExponent = expStr.toInt();

        if (rawExponent == 0) {
          result = result.substring(0, expIdx);
          exponent2 = false;
          isExponentNegative = false;
          exponent = 0;
        } else {
          exponent2 = true;
          result = result.substring(0, expIdx);
          if (rawExponent < 0) {
            isExponentNegative = true;
            exponent = -rawExponent;
          } else {
            isExponentNegative = false;
            exponent = rawExponent;
          }
        }
      } else {
        exponent2 = false;
        isExponentNegative = false;
        exponent = 0;
      }
      Serial.print("ans: ");
      Serial.println(ans);

      int size = 10;
      for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
      result = result.substring(0, size);
    }
    dot = 0;

    do {
      pressedBtn = scanNumberKeypad();
    } while(pressedBtn == "" || pressedBtn != "=");

    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    saveToEEPROM();
  }
  else if(pressedBtn == "SPEC") {
    spec = true;
    alpha = false;
    tft.fillScreen(ILI9341_BLACK);
    return;
  }
  else if(pressedBtn == "hyp ") {
    hyp = !hyp;
  }
  else if(pressedBtn == "rcl") {
    rcl = !rcl;
    sto = false;
    shift = false;
    alpha = false;
    hyp = false;
  }
  else if(pressedBtn == "sto") {
    sto = !sto;
    rcl = false;
    shift = false;
    alpha = false;
    hyp = false;
  }
  else if(pressedBtn == "eng ") {
    String inp = result + "x10^";
    if(isExponentNegative) inp += "-";
    inp += exponent;
    if(shift) {
      result = processSHIFT_ENG(inp);
    }else {
      result = processENG(inp);
    }

    Serial.print("result: ");
    Serial.println(result);

    if (result.indexOf("x10^") != -1) {
      int expIdx = result.indexOf("x10^");
      String expStr = result.substring(expIdx + 4);
      int rawExponent = expStr.toInt();

      if (rawExponent == 0) {
        exponent2 = true; 
        result = result.substring(0, expIdx); 
        isExponentNegative = false;
        exponent = 0;
      } else {
        exponent2 = true;
        result = result.substring(0, expIdx);
        if (rawExponent < 0) {
          isExponentNegative = true;
          exponent = -rawExponent;
        } else {
          isExponentNegative = false;
          exponent = rawExponent;
        }
      }
    } else {
      exponent2 = false;
      isExponentNegative = false;
      exponent = 0;
    }
    Serial.print("ans: ");
    Serial.println(ans);

    int size = 10;
    for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
    result = result.substring(0, size);
    
    shift = false;
    alpha = false;
    hyp = false;
    dot = 0;
  }
  else if (pressedBtn == "DRG") {
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    shift = false;
    alpha = false;
    hyp = false;
    tft.setTextSize(1);
    tft.setFont(&calcFont);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(0, 155);
    tft.println("D  R   G");
    tft.setFont(&Digital_736pt8b);
    tft.setCursor(0, 216);
    tft.println("1  2  3");

    do {
      pressedBtn = scanNumberKeypad();
    } while(pressedBtn == "" || !isNumber(pressedBtn));

    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));

    int selected =  pressedBtn.toInt();
    if(selected < 1) selected=1;
    else if(selected > 3) selected=3;

    String angleChar = (selected==1) ? "d" : (selected==2) ? "r" : "g";

    if(scrolling && selectedLogIdx >= 0) {
      int editPos = logStart[selectedLogIdx++];
      bool oldHasSpace = (editPos+1 < count && expression[editPos+1] == " ");

      if(ins) {
        int insertPos = editPos + (oldHasSpace ? 2 : 1);
        for (int k = count; k >= insertPos + 1; k--) {
          expression[k] = expression[k - 1];
        }
        expression[insertPos] = angleChar;
        count += 1;
      }
      else {
        expression[editPos] = angleChar;
        if(oldHasSpace) {
          for (int k = editPos + 1; k + 1 < count; k++) {
            expression[k] = expression[k + 1];
          }
          expression[count - 1] = "";
          count -= 1;
        }
      }
      lastPrintedLogIdx = -999;
    }
    else {
      expression[count++] = angleChar;
    }
  }
  else if(pressedBtn == "0x90" && !indicator2) {
    if(newResult == "") newResult = toggleDegreesAndDecimal(result);
    else newResult = toggleDegreesAndDecimal(newResult);
    newResult = roundResult(newResult, 0, 0, 2, 2);

    Serial.print("result: ");
    Serial.println(newResult);

    int size = 10;
    for(int i = 0; i < newResult.length(); i++) if(newResult[i] == '.') size = 11;
    newResult = newResult.substring(0, size);
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));

    dot = 0;
    shift = false;
    alpha = false;
  }
  else if(pressedBtn == "0xae" && !indicator2) {
    if(newResult == "") newResult = toggleFractionAndDecimal(result, defaultFrac);
    else newResult = toggleFractionAndDecimal(newResult, defaultFrac);
    newResult = roundResult(newResult, 0, 0, 2, 2);

    Serial.print("result: ");
    Serial.println(newResult);

    int size = 10;
    for(int i = 0; i < newResult.length(); i++) if(newResult[i] == '.') size = 11;
    newResult = newResult.substring(0, size);
    tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));

    dot = 0;
    shift = false;
    alpha = false;
  }
  else if(pressedBtn != ""){
    if(!indicator2) { 
      if(err == "") {
        for(int i = 0; i < count; i++) expression[i] = ""; 
        count = 0;
        lastCount = 0;
        result = "0";
        exponent = 0;
        exponent2 = false;
        isExponentNegative = false;
        dot = 0;
        ins = false;
        scrolling = false;
        selectedLogIdx = -1;
        windowFirstLog = 0;
        lastPrintedLogIdx = -999;
      }
      tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
    }
    if(!indicator2 && shouldPrependAns(pressedBtn) && err == "") expression[count++] = "Ans";

    if(pressedBtn[pressedBtn.length()-1] == ' ') {
      if(scrolling  && selectedLogIdx >= 0) {
        int editPos = logStart[selectedLogIdx++];
        bool oldHasSpace = (editPos+1 < count && expression[editPos+1] == " ");
        String newWord = pressedBtn.substring(0, pressedBtn.length()-1);

        if(ins) {
          int insertPos = editPos + (oldHasSpace ? 2 : 1);
          for (int k = count + 1; k >= insertPos + 2; k--) {
            expression[k] = expression[k - 2];
          }
          expression[insertPos] = newWord;
          expression[insertPos + 1] = " ";
          count += 2;
        }
        else {
          expression[editPos] = newWord;
          if(!oldHasSpace) {
            for (int k = count; k >= editPos + 2; k--) {
              expression[k] = expression[k - 1];
            }
            expression[editPos + 1] = " ";
            count += 1;
          }
        }
        lastPrintedLogIdx = -999;
      }
      else {
        expression[count++] = pressedBtn.substring(0, pressedBtn.length()-1);
        expression[count++] = " ";
      }
    }
    else {
      if(scrolling  && selectedLogIdx >= 0) {
        int editPos = logStart[selectedLogIdx++];
        bool oldHasSpace = (editPos+1 < count && expression[editPos+1] == " ");

        if(ins) {
          int insertPos = editPos + (oldHasSpace ? 2 : 1);
          for (int k = count; k >= insertPos + 1; k--) {
            expression[k] = expression[k - 1];
          }
          expression[insertPos] = pressedBtn;
          count += 1;
        }
        else {
          expression[editPos] = pressedBtn;
          if(oldHasSpace) {
            for (int k = editPos + 1; k + 1 < count; k++) {
              expression[k] = expression[k + 1];
            }
            expression[count - 1] = "";
            count -= 1;
          }
        }
        lastPrintedLogIdx = -999;
      }
      else {
        expression[count++] = pressedBtn;
      }
    }

    tft.fillRect(0, 110, 320, 17, tft.color565(150, 175, 130));
    shift = false;
    alpha = false;
    hyp = false;
    indicator2 = true;
    if(err != "") {
      err = "";
      result = "0";
      dot = 0;
      exponent = 0;
      exponent2 = false;
      isExponentNegative = false;
    }

    if(pressedBtn == "%" && !scrolling) {
      result = calculate(expression, ans, angleUnit, defaultFrac);
      if(result == "Syntax ERROR" || result == "Math ERROR") {
        err = result;
        result = ans;
      } else {
        ans = result;
        result = roundResult(result, displayParameterFix, displayParameterSci, displayParameterNorm, displayMode);

        Serial.print("result: ");
        Serial.println(result);

        if (result.indexOf("x10^") != -1) {
          int expIdx = result.indexOf("x10^");
          String expStr = result.substring(expIdx + 4);
          int rawExponent = expStr.toInt();

          if (rawExponent == 0) {
            result = result.substring(0, expIdx);
            exponent2 = false;
            isExponentNegative = false;
            exponent = 0;
          } else {
            exponent2 = true;
            result = result.substring(0, expIdx);
            if (rawExponent < 0) {
              isExponentNegative = true;
              exponent = -rawExponent;
            } else {
              isExponentNegative = false;
              exponent = rawExponent;
            }
          }
        } else {
          exponent2 = false;
          isExponentNegative = false;
          exponent = 0;
        }
        Serial.print("ans: ");
        Serial.println(ans);

        int size = 10;
        for(int i = 0; i < result.length(); i++) if(result[i] == '.') size = 11;
        result = result.substring(0, size);

        newResult = "";
        dot = 0;
        ins = false;
        spec = false;
      }
    }
  }

  if (pressedBtn == "=" || pressedBtn == "%" || pressedBtn == "MODE" || pressedBtn == "ON" || pressedBtn == "AC" || pressedBtn == "eng ") tft.fillRect(0, 105, 320, 135, tft.color565(150, 175, 130));
  else if (pressedBtn == "SHIFT" || pressedBtn == "ALPHA" || pressedBtn == "hyp " || pressedBtn == "INS") tft.fillRect(0, 110, 320, 17, tft.color565(150, 175, 130));
  else if(pressedBtn == "DEL") tft.fillRect(0, 125, 320, 35, tft.color565(150, 175, 130));

  tft.setFont();
  tft.setTextSize(1);

  ///////////////////////////tab///////////////////////////
  if(shift) {
    tft.fillRect(0, 116, 7, 9, ILI9341_BLACK);
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(1, 117);
    tft.print("S");
  } else if(alpha) {
    tft.fillRect(13, 116, 7, 9, ILI9341_BLACK);
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(14, 117);
    tft.print("A");
  }

  if(hyp) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(25, 117);
    tft.print("hyp");
  }

  if(M) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(45, 117);
    tft.print("M");
  }

  if(sto) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(55, 117);
    tft.print("STO");
  } else if(rcl) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(75, 117);
    tft.print("RCL");
  }

  if(angleUnit == 0) {
    tft.fillRect(190, 116, 7, 9, ILI9341_BLACK);
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(190+1, 117);
    tft.print("D");
  } else if(angleUnit == 1) {
    tft.fillRect(190+13, 116, 7, 9, ILI9341_BLACK);
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(190+14, 117);
    tft.print("R");
  } else if(angleUnit == 2) {
    tft.fillRect(190+26, 116, 7, 9, ILI9341_BLACK);
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(190+27, 117);
    tft.print("G");
  }

  if(displayMode == 0) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(231, 117);
    tft.print("FIX");
  } else if(displayMode == 1) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(256, 117);
    tft.print("SCI");
  }

  ///////////////////////////build logical tokens///////////////////////////
  logCount = 0;
  for (int i = 0; i < count; ) {
    if (expression[i] == " ") { i++; continue; }
    int visLen = getTokenVisualLength(i);
    int footprint = visLen;
    int consumed = 1;
    if (i+1 < count && expression[i+1] == " ") {
      footprint += 1;
      consumed = 2;
    }
    if (logCount < MAX_LOG_TOKENS) {
      logStart[logCount] = i;
      logVisLen[logCount] = visLen;
      logFootprint[logCount] = footprint;
      logCount++;
    }
    i += consumed;
  }
  if (selectedLogIdx >= logCount) selectedLogIdx = logCount - 1;
  if (selectedLogIdx < 0 && scrolling) scrolling = false;

  ///////////////////////////compute visible window///////////////////////////
  if (!scrolling || selectedLogIdx < 0) {
    windowFirstLog = computeFirstVisibleLogIdx(logCount - 1);
  } else {
    if (windowFirstLog > logCount - 1) windowFirstLog = computeFirstVisibleLogIdx(selectedLogIdx);
    int windowLastLogCheck = computeLastVisibleLogIdx(windowFirstLog);
    if (selectedLogIdx > windowLastLogCheck) {
      windowFirstLog = computeFirstVisibleLogIdx(selectedLogIdx);
    } else if (selectedLogIdx < windowFirstLog) {
      windowFirstLog = selectedLogIdx;
    }
  }
  if (windowFirstLog < 0) windowFirstLog = 0;
  int windowLastLog = computeLastVisibleLogIdx(windowFirstLog);

  if (scrolling && selectedLogIdx >= 0 && selectedLogIdx != lastPrintedLogIdx) {
    Serial.println(expression[logStart[selectedLogIdx]]);
    lastPrintedLogIdx = selectedLogIdx;
  }
  if (!scrolling) lastPrintedLogIdx = -999;

  ///////////////////////////exp///////////////////////////
  printedCharsCount = 0;

  tft.setFont(&calcFont);
  tft.setTextColor(ILI9341_BLACK);

  tft.setCursor(0, 155);
  if(err == "") {
    for (int t = windowFirstLog; t <= windowLastLog; t++) {
      int i = logStart[t];
      String p = expression[i];

      bool nothing = false;
      if(printedCharsCount == currentIndicatorPos && indicator && indicator2) nothing = true;

      for(int j = 0; j < p.length(); j++) {
        bool isHex = (j+3 < p.length() && p[j]=='0' && p[j+1]=='x' && !ispunct(p[j+2]));
        if(isHex && !nothing) {
          char c = (char)strtol(p.substring(j+2, j+4).c_str(), NULL, 16);
          tft.write(c);
        } else if(!isHex && !nothing) {
          tft.write(p[j]);
        } else {
          tft.setTextColor(tft.color565(150, 175, 130));
          if(j == 0) tft.write(ins? 0xBA : 0xB8);
          else tft.write(0xB6);
          tft.setTextColor(ILI9341_BLACK);
        }
        printedCharsCount++;
        if(pressedBtn != "") {
          tft.fillRect(0, 125, 320, 35, tft.color565(150, 175, 130));
          pressedBtn = "";
        }
        if(isHex) j += 3;
      }

      if (i+1 < count && expression[i+1] == " ") {
        tft.write(' ');
        printedCharsCount++;
      }
    }
  } else tft.println(err);

  ///////////////////////////position indicator///////////////////////////
  currentIndicatorPos = printedCharsCount;

  if(scrolling && selectedLogIdx >= 0) {
    currentIndicatorPos = logStartColRelative(selectedLogIdx, windowFirstLog);
  }

  if (currentIndicatorPos > 11) currentIndicatorPos = 11;
  if (currentIndicatorPos < 0) currentIndicatorPos = 0;

  if(lastCount != currentIndicatorPos) {
    tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(0, 155);
    for(int i = 0; i < lastCount; i++) {
      tft.print(' ');
    }
    if(ins) tft.write(0xB9);
    else tft.print('_'); 
    lastCount = currentIndicatorPos;
  }

  if(((now-last > 300 && !indicator) || (now-last > 600 && indicator)) && indicator2) {
    last = now;
    indicator = !indicator;
    if(indicator) tft.setTextColor(ILI9341_BLACK);
    else tft.setTextColor(tft.color565(150, 175, 130));
    tft.setCursor(0, 155);
    
    for(int i = 0; i < currentIndicatorPos; i++) {
      tft.print(' ');
    }
    if(ins) tft.write(0xB9);
    else tft.print('_');
  }

  ///////////////////////////result///////////////////////////
  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(&Digital_736pt8b);

  String r;
  if(newResult != "") r = newResult;
  else r = result;

  for(int i = 0; i < r.length(); i++) {
    if(r[i] == '.') {
      r.remove(i, 1);
      dot = i-r.length();
      break;
    }
  }

  if(err == "") {
    tft.setCursor((10-r.length())*29, 216);
    tft.println(r);
    tft.fillCircle(25+29*(9+dot), 217, 2, ILI9341_BLACK);
  }

  if(exponent2) {
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(1);

    tft.setFont(&digital_7_regular14pt8b);
    tft.setCursor(295, 189);
    if(exponent<10) tft.print("0");
    tft.println(exponent);

    tft.setFont();
    tft.setCursor(287, 195);
    tft.println("X10"); 

    if(isExponentNegative) {
      tft.setCursor(288, 178);
      tft.println("-"); 
    }
  }
  delay(5);
}

String scanFunctionKeypad() {
  static String lastPressed = "";
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  String currentPressed = "";

  for (int r = 0; r < ROW1_NUM; r++) {
    pinMode(row1Pins[r], OUTPUT);
    digitalWrite(row1Pins[r], LOW);
    delayMicroseconds(1000);

    for (int c = 0; c < COL1_NUM; c++) {
      if (digitalRead(col1Pins[c]) == LOW) {
        currentPressed = shift ? keys1shift[r][c] : (alpha ? keys1alpha[r][c] : keys1[r][c]);

        //modifers
        if (hyp && currentPressed != "hyp ") {
          if (c == 2 && (r == 5 || r == 6 || r == 7) && !alpha) {
            currentPressed = currentPressed.substring(0,3) + 'h' + currentPressed.substring(3);
          }
        }
        goto release_row1;
      }
    }

    release_row1:
    pinMode(row1Pins[r], INPUT);
    delayMicroseconds(900);
  }

  String toReturn = "";
  if (currentPressed != "" && lastPressed == "") {
    lastDebounceTime = millis();
  }

  if (lastPressed != "" && currentPressed == "" && (millis() - lastDebounceTime > debounceDelay)) {
    toReturn = lastPressed;
  }

  lastPressed = currentPressed;
  return toReturn;
}

String scanNumberKeypad() {
  static String lastPressed = "";
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  String currentPressed = "";

  for (int r = 0; r < ROW2_NUM; r++) {
    pinMode(row2Pins[r], OUTPUT);
    digitalWrite(row2Pins[r], LOW);
    delayMicroseconds(900);

    for (int c = 0; c < COL2_NUM; c++) {
      if (digitalRead(col2Pins[c]) == LOW) {
        currentPressed = shift ? keys2shift[r][c] : keys2[r][c];
        goto release_row2;
      }
    }

    release_row2:
    pinMode(row2Pins[r], INPUT);
    delayMicroseconds(800);
  }

  String toReturn = "";
  if (currentPressed != "" && lastPressed == "") {
    lastDebounceTime = millis();
  }

  if (lastPressed != "" && currentPressed == "" && (millis() - lastDebounceTime > debounceDelay)) {
    toReturn = lastPressed;
    shift = false;
  }

  lastPressed = currentPressed;
  return toReturn;
} 

String scanSpecialKeypad() {
  static bool lastOn = false;
  String btn = "";

  bool nowOn = (digitalRead(WAKEUP_PIN) == LOW);

  if (lastOn == true && nowOn == false) {
    if (btn == "") {
      btn = "ON";
    }
  }
  lastOn = nowOn;

  return btn;
}


int getTokenVisualLength(int idx) {
  String p = expression[idx];
  int len = 0;
  for (int j = 0; j < p.length(); j++) {
    if (j+3 < p.length() && p[j]=='0' && p[j+1]=='x' && !ispunct(p[j+2])) {
      len++;
      j += 3;
    } else {
      len++;
    }
  }
  return len;
}

int computeFirstVisibleLogIdx(int lastLogIdxInclusive) {
  int total = 0;
  int first = lastLogIdxInclusive + 1;
  for (int k = lastLogIdxInclusive; k >= 0; k--) {
    int fp = logFootprint[k];
    if (total + fp > 11) break;
    total += fp;
    first = k;
  }
  return first;
}

int computeLastVisibleLogIdx(int firstLogIdxInclusive) {
  int total = 0;
  int last = firstLogIdxInclusive - 1;
  for (int k = firstLogIdxInclusive; k < logCount; k++) {
    int fp = logFootprint[k];
    if (total + fp > 11) break;
    total += fp;
    last = k;
  }
  return last;
}

int logStartColRelative(int logIdx, int fromLog) {
  int col = 0;
  for (int k = fromLog; k < logIdx; k++) col += logFootprint[k];
  return col;
}

void saveToHistory(int slot) {
  for (int i = 0; i < 50; i++) {
    expressionHistory[slot][i] = (i < count) ? expression[i] : "";
  }
  historyCount[slot] = count;
}

void loadFromHistory(int slot) {
  for (int i = 0; i < 50; i++) {
    expression[i] = expressionHistory[slot][i];
  }
  count = historyCount[slot];
  scrolling = false;
  selectedLogIdx = -1;
  windowFirstLog = 0;
  lastPrintedLogIdx = -999;
  tft.fillRect(0, 125, 320, 65, tft.color565(150, 175, 130));
}

void pushHistory() {
  if (count <= 0) return;

  if (historyEntries >= MAX_HISTORY) {
    for (int i = 0; i < MAX_HISTORY - 1; i++) {
      for (int j = 0; j < 50; j++) {
        expressionHistory[i][j] = expressionHistory[i+1][j];
      }
      historyCount[i] = historyCount[i+1];
    }
    historyEntries = MAX_HISTORY - 1;
  }

  saveToHistory(historyEntries);
  historyEntries++;
  historyIndex = -1;
}

void clearHistory() {
  for (int i = 0; i < MAX_HISTORY; i++) {
    for (int j = 0; j < 50; j++) {
      expressionHistory[i][j] = "";
    }
    historyCount[i] = 0;
  }
  historyEntries = 0;
  historyIndex = -1;
  scrolling2 = false;
}