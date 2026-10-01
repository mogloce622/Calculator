void special(String pressed_btn) {
  unsigned long currentMillis = millis();
  battery = getBatteryPercentage();

  static bool ansActive = false;
  static unsigned long lastBlinkTime = 0;
  static bool cursorVisible = true;

  if (pressed_btn == "ON") {
    spec = false;
    specialMenuState = 0;
    ansActive = false;
    tft.fillScreen(tft.color565(150, 175, 130));
    return;
  }

  if (pressed_btn == "Ans") {
    ansActive = !ansActive;

    if (ansActive) {
      tft.fillScreen(tft.color565(150, 175, 130));

      tft.setFont();
      tft.setTextSize(1);
      
      tft.fillRect(190, 116, 7, 9, ILI9341_BLACK);
      tft.setTextColor(tft.color565(150, 175, 130));
      tft.setCursor(191, 117);
      tft.print("D");

      tft.setFont(&calcFont);
      tft.setTextColor(ILI9341_BLACK);
      tft.setCursor(0, 155);
      tft.print("12");
      tft.write((char)0xB7);
      tft.print("5");

      String r = "60.00";
      int dot = 0;

      for (int i = 0; i < r.length(); i++) {
        if (r[i] == '.') {
          r.remove(i, 1);
          dot = i - r.length();
          break;
        }
      }

      tft.setTextColor(ILI9341_BLACK);
      tft.setFont(&Digital_736pt8b);
      tft.setCursor((10 - r.length()) * 29, 216);
      tft.println(r);
      
      if (dot != 0) {
        tft.fillCircle(25 + 29 * (9 + dot), 217, 2, ILI9341_BLACK);
      }

      bool exponent2 = true;
      int exponent = 2;
      bool isExponentNegative = false;

      if (exponent2) {
        tft.setTextColor(ILI9341_BLACK);
        tft.setTextSize(1);

        tft.setFont(&digital_7_regular14pt8b);
        tft.setCursor(295, 189);
        if (exponent < 10) tft.print("0");
        tft.println(exponent);

        tft.setFont();
        tft.setCursor(287, 195);
        tft.println("X10");

        if (isExponentNegative) {
          tft.setCursor(288, 178);
          tft.println("-");
        }
      }

    } else {
      tft.fillScreen(ILI9341_BLACK);
    }
  }

  if (ansActive) {
    if (currentMillis - lastBlinkTime > 500) {
      lastBlinkTime = currentMillis;
      cursorVisible = !cursorVisible;

      tft.setFont(&calcFont);
      tft.setCursor(105, 155);

      if (cursorVisible) {
        tft.setTextColor(ILI9341_BLACK);
        tft.print('_');
      } else {
        tft.setTextColor(tft.color565(150, 175, 130));
        tft.print('_');
      }
    }
    return;
  }

  if (specialMenuState == 0) {
    tft.setFont(); 
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); 

    tft.setTextSize(1);
    tft.setCursor(280, 110);
    tft.print(battery);
    tft.print("% ");

    tft.setTextSize(2);
    
    int xCoords[2] = {10, 170};
    int yCoords[5] = {120, 142, 164, 186, 208};

    for (int i = 0; i < 9; i++) {
      int col = i / 5;
      int row = i % 5;
      
      tft.setCursor(xCoords[col], yCoords[row]);
      tft.print(String(i + 1) + ".");
      
      if (i == 8) {
        if (wifiStatus) {
          tft.print("WiFi OFF");
        } else {
          tft.print("WiFi ON ");
        }
      } else {
        tft.print(menuItems[i]);
      }
    }

    if (pressed_btn != "") {
      if (pressed_btn == "1") {
        specialMenuState = 1;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "2") {
        specialMenuState = 2;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "3") {
        specialMenuState = 3;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "4") {
        specialMenuState = 4;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "5") {
        specialMenuState = 5;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "6") {
        specialMenuState = 6;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "7") {
        specialMenuState = 7;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "8") {
        specialMenuState = 8;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "9") {
        wifiStatus = !wifiStatus;
        tft.fillRect(170, 208, 140, 20, ILI9341_BLACK); 
        
        if (wifiStatus) {
          WiFi.mode(WIFI_STA);
          WiFi.begin(ssid.c_str(), password.c_str());
          Serial.print(ssid);
          Serial.print(' ');
          Serial.print(password);
          
          int brojac = 0;
          while (WiFi.status() != WL_CONNECTED && brojac < 20) {
            Serial.print('.');
            delay(500);
            brojac++;
          }
          Serial.println();
          
          if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            wifiStatus = false;
          } else Serial.println("Connected!");
        } else {
          WiFi.disconnect(true);
          WiFi.mode(WIFI_OFF);
        }
      }
    }
  }
  else if (specialMenuState == 1) {
    if (pressed_btn == "AC") {
      specialMenuState = 0;
      activeNote = -1;
      selectedNoteIdx = 0;
      tft.fillScreen(ILI9341_BLACK);
      return;
    }

    if (activeNote == -1) {
      tft.setFont();
      tft.setTextSize(2);

      int xCoords[2] = {10, 170};
      int yCoords[5] = {120, 142, 164, 186, 208};

      int arraySize = 0;
      while(noteTitles[arraySize] != "") arraySize++;

      if (pressed_btn == "up") {
        selectedNoteIdx--;
        if (selectedNoteIdx < 0) selectedNoteIdx = arraySize - 1;
        tft.fillScreen(ILI9341_BLACK); 
      }
      else if (pressed_btn == "down") {
        selectedNoteIdx++;
        if (selectedNoteIdx >= arraySize) selectedNoteIdx = 0;
        tft.fillScreen(ILI9341_BLACK);
      }
      else if (pressed_btn == "=") {
        activeNote = selectedNoteIdx;
        tft.fillScreen(ILI9341_BLACK);
        return;
      }

      int maxVisibleRows = 5;
      int startIdx = 0;
      if (selectedNoteIdx >= maxVisibleRows) {
        startIdx = selectedNoteIdx - maxVisibleRows + 1;
      }

      int printCount = 0;
      for (int i = startIdx; i < arraySize && printCount < maxVisibleRows; i++) {
        int col = printCount / 5;
        int row = printCount % 5;
        if (row > 4 || col > 1) continue;
        tft.setCursor(xCoords[col], yCoords[row]);
       
        if (i == selectedNoteIdx) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.print("> " + noteTitles[i] + " ");
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print("  " + noteTitles[i] + " ");
        }
        printCount++;
      }
    }
    else {
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(10, 115);
      tft.print(noteTitles[activeNote]);

      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
     
      int startY = 140;
      int lineHeight = 12;
      String currentLine = "";
      int currentY = startY;
     
      for (int i = 0; i < noteTexts[activeNote].length(); i++) {
        currentLine += noteTexts[activeNote][i];
        if (currentLine.length() >= 50 || noteTexts[activeNote][i] == '\n' || i == noteTexts[activeNote].length() - 1) {
          tft.setCursor(10, currentY);
          tft.print(currentLine);
          currentLine = "";
          currentY += lineHeight;
          if (currentY > 230) break;
        }
      }

      if (pressed_btn == "DEL") {
        activeNote = -1;
        tft.fillScreen(ILI9341_BLACK);
      }
    }
  }
  else if (specialMenuState == 2) {
    if (pressed_btn == "AC") {
      specialMenuState = 0;
      activeFormula = -1;
      selectedFormulaIdx = 0;
      tft.fillScreen(ILI9341_BLACK);
      return;
    }

    if (activeFormula == -1) {
      tft.setFont();
      tft.setTextSize(2);

      int xCoords[2] = {10, 170};
      int yCoords[5] = {120, 142, 164, 186, 208};

      int arraySize = 0;
      while(arraySize < 30 && formulaTitles[arraySize] != "") arraySize++;

      if (pressed_btn == "up") {
        selectedFormulaIdx--;
        if (selectedFormulaIdx < 0) selectedFormulaIdx = arraySize - 1;
        tft.fillScreen(ILI9341_BLACK); 
      }
      else if (pressed_btn == "down") {
        selectedFormulaIdx++;
        if (selectedFormulaIdx >= arraySize) selectedFormulaIdx = 0;
        tft.fillScreen(ILI9341_BLACK); 
      }
      else if (pressed_btn == "=") {
        activeFormula = selectedFormulaIdx;
        tft.fillScreen(ILI9341_BLACK);
        return;
      }

      int maxVisibleRows = 5;
      int startIdx = 0;
      if (selectedFormulaIdx >= maxVisibleRows) {
        startIdx = selectedFormulaIdx - maxVisibleRows + 1;
      }

      int printCount = 0;
      for (int i = startIdx; i < arraySize && printCount < maxVisibleRows; i++) {
        int col = printCount / 5;
        int row = printCount % 5;
        if (row > 4 || col > 1) continue;
        tft.setCursor(xCoords[col], yCoords[row]);
       
        if (i == selectedFormulaIdx) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.print("> " + formulaTitles[i] + " ");
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print("  " + formulaTitles[i] + " ");
        }
        printCount++;
      }
    }
    else {
      tft.setFont();
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(10, 115);
      tft.print(formulaTitles[activeFormula]);

      tft.setFont(&calcFontSmall);
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

      String rawFormula = formulaTexts[activeFormula];
      String leftSide = "";
      String rightSide = rawFormula;
      Serial.println(rawFormula);

      int eqIndex = rawFormula.indexOf('=');
      if (eqIndex != -1) {
        leftSide = rawFormula.substring(0, eqIndex + 1);
        rightSide = rawFormula.substring(eqIndex + 1);
      }

      int startX = 10;
      int baseLineY = 175;

      if (leftSide != "") {
        for (int i = 0; i < leftSide.length(); i++) {
          if (leftSide[i] == ' ') continue;
          tft.setCursor(startX, baseLineY + 6);
          tft.print(leftSide[i]);
          startX += 17;
        }
      }

      String formatted = "";
      for (int i = 0; i < rightSide.length(); i++) {
        if (rightSide[i] == ' ') continue;
       
        if (rightSide.substring(i, i + 2) == "P2") { formatted += (char)0x92; i++; }
        else if (rightSide.substring(i, i + 2) == "P3") { formatted += (char)0x93; i++; }
        else if (rightSide.substring(i, i + 3) == "-01") { formatted += (char)0xAA; i += 2; }
        else if (rightSide.substring(i, i + 4) == "ROOT") { formatted += (char)0xAB; i += 3; }
        else if (rightSide.substring(i, i + 2) == "PI") { formatted += (char)0xAD; i++; }
        else if (rightSide.substring(i, i + 3) == "NEG") { formatted += (char)0xAF; i += 2; }
        else if (rightSide.substring(i, i + 4) == "EULR") { formatted += (char)0xB1; i += 3; }
        else if (rightSide.substring(i, i + 3) == "10X") { formatted += (char)0xB5; i += 2; }
        else if (rightSide.substring(i, i + 4) == "MULT") { formatted += (char)0xB7; i += 3; }
        else { formatted += rightSide[i]; }
      }

      int slashIndex = -1;
      for (int i = 0; i < formatted.length(); i++) {
        if (formatted[i] == '/') {
          bool isPlusMinus = false;
          if (i > 0 && formatted[i-1] == '+') {
            if (i + 1 < formatted.length() && formatted[i+1] == '-') {
              isPlusMinus = true;
            }
          }
          if (!isPlusMinus) {
            slashIndex = i;
            break;
          }
        }
      }

      if (slashIndex != -1) {
        String numerator = formatted.substring(0, slashIndex);
        String denominator = formatted.substring(slashIndex + 1);

        int numLen = numerator.length();
        int denLen = denominator.length();
        int maxLen = (numLen > denLen) ? numLen : denLen;
        int maxWidth = maxLen * 17;

        tft.fillRect(startX, baseLineY, maxWidth, 3, ILI9341_WHITE);

        int numStartOffsetX = ((maxLen - numLen) * 17) / 2;
        for (int i = 0; i < numLen; i++) {
          tft.setCursor(startX + numStartOffsetX + (i * 17), baseLineY - 14);
          tft.print(numerator[i]);
        }

        int denStartOffsetX = ((maxLen - denLen) * 17) / 2;
        for (int i = 0; i < denLen; i++) {
          tft.setCursor(startX + denStartOffsetX + (i * 17), baseLineY + 22);
          tft.print(denominator[i]);
        }
      }
      else {
        for (int i = 0; i < formatted.length(); i++) {
          tft.setCursor(startX + (i * 17), baseLineY + 6);
          tft.print(formatted[i]);
        }
      }

      if (pressed_btn == "DEL") {
        activeFormula = -1;
        tft.fillScreen(ILI9341_BLACK);
      }
    }
  }
  else if (specialMenuState == 3) {
    if (pressed_btn == "AC") {
      if (convStep > 0) {
        convStep--;
        if (convStep == 2) {
          convInput = "";
          convResult = "";
        }
        tft.fillScreen(ILI9341_BLACK);
        return;
      } else {
        specialMenuState = 0;
        convStep = 0; activeCatIdx = 0; fromUnitIdx = 0; toUnitIdx = 0; convInput = ""; convResult = "";
        tft.fillScreen(ILI9341_BLACK);
        return;
      }
    }

    if (convStep == 0) {
      tft.setFont(); tft.setTextSize(2);

      if (pressed_btn == "up") { activeCatIdx--; if (activeCatIdx < 0) activeCatIdx = 8; } 
      else if (pressed_btn == "down") { activeCatIdx++; if (activeCatIdx > 8) activeCatIdx = 0; }
      else if (pressed_btn == "=") { convStep = 1; fromUnitIdx = 0; tft.fillScreen(ILI9341_BLACK); return; }

      int xCoords[2] = {10, 145};
      int yCoords[5] = {120, 142, 164, 186, 208};

      for (int i = 0; i < 9; i++) {
        int col = i / 5; int row = i % 5;
        tft.setCursor(xCoords[col], yCoords[row]);
        if (i == activeCatIdx) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.print("> " + convCategories[i]);
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print("  " + convCategories[i]);
        }
      }
    }
    else if (convStep == 1) {
      tft.setFont(); tft.setTextSize(2);
      tft.setCursor(10, 115); tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print("Convert FROM:");

      int maxFromIdx = 14;
      while (maxFromIdx > 0 && unitsMatrix[activeCatIdx][maxFromIdx] == "") {
        maxFromIdx--;
      }

      if (pressed_btn == "up") { fromUnitIdx--; if (fromUnitIdx < 0) fromUnitIdx = maxFromIdx; } 
      else if (pressed_btn == "down") { fromUnitIdx++; if (fromUnitIdx > maxFromIdx) fromUnitIdx = 0; }
      else if (pressed_btn == "=") { convStep = 2; toUnitIdx = 0; tft.fillScreen(ILI9341_BLACK); return; }
      else if (pressed_btn == "DEL") { convStep = 0; tft.fillScreen(ILI9341_BLACK); return; }

      int xCoords[4] = {5, 85, 165, 245};
      int yCoords[4] = {140, 165, 190, 215};

      for (int i = 0; i <= maxFromIdx; i++) {
        int col = i / 4; int row = i % 4;
        tft.setCursor(xCoords[col], yCoords[row]);
        if (i == fromUnitIdx) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print(">" + unitsMatrix[activeCatIdx][i]);
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.print(" " + unitsMatrix[activeCatIdx][i]);
        }
      }
    }
    else if (convStep == 2) {
      tft.setFont(); tft.setTextSize(2);
      tft.setCursor(10, 115); tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print("Convert TO:  ");

      int maxToIdx = 14;
      while (maxToIdx > 0 && unitsMatrix[activeCatIdx][maxToIdx] == "") {
        maxToIdx--;
      }

      if (pressed_btn == "up") { toUnitIdx--; if (toUnitIdx < 0) toUnitIdx = maxToIdx; } 
      else if (pressed_btn == "down") { toUnitIdx++; if (toUnitIdx > maxToIdx) toUnitIdx = 0; }
      else if (pressed_btn == "=") { convStep = 3; convInput = ""; convResult = ""; tft.fillScreen(ILI9341_BLACK); return; }
      else if (pressed_btn == "DEL") { convStep = 1; tft.fillScreen(ILI9341_BLACK); return; }

      int xCoords[4] = {5, 85, 165, 245};
      int yCoords[4] = {140, 165, 190, 215};

      for (int i = 0; i <= maxToIdx; i++) {
        int col = i / 4; int row = i % 4;
        tft.setCursor(xCoords[col], yCoords[row]);
        if (i == toUnitIdx) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print(">" + unitsMatrix[activeCatIdx][i]);
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.print(" " + unitsMatrix[activeCatIdx][i]);
        }
      }
    }
    else if (convStep == 3) {
      tft.setFont(); tft.setTextSize(2);
      
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(10, 115);
      tft.print(convCategories[activeCatIdx] + " (" + unitsMatrix[activeCatIdx][fromUnitIdx] + "->" + unitsMatrix[activeCatIdx][toUnitIdx] + ")");

      if (pressed_btn != "") {
        tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
        tft.setCursor(10, 150); tft.print("IN:  " + (convInput == "" ? "0" : convInput));
        tft.setCursor(10, 180); tft.print("OUT: " + convResult);

        if (pressed_btn == "DEL") {
          if (convInput.length() > 0) {
            convInput.remove(convInput.length() - 1);
            convResult = ""; 
          } else {
            convStep = 2;
            tft.fillScreen(ILI9341_BLACK);
            return;
          }
        }
        else if (pressed_btn == "=") {
          String displayInput = convInput;
          if (displayInput == "") displayInput = "0";
          convResult = convertValue(convCategories[activeCatIdx], unitsMatrix[activeCatIdx][fromUnitIdx], unitsMatrix[activeCatIdx][toUnitIdx], displayInput);
        }
        else if (pressed_btn == "0xd7") {
          convInput += "/";
          convResult = "";
        }
        else if (pressed_btn == "0xaf") {
          convInput += "A";
          convResult = "";
        }
        else if (pressed_btn == "0x90") {
          convInput += "B";
          convResult = "";
        }
        else if (pressed_btn == "hyp ") {
          convInput += "C";
          convResult = "";
        }
        else if (pressed_btn == "sin ") {
          convInput += "D";
          convResult = "";
        }
        else if (pressed_btn == "cos ") {
          convInput += "E";
          convResult = "";
        }
        else if (pressed_btn == "tan ") {
          convInput += "F";
          convResult = "";
        }
        else if (pressed_btn == "0" || pressed_btn == "1" || pressed_btn == "2" || pressed_btn == "3" || 
                 pressed_btn == "4" || pressed_btn == "5" || pressed_btn == "6" || pressed_btn == "7" || 
                 pressed_btn == "8" || pressed_btn == "9" || pressed_btn == ".") {
          convInput += pressed_btn;
          convResult = ""; 
        }
      }

      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(10, 150);
      tft.print("IN:  " + (convInput == "" ? "0" : convInput));
      
      if (convResult != "") {
        tft.setCursor(10, 180);
        tft.print("OUT: " + convResult);
      }
    }
  }
  else if (specialMenuState == 4) {
    if (pressed_btn == "AC") {
      specialMenuState = 0;
      numExpr = ""; numDisplay = ""; currentBaseStr = ""; numResult = "";
      enteringBase = true;
      showCursor = true;
      engMode = false;
      tft.fillScreen(ILI9341_BLACK);
      return;
    }

    tft.setFont();

    if (pressed_btn != "") {
      tft.fillRect(10, 110, 300, 121, ILI9341_BLACK);
      
      if (pressed_btn != "=" && pressed_btn != "M+" && pressed_btn != "eng ") {
        showCursor = true;
      }

      if (pressed_btn == "DEL") {
        if (enteringBase) {
          if (currentBaseStr.length() > 0) currentBaseStr.remove(currentBaseStr.length() - 1);
        } else {
          if (numExpr.length() > 0) {
            if (numExpr.endsWith("]")) {
              int openBrace = numExpr.lastIndexOf('[');
              currentBaseStr = numExpr.substring(openBrace + 1, numExpr.length() - 1);
              numExpr.remove(openBrace);
              enteringBase = true;
            } else {
              numExpr.remove(numExpr.length() - 1);
            }
          }
        }
        numResult = "";
      }
      else if (pressed_btn == "M+") {
        if (numResult != "") {
          if (currentBaseStr == "" || currentBaseStr == "10") currentBaseStr = "2";
          else if (currentBaseStr == "2") currentBaseStr = "8";
          else if (currentBaseStr == "8") currentBaseStr = "16";
          else if (currentBaseStr == "16") currentBaseStr = "10";
        }
      }
      else if (pressed_btn == "eng ") {
        if (numResult != "") {
          engMode = !engMode;
        }
      }
      else if (pressed_btn == "," && enteringBase) {
        if (currentBaseStr == "") currentBaseStr = "10";
        numExpr += "[" + currentBaseStr + "]";
        enteringBase = false;
      }
      else if (pressed_btn == "=") {
        numResult = String(evaluateExpression(numExpr), 6);
        showCursor = false;
        currentBaseStr = "10"; 
      }
      else if (pressed_btn == "+" || pressed_btn == "-" || pressed_btn == "0xb7" || pressed_btn == "0xd7" || pressed_btn == "(" || pressed_btn == ")") {
        if (pressed_btn == "0xb7") numExpr += "*";
        else if (pressed_btn == "0xd7") numExpr += "/";
        else numExpr += pressed_btn;
        
        enteringBase = true;
        currentBaseStr = "";
        numResult = "";
      }
      else {
        String charToInput = pressed_btn;
        if (pressed_btn == "0xaf") charToInput = "A";
        else if (pressed_btn == "0x90") charToInput = "B";
        else if (pressed_btn == "hyp ") charToInput = "C";
        else if (pressed_btn == "sin ") charToInput = "D";
        else if (pressed_btn == "cos ") charToInput = "E";
        else if (pressed_btn == "tan ") charToInput = "F";

        if (enteringBase) {
          if (charToInput >= "0" && charToInput <= "9") {
            currentBaseStr += charToInput;
          }
        } else {
          if (isHexChar(charToInput[0]) || charToInput == ".") {
            numExpr += charToInput;
          }
        }
        if (pressed_btn != "M+" && pressed_btn != "eng ") {
          numResult = "";
        }
      }
    }

    int totalRequiredWidth = 0;
    int lastBlockStartWidth = 0; 
    
    int idx = 0;
    while (idx < numExpr.length()) {
      if (numExpr[idx] == '[') {
        int endIdx = numExpr.indexOf(']', idx);
        String b = numExpr.substring(idx + 1, endIdx);
        totalRequiredWidth += (b.length() * 12) + 4; 
        idx = endIdx + 1;
      } else {
        if (numExpr[idx] == '+' || numExpr[idx] == '-' || numExpr[idx] == '*' || numExpr[idx] == '/' || numExpr[idx] == '(' || numExpr[idx] == ')') {
          lastBlockStartWidth = totalRequiredWidth; 
        }
        totalRequiredWidth += 18 + 2; 
        idx++;
      }
    }

    if (enteringBase) {
      int baseLen = (currentBaseStr == "") ? 1 : currentBaseStr.length();
      totalRequiredWidth += (baseLen * 12) + 4;
    } else {
      totalRequiredWidth += 18 + 2; 
    }

    int scrollX = 0;
    if (totalRequiredWidth > 290) {
      scrollX = lastBlockStartWidth; 
    }

    int currentX = 10 - scrollX;
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    
    int renderIdx = 0;
    while (renderIdx < numExpr.length()) {
      if (numExpr[renderIdx] == '[') {
        int endIdx = numExpr.indexOf(']', renderIdx);
        String b = numExpr.substring(renderIdx + 1, endIdx);
        
        int blockWidth = (b.length() * 12) + 4;
        if (currentX + blockWidth > 10 && currentX < 310) { 
          tft.setTextSize(2); 
          tft.setCursor(currentX, 140); 
          tft.print(b);
        }
        currentX += blockWidth;
        renderIdx = endIdx + 1;
      } else {
        if (currentX + 20 > 10 && currentX < 310) {
          tft.setTextSize(3); 
          tft.setCursor(currentX, 115); 
          tft.print(numExpr[renderIdx]);
        }
        currentX += 20;
        renderIdx++;
      }
    }
    
    if (showCursor) {
      if (enteringBase) {
        tft.setTextSize(2);
        tft.setCursor(currentX, 140);
        if (currentBaseStr == "") {
          if (currentX + 12 > 10 && currentX < 310) {
            tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
            tft.print("?"); 
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          }
        } else {
          if (currentX + (currentBaseStr.length() * 12) > 10 && currentX < 310) {
            tft.print(currentBaseStr);
            if ((millis() / 300) % 2) {
              tft.print("_");
            }
          }
        }
      } else {
        if (currentX + 20 > 10 && currentX < 310) {
          tft.setTextSize(3);
          tft.setCursor(currentX, 115);
          if ((millis() / 300) % 2) {
            tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
            tft.print("_");
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          }
        }
      }
    }

    if (numResult != "") {
      tft.setTextSize(3);
      tft.setCursor(10, 190);
      
      int outBase = currentBaseStr.toInt();
      if (outBase != 2 && outBase != 8 && outBase != 16) outBase = 10;

      double totalVal = numResult.toDouble();
      long cjeliDio = (long)totalVal;

      if (engMode) {
        int lastDivIdx = numExpr.lastIndexOf('/');
        double divisor = 1.0;
        int divBase = 10;
        
        if (lastDivIdx != -1) {
          int bStart = numExpr.lastIndexOf('[', lastDivIdx);
          if (bStart != -1) {
            divBase = numExpr.substring(bStart + 1, lastDivIdx).toInt();
          }
          int startDiv = lastDivIdx + 1;
          while (startDiv < numExpr.length() && numExpr[startDiv] == '[') {
            int closeB = numExpr.indexOf(']', startDiv);
            startDiv = closeB + 1;
          }
          int endDiv = startDiv;
          while (endDiv < numExpr.length() && ((numExpr[endDiv] >= '0' && numExpr[endDiv] <= '9') || (numExpr[endDiv] >= 'A' && numExpr[endDiv] <= 'F') || (numExpr[endDiv] >= 'a' && numExpr[endDiv] <= 'f') || numExpr[endDiv] == '.')) {
            endDiv++;
          }
          if (endDiv > startDiv) {
            divisor = anyBaseToDec(numExpr.substring(startDiv, endDiv), divBase);
          }
        }

        long ostatak = (long)round((totalVal - (double)cjeliDio) * divisor);

        if (outBase == 10) {
          tft.print("=" + String(cjeliDio) + " R " + String(ostatak));
        } else if (outBase == 2) {
          tft.print("=" + String(cjeliDio, BIN) + " R " + String(ostatak, BIN));
        } else if (outBase == 8) {
          tft.print("=" + String(cjeliDio, OCT) + " R " + String(ostatak, OCT));
        } else if (outBase == 16) {
          String hexCjeli = String(cjeliDio, HEX);
          String hexOstatak = String(ostatak, HEX);
          hexCjeli.toUpperCase();
          hexOstatak.toUpperCase();
          tft.print("=" + hexCjeli + " R " + hexOstatak);
        }
      } else {
        if (outBase == 10) {
          tft.print("=" + numResult);
        } else {
          if (outBase == 2) {
            tft.print("=" + String(cjeliDio, BIN));
          } else if (outBase == 8) {
            tft.print("=" + String(cjeliDio, OCT));
          } else if (outBase == 16) {
            String hexStr = String(cjeliDio, HEX);
            hexStr.toUpperCase();
            tft.print("=" + hexStr);
          }
        }
      }
    }
  }
  else if (specialMenuState == 5) {
    tft.setFont();

    if (gameState == 0) {
      if (pressed_btn == "up") {
        gameSelect--;
        if (gameSelect < 0) gameSelect = 2;
        tft.fillRect(10, 115, 300, 110, ILI9341_BLACK);
      }
      else if (pressed_btn == "down") {
        gameSelect++;
        if (gameSelect > 2) gameSelect = 0;
        tft.fillRect(10, 115, 300, 110, ILI9341_BLACK);
      }
      else if (pressed_btn == "=") {
        gameState = gameSelect + 1;
        tft.fillRect(0, 110, 320, 115, ILI9341_BLACK);
        
        if (gameState == 1) {
          snakeLen = 1;
          snakeX[0] = 160; snakeY[0] = 170;
          snakeDir = 1;
          foodX = 20 + (rand() % 26) * 10;
          foodY = 130 + (rand() % 9) * 10;
          lastSnakeMove = millis();
          
          tft.drawRect(10, 115, 300, 110, ILI9341_WHITE);
          tft.setTextSize(1);
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.setCursor(15, 118);
          tft.print("SCORE: 0");
        }
        else if (gameState == 2) {
          for (int i = 0; i < 9; i++) board[i] = 0;
          cursorXOX = 4;
          playerXOX = 1;

          tft.fillRect(0, 115, 20, 110, ILI9341_WHITE);

          tft.fillRect(grid_x + cell,          grid_y, thick, cell*3 + thick*2, ILI9341_WHITE);
          tft.fillRect(grid_x + cell*2 + thick, grid_y, thick, cell*3 + thick*2, ILI9341_WHITE);

          tft.fillRect(grid_x, grid_y + cell,          grid_w, thick, ILI9341_WHITE);
          tft.fillRect(grid_x, grid_y + cell*2 + thick, grid_w, thick, ILI9341_WHITE);

          int center_x = grid_x + (grid_w - small) / 2;
          int center_y = grid_y + (cell*3 + thick*2 - small) / 2;
          tft.drawRect(center_x, center_y, small, small, ILI9341_WHITE);
        }
        else if (gameState == 3) {
          for (int i = 0; i < 9; i++) board[i] = 0;
          p1Y = 150; p2Y = 150;
          ballX = 160; ballY = 170;
          ballVX = 5; ballVY = 3; 
          lastPongMove = millis();
          tft.drawRect(10, 115, 300, 110, ILI9341_WHITE);
        }
        return;
      }
      else if (pressed_btn == "AC") {
        specialMenuState = 0;
        tft.fillScreen(ILI9341_BLACK);
        return;
      }

      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(20, 115);
      tft.print("SELECT GAME:");

      tft.setTextSize(2);
      tft.setCursor(20, 140);
      if (gameSelect == 0) { tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print("> Snake"); }
      else { tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.print("  Snake"); }

      tft.setCursor(20, 165);
      if (gameSelect == 1) { tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print("> XOX"); }
      else { tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.print("  XOX"); }

      tft.setCursor(20, 190);
      if (gameSelect == 2) { tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.print("> Pong"); }
      else { tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.print("  Pong"); }
    }
    
    else if (gameState == 1) {
      if (pressed_btn == "AC") { 
        snakeLen = 1;
        gameState = 0; 
        tft.fillRect(0, 110, 320, 115, ILI9341_BLACK); 
        return; 
      }
      
      if (pressed_btn == "left" && (snakeLen == 1 || snakeDir != 1)) snakeDir = 0;
      else if (pressed_btn == "right" && (snakeLen == 1 || snakeDir != 0)) snakeDir = 1;
      else if (pressed_btn == "up" && (snakeLen == 1 || snakeDir != 3)) snakeDir = 2;
      else if (pressed_btn == "down" && (snakeLen == 1 || snakeDir != 2)) snakeDir = 3;

      if (millis() - lastSnakeMove > 200) {
        lastSnakeMove = millis();

        int oldTailX = snakeX[snakeLen - 1];
        int oldTailY = snakeY[snakeLen - 1];

        for (int i = snakeLen - 1; i > 0; i--) {
          snakeX[i] = snakeX[i - 1];
          snakeY[i] = snakeY[i - 1];
        }

        if (snakeDir == 0) snakeX[0] -= 10;
        else if (snakeDir == 1) snakeX[0] += 10;
        else if (snakeDir == 2) snakeY[0] -= 10;
        else if (snakeDir == 3) snakeY[0] += 10;

        if (snakeX[0] < 11 || snakeX[0] >= 300 || snakeY[0] < 116 || snakeY[0] >= 220) {
          gameState = 0; tft.fillRect(0, 110, 320, 115, ILI9341_BLACK); return;
        }

        for (int i = 1; i < snakeLen; i++) {
          if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameState = 0; tft.fillRect(0, 110, 320, 115, ILI9341_BLACK); return;
          }
        }

        tft.fillRect(oldTailX, oldTailY, 10, 10, ILI9341_BLACK);

        if (snakeX[0] == foodX && snakeY[0] == foodY) {
          if (snakeLen < 50) snakeLen++;
          foodX = 20 + (rand() % 26) * 10;
          foodY = 120 + (rand() % 9) * 10;
        }

        tft.fillRect(52, 118, 40, 8, ILI9341_BLACK);
        tft.setTextSize(1);
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setCursor(52, 118);
        tft.print(snakeLen - 1);
        tft.setCursor(15, 118);
        tft.print("SCORE:");

        tft.fillRect(foodX, foodY, 10, 10, ILI9341_RED);
        for (int i = 0; i < snakeLen; i++) {
          tft.fillRect(snakeX[i], snakeY[i], 10, 10, i == 0 ? 0x03E0 : ILI9341_GREEN);
        }
      }
    }
    
    else if (gameState == 2) {
      if (pressed_btn == "AC") { gameState = 0; tft.fillRect(0, 115, 320, 110, ILI9341_BLACK); return; }

      if (pressed_btn != "") {
        int oldCursor = cursorXOX;
        bool moved = false;

        if (pressed_btn == "left") { if (cursorXOX % 3 > 0) { cursorXOX--; moved = true; } }
        else if (pressed_btn == "right") { if (cursorXOX % 3 < 2) { cursorXOX++; moved = true; } }
        else if (pressed_btn == "up") { if (cursorXOX / 3 > 0) { cursorXOX -= 3; moved = true; } }
        else if (pressed_btn == "down") { if (cursorXOX / 3 < 2) { cursorXOX += 3; moved = true; } }
        else if (pressed_btn == "=") {
          if (board[cursorXOX] == 0) {
            board[cursorXOX] = playerXOX;
            
            int r = cursorXOX / 3;
            int c = cursorXOX % 3;
            
            int xpos = grid_x + c * (cell + thick);
            int ypos = grid_y + r * (cell + thick);
            
            tft.setTextSize(3);
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
            tft.setCursor(xpos + 10, ypos + 6);
            
            if (playerXOX == 1) tft.print("X");
            else tft.print("O");

            int win[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
            bool hasWon = false;
            for (int i = 0; i < 8; i++) {
              if (board[win[i][0]] == playerXOX && board[win[i][1]] == playerXOX && board[win[i][2]] == playerXOX) hasWon = true;
            }
            
            if (hasWon) {
              gameState = 0; tft.fillRect(0, 115, 320, 110, ILI9341_BLACK); return;
            }
            
            bool full = true;
            for (int i = 0; i < 9; i++) if (board[i] == 0) full = false;
            if (full) { gameState = 0; tft.fillRect(0, 115, 320, 110, ILI9341_BLACK); return; }
            
            playerXOX = (playerXOX == 1) ? 2 : 1;
            moved = true; 
          }
        }

        if (moved) {
          int oldR = oldCursor / 3;
          int oldC = oldCursor % 3;
          int oldX = grid_x + oldC * (cell + thick) + (cell - small) / 2;
          int oldY = grid_y + oldR * (cell + thick) + (cell - small) / 2;
          tft.drawRect(oldX, oldY, small, small, ILI9341_BLACK);

          if (playerXOX == 1) {
            tft.fillRect(0, 115, 20, 110, ILI9341_WHITE);
            tft.fillRect(300, 115, 20, 110, ILI9341_BLACK);
          } else {
            tft.fillRect(0, 115, 20, 110, ILI9341_BLACK);
            tft.fillRect(300, 115, 20, 110, ILI9341_WHITE);
          }

          int newR = cursorXOX / 3;
          int newC = cursorXOX % 3;
          int newX = grid_x + newC * (cell + thick) + (cell - small) / 2;
          int newY = grid_y + newR * (cell + thick) + (cell - small) / 2;
          tft.drawRect(newX, newY, small, small, ILI9341_WHITE);
        }
      }
    }
    
    else if (gameState == 3) {
      if (pressed_btn == "AC") { 
        for (int i = 0; i < 9; i++) board[i] = 0; 
        gameState = 0; 
        tft.fillRect(0, 110, 320, 115, ILI9341_BLACK); 
        return; 
      }

      if (pressed_btn == "4") { p1Y -= 10; if (p1Y < 116) p1Y = 116; }
      else if (pressed_btn == "1") { p1Y += 10; if (p1Y > 194) p1Y = 194; }
      
      if (pressed_btn == "6") { p2Y -= 10; if (p2Y < 116) p2Y = 116; }
      else if (pressed_btn == "3") { p2Y += 10; if (p2Y > 194) p2Y = 194; }

      if (millis() - lastPongMove > 10) {
        lastPongMove = millis();

        tft.fillRect(ballX, ballY, 6, 6, ILI9341_BLACK);
        tft.fillRect(11, 116, 6, 108, ILI9341_BLACK);
        tft.fillRect(303, 116, 6, 108, ILI9341_BLACK);

        ballX += ballVX;
        ballY += ballVY;

        if (ballY <= 116) { ballY = 116; ballVY = -ballVY; }
        if (ballY >= 218) { ballY = 218; ballVY = -ballVY; }

        if (ballX <= 17) {
          if (ballY + 6 >= p1Y - 4 && ballY <= p1Y + 25 + 4) {
            ballVX = -ballVX;
            if (ballVX > 0 && ballVX < 18) ballVX++; else if (ballVX < 0 && ballVX > -18) ballVX--;
            if (ballVY > 0 && ballVY < 18) ballVY++; else if (ballVY < 0 && ballVY > -18) ballVY--;
            ballX = 18; 
          } 
          else if (ballX < 11) {
            board[1]++; 
            ballX = 160; ballY = 170; 
            ballVX = -ballVX; 
            tft.fillRect(11, 116, 292, 108, ILI9341_BLACK); 
          }
        }

        if (ballX + 6 >= 303) {
          if (ballY + 6 >= p2Y - 4 && ballY <= p2Y + 25 + 4) {
            ballVX = -ballVX;
            if (ballVX > 0 && ballVX < 18) ballVX++; else if (ballVX < 0 && ballVX > -18) ballVX--;
            if (ballVY > 0 && ballVY < 18) ballVY++; else if (ballVY < 0 && ballVY > -18) ballVY--;
            ballX = 297; 
          } 
          else if (ballX + 6 > 309) {
            board[0]++; 
            ballX = 160; ballY = 170; 
            ballVX = -ballVX; 
            tft.fillRect(11, 116, 292, 108, ILI9341_BLACK); 
          }
        }

        tft.drawRect(10, 115, 300, 110, ILI9341_WHITE);
        tft.fillRect(11, p1Y, 6, 25, ILI9341_WHITE);
        tft.fillRect(303, p2Y, 6, 25, ILI9341_WHITE);
        tft.fillRect(ballX, ballY, 6, 6, ILI9341_GREEN);
        
        tft.setTextSize(1);
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setCursor(135, 118);
        tft.print(board[0]);
        tft.setCursor(175, 118);
        tft.print(board[1]);
      }
    }
  }
  else if (specialMenuState == 6) {
    const char keysLowercase[4][10] = {
      {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'},
      {'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'},
      {'u', 'v', 'w', 'x', 'y', 'z', ' ', '.', ',', '?'},
      {'-', '+', '*', '/', '=', '_', '(', ')', '[', ']'}
    };

    const char keysUppercase[4][10] = {
      {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'},
      {'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'},
      {'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '.', ',', '?'},
      {'-', '+', '*', '/', '=', '_', '(', ')', '[', ']'}
    };

    const char keysAlpha[4][10] = {
      {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
      {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'},
      {'-', '_', '=', '+', '[', ']', '{', '}', ';', ':'},
      {'\'', '"', '<', '>', '/', '\\', '|', '`', '~', '?'}
    };

    if(aiIntro) {
      tft.drawRGBBitmap(54, 140, image_data, 211, 50);
      aiIntro = false;
      delay(1000);
      tft.fillRect(0, 110, 320, 120, ILI9341_BLACK);
    }

    static int keyboardMode = 0; 

    if (pressed_btn == "AC") {
      aiViewingResponse = false;
      aiPrompt = "";
      aiResponse = "";
      specialMenuState = 0;
      tft.fillRect(0, 110, 320, 120, ILI9341_BLACK);
      aiIntro = true;
      return;
    }

    if (pressed_btn == "SHIFT") {
      if (keyboardMode == 1) keyboardMode = 0;
      else keyboardMode = 1;
      pressed_btn = "";
    }
    else if (pressed_btn == "ALPHA") {
      if (keyboardMode == 2) keyboardMode = 0;
      else keyboardMode = 2;
      pressed_btn = "";
    }

    if (aiViewingResponse) {
      static int aiScrollLine = 0;
      static int lastScrollLine = -1;
      static String lastResponse = "";

      if (aiResponse != lastResponse) {
        aiScrollLine = 0;
        lastScrollLine = -1;
        lastResponse = aiResponse;
      }

      if (pressed_btn == "up" || pressed_btn == "8") {
        if (aiScrollLine > 0) {
          aiScrollLine--;
        }
        pressed_btn = "";
      }
      else if (pressed_btn == "down" || pressed_btn == "5") {
        aiScrollLine++;
        pressed_btn = "";
      }
      else if (pressed_btn == "DEL") {
        aiViewingResponse = false;
        aiScrollLine = 0;
        lastScrollLine = -1;
        lastResponse = "";
        tft.fillRect(0, 110, 320, 122, ILI9341_BLACK);
        pressed_btn = "";
        return;
      }

      if (lastScrollLine != aiScrollLine) {
        lastScrollLine = aiScrollLine;

        tft.fillRect(0, 110, 320, 122, ILI9341_BLACK);
        tft.setFont();
        tft.setTextSize(1);
        tft.setTextWrap(false);
        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
        tft.setCursor(0, 110);
        tft.println("GEMINI RESPONSE");
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

        int currentLine = 0;
        int linesPrinted = 0;
        const int maxVisibleLines = 12;
        const int maxCharsPerLine = 53;
        int col = 0;

        int i = 0;
        int len = aiResponse.length();

        while (i < len) {
          if (aiResponse[i] == '\n') {
            if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
              tft.println();
            }
            currentLine++;
            col = 0;
            if (currentLine > aiScrollLine) linesPrinted++;
            i++;
            continue;
          }

          int wordStart = i;
          while (i < len && aiResponse[i] != ' ' && aiResponse[i] != '\n') {
            i++;
          }
          String word = aiResponse.substring(wordStart, i);

          if (col + word.length() > maxCharsPerLine && col > 0) {
            if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
              tft.println();
            }
            currentLine++;
            col = 0;
            if (currentLine > aiScrollLine) linesPrinted++;
          }

          for (int w = 0; w < word.length(); w++) {
            char c = word[w];
            if (col >= maxCharsPerLine) {
              if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
                tft.println();
              }
              currentLine++;
              col = 0;
              if (currentLine > aiScrollLine) linesPrinted++;
            }
            if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
              tft.print(c);
            }
            col++;
          }

          if (i < len && aiResponse[i] == ' ') {
            if (col >= maxCharsPerLine) {
              if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
                tft.println();
              }
              currentLine++;
              col = 0;
              if (currentLine > aiScrollLine) linesPrinted++;
            }
            if (currentLine >= aiScrollLine && linesPrinted < maxVisibleLines) {
              tft.print(' ');
            }
            col++;
            i++;
          }
        }

        int maxScroll = currentLine - maxVisibleLines + 1;
        if (maxScroll < 0) maxScroll = 0;
        if (aiScrollLine > maxScroll) {
          aiScrollLine = maxScroll;
          lastScrollLine = maxScroll;
        }
      }
    }else {
      if (pressed_btn == "up" || pressed_btn == "8") {
        keyRow--;
        if (keyRow < 0) keyRow = 3;
      }
      else if (pressed_btn == "down" || pressed_btn == "5") {
        keyRow++;
        if (keyRow > 3) keyRow = 0;
      }
      else if (pressed_btn == "left" || pressed_btn == "4") {
        keyCol--;
        if (keyCol < 0) keyCol = 9;
      }
      else if (pressed_btn == "right" || pressed_btn == "6") {
        keyCol++;
        if (keyCol > 9) keyCol = 0;
      }
      else if (pressed_btn == "DEL") {
        if (aiPrompt.length() > 0) {
          aiPrompt.remove(aiPrompt.length() - 1);
        }
        tft.fillRect(0, 110, 320, 30, ILI9341_BLACK);
      }
      else if (pressed_btn == ",") {
        char selectedChar = ' ';
        if (keyboardMode == 2) {
          selectedChar = keysAlpha[keyRow][keyCol];
        } else if (keyboardMode == 1) {
          selectedChar = keysUppercase[keyRow][keyCol];
        } else {
          selectedChar = keysLowercase[keyRow][keyCol];
        }
        aiPrompt += selectedChar;
      }
      else if (pressed_btn == "=") {
        tft.fillRect(0, 110, 320, 120, ILI9341_BLACK);
        tft.setCursor(0, 115);
        tft.setFont();
        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);

        if (WiFi.status() != WL_CONNECTED) {
          tft.println("wifi is off");
          delay(2000);
          tft.fillRect(0, 110, 320, 120, ILI9341_BLACK);
        } else {
          tft.println("Thinking...");

          HTTPClient http;
          String url = "https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:generateContent?key=" + apiKey;

          http.begin(url);
          http.setTimeout(15000);
          http.addHeader("Content-Type", "application/json");

          String jsonPayload = "{\"contents\":[{\"parts\":[{\"text\":\"" + aiPrompttConst + aiPrompt + "\"}]}]}";
          int httpResponseCode = http.POST(jsonPayload);

          if (httpResponseCode > 0) {
            String response = http.getString();
            int textIndex = response.indexOf("\"text\": \"");
            if (textIndex != -1) {
              int start = textIndex + 9;
              int end = response.indexOf("\"", start);
              aiResponse = response.substring(start, end);
              aiResponse.replace("\\n", "\n");
            } else {
              aiResponse = "Error parsing response.";
            }
          } else {
            aiResponse = "HTTP Error: " + String(httpResponseCode);
          }
          http.end();
          
          tft.fillScreen(ILI9341_BLACK);
          aiViewingResponse = true;
          return;
        }
      }

      tft.setFont();
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(0, 112);
      tft.print("Prompt: ");
      tft.print(aiPrompt);

      if ((millis() / 300) % 2 == 0) {
        tft.print("_");
      } else {
        tft.print(" ");
      }

      int startX = 10;
      int startY = 135;
      int cellWidth = 30;
      int cellHeight = 20;

      for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 10; c++) {
          int cellX = startX + (c * cellWidth);
          int cellY = startY + (r * cellHeight);

          char displayChar = ' ';
          if (keyboardMode == 2) {
            displayChar = keysAlpha[r][c];
          } else if (keyboardMode == 1) {
            displayChar = keysUppercase[r][c];
          } else {
            displayChar = keysLowercase[r][c];
          }

          if (r == keyRow && c == keyCol) {
            tft.fillRect(cellX - 2, cellY - 4, cellWidth - 4, cellHeight - 4, ILI9341_WHITE);
            tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
          } else {
            tft.fillRect(cellX - 2, cellY - 4, cellWidth - 4, cellHeight - 4, ILI9341_BLACK);
            tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
          }

          tft.setCursor(cellX + 6, cellY);
          if (displayChar == ' ') {
            tft.print(" ");
          } else {
            tft.print(displayChar);
          }
        }
      }

      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(10, 220);
      if (keyboardMode == 2) tft.print("[MODE: ALPHA]");
      else if (keyboardMode == 1) tft.print("[MODE: CAPS]");
      else tft.print("[MODE: lower]");
    }
  }
  else if (specialMenuState == 7) {
    //camera-> maybe in future
    if (gameState == 0) {
      gameState = 1;
      tft.fillScreen(ILI9341_BLACK);

      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(20, 130);
      tft.print("CAMERA");

      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(20, 160);
      tft.print("This feature is not supported yet.");

      tft.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
      tft.setCursor(20, 180);
      tft.print("Press AC to exit.");
    }
    else if (pressed_btn == "AC") {
      gameState = 0;
      specialMenuState = 0;
      tft.fillScreen(ILI9341_BLACK);
      return;
    }
  }
  else if (specialMenuState == 8) {
    if (gameState == 0) {
      gameState = 1;
      tft.fillScreen(ILI9341_BLACK);
      
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.setCursor(20, 40);
      tft.print("STARTING WIFI...");

      WiFi.disconnect(true, true);
      WiFi.mode(WIFI_OFF);
      delay(100); 
      
      WiFi.mode(WIFI_AP);
      delay(50);

      WiFi.softAP("Calculator");
      delay(100);
      
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(20, 40);
      tft.print("WEB DATA EDITOR");
      
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(20, 80);
      tft.print("Connect to Wi-Fi: Calculator");
      
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.setCursor(20, 130);
      tft.print("192.168.4.1/edit"); 
      
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
      tft.setCursor(20, 180);
      tft.print("Press AC to exit editor.");

      server.begin();
      isServerRunning = true;
    }
    else if (pressed_btn == "AC") {
      isServerRunning = false;
      server.stop();
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_OFF);
      
      gameState = 0;
      specialMenuState = 0;
      tft.fillScreen(ILI9341_BLACK);
      return;
    }
    else if (isServerRunning) {
      server.handleClient();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Calculator Editor</title>
    <style>
        :root {
            --bg-color: #0f0f11;
            --card-bg: #16161a;
            --accent: #ffcc00;
            --text: #eeeeee;
            --text-muted: #a0a0aa;
            --border: #27272a;
            --danger: #ef4444;
            --success: #10b981;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            background-color: var(--bg-color);
            color: var(--text);
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: flex-start;
            min-height: 100vh;
            box-sizing: border-box;
        }
        .container {
            width: 100%;
            max-width: 650px;
            background: var(--card-bg);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.5);
        }
        
        /* Tabovi */
        .tabs {
            display: flex;
            gap: 10px;
            margin-bottom: 25px;
            border-bottom: 1px solid var(--border);
            padding-bottom: 10px;
        }
        .tab-btn {
            background: transparent;
            color: var(--text-muted);
            padding: 10px 20px;
            font-size: 15px;
            font-weight: 600;
            border-radius: 6px;
            border: 1px solid transparent;
            transition: all 0.2s;
        }
        .tab-btn:hover {
            color: var(--text);
            background: rgba(255,255,255,0.02);
        }
        .tab-btn.active {
            color: var(--accent);
            background: rgba(255, 204, 0, 0.1);
            border-color: rgba(255, 204, 0, 0.2);
        }
        .tab-content {
            display: none;
        }
        .tab-content.active {
            display: block;
        }

        h1 {
            font-size: 24px;
            color: var(--accent);
            margin-top: 0;
            margin-bottom: 25px;
            text-align: center;
            font-weight: 600;
            letter-spacing: 0.5px;
        }
        .section-title {
            font-size: 14px;
            text-transform: uppercase;
            letter-spacing: 1px;
            color: var(--text-muted);
            margin: 25px 0 10px 0;
            border-bottom: 1px solid var(--border);
            padding-bottom: 5px;
        }
        .item-list {
            display: flex;
            flex-direction: column;
            gap: 15px;
            margin-bottom: 15px;
        }
        .item-row {
            display: flex;
            gap: 15px;
            align-items: center;
            background: rgba(255,255,255,0.02);
            padding: 12px;
            border-radius: 8px;
            border: 1px solid var(--border);
        }
        .inputs-grp {
            display: flex;
            flex-direction: column;
            flex: 1;
            gap: 8px;
        }
        .inp-title, .wifi-input {
            width: 50%;
            background: #202024;
            border: 1px solid var(--border);
            color: var(--accent);
            padding: 8px 12px;
            border-radius: 6px;
            font-size: 15px;
            font-weight: 600;
            outline: none;
            box-sizing: border-box;
        }
        .inp-body {
            width: 90%;
            align-self: flex-end;
            background: #202024;
            border: 1px solid var(--border);
            color: var(--text);
            padding: 6px 10px;
            border-radius: 6px;
            font-size: 13px;
            outline: none;
            box-sizing: border-box;
        }
        .wifi-field {
            display: flex;
            flex-direction: column;
            gap: 8px;
            margin-bottom: 20px;
        }
        .wifi-field label {
            font-size: 14px;
            color: var(--text-muted);
        }
        .wifi-input {
            width: 100%;
            color: var(--text);
            font-weight: normal;
        }
        input:focus {
            border-color: var(--accent);
        }
        button {
            cursor: pointer;
            border: none;
            border-radius: 6px;
            font-weight: 500;
            font-size: 14px;
        }
        .btn-add {
            background: transparent;
            color: var(--accent);
            border: 1px dashed var(--accent);
            width: 100%;
            padding: 12px;
            margin-top: 5px;
        }
        .btn-delete {
            background: #27272a;
            color: var(--danger);
            padding: 10px 14px;
            height: fit-content;
        }
        .btn-upload {
            background: var(--accent);
            color: #000;
            width: 100%;
            padding: 14px;
            font-size: 16px;
            font-weight: 600;
            margin-top: 40px;
            border-radius: 8px;
        }
        .btn-wifi-update {
            background: var(--success);
            color: #fff;
            width: 100%;
            padding: 14px;
            font-size: 16px;
            font-weight: 600;
            margin-top: 20px;
            border-radius: 8px;
        }
    </style>
</head>
<body>
<div class="container">
    <div class="tabs">
        <button class="tab-btn active" onclick="switchTab('editor-tab', this)">Calculator Editor</button>
        <button class="tab-btn" onclick="switchTab('wifi-tab', this)">Wi-Fi Settings</button>
    </div>

    <div id="editor-tab" class="tab-content active">
        <h1>Calculator Data Editor</h1>
        <div class="section-title">Formulas</div>
        <div id="formulas-list" class="item-list"></div>
        <button id="add-f-btn" class="btn-add" onclick="addItem('formulas-list')">+ Add Formula</button>
        <div class="section-title">Notes</div>
        <div id="notes-list" class="item-list"></div>
        <button id="add-n-btn" class="btn-add" onclick="addItem('notes-list')">+ Add Note</button>
        <button class="btn-upload" onclick="uploadData()">UPLOAD TO CALCULATOR</button>
    </div>

    <div id="wifi-tab" class="tab-content">
        <h1>Wi-Fi Network Settings</h1>
        <div class="wifi-field">
            <label>Network Name (SSID):</label>
            <input type="text" id="wifi-ssid" class="wifi-input" placeholder="Enter SSID...">
        </div>
        <div class="wifi-field">
            <label>Password:</label>
            <input type="password" id="wifi-pass" class="wifi-input" placeholder="Enter Wi-Fi Password...">
        </div>
        <button class="btn-wifi-update" onclick="updateWifi()">UPDATE WI-FI CREDENTIALS</button>
    </div>
</div>

<script>
    function switchTab(tabId, btn) {
        document.querySelectorAll('.tab-content').forEach(tab => tab.classList.remove('active'));
        document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
        
        document.getElementById(tabId).classList.add('active');
        btn.classList.add('active');
    }

    window.onload = function() {
        fetch('/get-data')
            .then(response => response.json())
            .then(data => {
                if (data.formulas) {
                    data.formulas.forEach(f => {
                        let parts = f.split('|');
                        addItem('formulas-list', parts[0], parts[1] || '');
                    });
                }
                if (data.notes) {
                    data.notes.forEach(n => {
                        let parts = n.split('|');
                        addItem('notes-list', parts[0], parts[1] || '');
                    });
                }
                if (data.ssid) document.getElementById('wifi-ssid').value = data.ssid;
                if (data.password) document.getElementById('wifi-pass').value = data.password;
            })
            .catch(err => console.log(err));
    };

    function addItem(containerId, title = '', body = '') {
        const container = document.getElementById(containerId);
        if (container.children.length >= 30) return;
        
        const row = document.createElement('div');
        row.className = 'item-row';
        
        const grp = document.createElement('div');
        grp.className = 'inputs-grp';
        
        const tInput = document.createElement('input');
        tInput.type = 'text';
        tInput.className = 'inp-title';
        tInput.value = title;
        tInput.placeholder = 'Title';
        
        const bInput = document.createElement('input');
        bInput.type = 'text';
        bInput.className = 'inp-body';
        bInput.value = body;
        bInput.placeholder = containerId === 'formulas-list' ? 'Formula' : 'Note';

        bInput.oninput = function() {
            let val = this.value;
            val = val.replace(/root/gi, '√');
            val = val.replace(/pi/gi, 'π');
            val = val.replace(/p2/gi, '²');
            val = val.replace(/p3/gi, '³');
            this.value = val;
        };
        
        const delBtn = document.createElement('button');
        delBtn.className = 'btn-delete';
        delBtn.innerHTML = '✕';
        delBtn.onclick = function() { 
            row.remove();
            checkLimits();
        };
        
        grp.appendChild(tInput);
        grp.appendChild(grp.appendChild(bInput));
        row.appendChild(grp);
        row.appendChild(delBtn);
        container.appendChild(row);
        checkLimits();
    }

    function checkLimits() {
        document.getElementById('add-f-btn').style.display = document.getElementById('formulas-list').children.length >= 30 ? 'none' : 'block';
        document.getElementById('add-n-btn').style.display = document.getElementById('notes-list').children.length >= 30 ? 'none' : 'block';
    }

    function uploadData() {
        const formulas = Array.from(document.querySelectorAll('#formulas-list .item-row')).map(row => {
            let t = row.querySelector('.inp-title').value;
            let b = row.querySelector('.inp-body').value;
            return t + '|' + b;
        });
        const notes = Array.from(document.querySelectorAll('#notes-list .item-row')).map(row => {
            let t = row.querySelector('.inp-title').value;
            let b = row.querySelector('.inp-body').value;
            return t + '|' + b;
        });
        const payload = { formulas, notes };
        fetch('/save-data', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        })
        .then(res => {
            if(res.ok) alert('Data uploaded successfully!');
            else alert('Upload failed.');
        })
        .catch(err => alert('Communication error.'));
    }

    function updateWifi() {
        const ssidVal = document.getElementById('wifi-ssid').value;
        const passVal = document.getElementById('wifi-pass').value;
        
        const payload = { ssid: ssidVal, password: passVal };
        
        fetch('/save-wifi', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        })
        .then(res => {
            if(res.ok) alert('Wi-Fi credentials updated successfully!');
            else alert('Failed to update Wi-Fi credentials.');
        })
        .catch(err => alert('Communication error.'));
    }
</script>
</body>
</html>
)rawliteral";

void setupServerRoutes() {
  server.on("/edit", HTTP_GET, []() {
    server.send_P(200, "text/html", HTML_INDEX);
  });
  server.on("/get-data", HTTP_GET, handleGetData);
  server.on("/save-data", HTTP_POST, handleSaveData);
  server.on("/save-wifi", HTTP_POST, handleSaveWifi); 
}

bool isBoundary(char c) {
  return (c == ' ' || c == '+' || c == '-' || c == '*' || c == '/' || 
          c == '(' || c == ')' || c == '=' || c == ',' || c == '\n' || c == '\r');
}

bool checkWord(const String &input, int index, const String &word) {
  int len = input.length();
  int wordLen = word.length();
  
  if (index + wordLen > len) return false;
  if (input.substring(index, index + wordLen) != word) return false;
  
  bool startOk = (index == 0) || isBoundary(input.charAt(index - 1));
  bool endOk = (index + wordLen == len) || isBoundary(input.charAt(index + wordLen));
  
  return startOk && endOk;
}

String formatToDevice(String input) {
  input.replace("³", " P3 ");
  input.replace("²", " P2 ");
  input.replace("√", " ROOT ");
  input.replace("π", " PI ");
  input.replace("^-1", " -01 ");
  input.replace("10^", " 10X ");
  input.replace("*", " MULT ");
  
  String formatted = "";
  int len = input.length();
  for (int i = 0; i < len; i++) {
    if (input.charAt(i) == '-') {
      bool isNeg = false;
      if (i == 0) {
        isNeg = true;
      } else {
        int j = i - 1;
        while (j >= 0 && input.charAt(j) == ' ') {
          j--;
        }
        if (j < 0) {
          isNeg = true;
        } else {
          char prev = input.charAt(j);
          
          // Provjera da li je ispred kombinacija +/
          bool isPlusSlash = false;
          if (j > 0) {
            int k = j - 1;
            while (k >= 0 && input.charAt(k) == ' ') {
              k--;
            }
            if (k >= 0 && input.charAt(k) == '+' && prev == '/') {
              isPlusSlash = true;
            }
          }

          if (isPlusSlash) {
            isNeg = false; // Ako je ispred +/, ostaje obicni -
          } else if (prev == '+' || prev == '-' || prev == '*' || prev == '/' || prev == '(' || prev == '=') {
            isNeg = true;
          }
        }
      }
      if (isNeg) {
        formatted += " NEG ";
      } else {
        formatted += " - ";
      }
    } else {
      formatted += input.charAt(i);
    }
  }
  
  String finalString = "";
  int flen = formatted.length();
  for (int i = 0; i < flen; i++) {
    if (formatted.charAt(i) == 'e') {
      bool leftOk = (i == 0 || formatted.charAt(i - 1) == ' ' || formatted.charAt(i - 1) == '(' || formatted.charAt(i - 1) == '=');
      bool rightOk = (i == flen - 1 || formatted.charAt(i + 1) == ' ' || formatted.charAt(i + 1) == ')' || formatted.charAt(i + 1) == '+' || formatted.charAt(i + 1) == '-' || formatted.charAt(i + 1) == '*' || formatted.charAt(i + 1) == '/');
      if (leftOk && rightOk) {
        finalString += "EULR";
      } else {
        finalString += 'e';
      }
    } else {
      finalString += formatted.charAt(i);
    }
  }
  
  String cleanString = "";
  bool lastWasSpace = false;
  for (int i = 0; i < finalString.length(); i++) {
    char c = finalString.charAt(i);
    if (c == ' ') {
      if (!lastWasSpace) {
        cleanString += ' ';
        lastWasSpace = true;
      }
    } else {
      cleanString += c;
      lastWasSpace = false;
    }
  }
  
  if (cleanString.startsWith(" ")) cleanString.remove(0, 1);
  if (cleanString.endsWith(" ")) cleanString.remove(cleanString.length() - 1, 1);
  
  return cleanString;
}

String formatToWeb(String input) {
  String formatted = "";
  int len = input.length();
  
  for (int i = 0; i < len; ) {
    if (checkWord(input, i, "MULT")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "*";
      i += 4;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "10X")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "10^";
      i += 3;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "EULR")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "e";
      i += 4;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "NEG")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "-";
      i += 3;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "PI")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "π";
      i += 2;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "ROOT")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "√";
      i += 4;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "-01")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "^-1";
      i += 3;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "P3")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "³";
      i += 2;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else if (checkWord(input, i, "P2")) {
      if (formatted.length() > 0 && formatted.charAt(formatted.length() - 1) == ' ') formatted.remove(formatted.length() - 1);
      formatted += "²";
      i += 2;
      if (i < len && input.charAt(i) == ' ') i++;
    }
    else {
      formatted += input.charAt(i);
      i++;
    }
  }
  return formatted;
}

void handleGetData() {
  DynamicJsonDocument doc(8192);
  JsonArray jsonFormulas = doc.createNestedArray("formulas");
  JsonArray jsonNotes = doc.createNestedArray("notes");
  
  for (int i = 0; i < 30; i++) {
    if (formulaTitles[i] != "" || formulaTexts[i] != "") {
      jsonFormulas.add(formulaTitles[i] + "|" + formatToWeb(formulaTexts[i]));
    }
    if (noteTitles[i] != "" || noteTexts[i] != "") {
      jsonNotes.add(noteTitles[i] + "|" + formatToWeb(noteTexts[i]));
    }
  }
  
  doc["ssid"] = ssid;
  doc["password"] = password;
  
  String jsonString;
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleSaveData() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  String body = server.arg("plain");
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "text/plain", "Json Error");
    return;
  }
  
  for (int i = 0; i < 30; i++) {
    formulaTitles[i] = "";
    formulaTexts[i] = "";
    noteTitles[i] = "";
    noteTexts[i] = "";
  }
  
  if (doc.containsKey("formulas") && doc["formulas"].is<JsonArray>()) {
    JsonArray jsonFormulas = doc["formulas"];
    int count = 0;
    for (int i = 0; i < jsonFormulas.size() && count < 30; i++) {
      if (jsonFormulas[i].is<String>()) {
        String raw = jsonFormulas[i].as<String>();
        int sep = raw.indexOf('|');
        if (sep != -1) {
          String t = raw.substring(0, sep);
          String b = raw.substring(sep + 1);
          if (t != "" || b != "") {
            formulaTitles[count] = t;
            formulaTexts[count] = formatToDevice(b);
            count++;
          }
        }
      }
    }
  }
  
  if (doc.containsKey("notes") && doc["notes"].is<JsonArray>()) {
    JsonArray jsonNotes = doc["notes"];
    int count = 0;
    for (int i = 0; i < jsonNotes.size() && count < 30; i++) {
      if (jsonNotes[i].is<String>()) {
        String raw = jsonNotes[i].as<String>();
        int sep = raw.indexOf('|');
        if (sep != -1) {
          String t = raw.substring(0, sep);
          String b = raw.substring(sep + 1);
          if (t != "" || b != "") {
            noteTitles[count] = t;
            noteTexts[count] = formatToDevice(b);
            count++;
          }
        }
      }
    }
  }

  saveArrays();
  server.send(200, "text/plain", "OK");
}

void handleSaveWifi() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  
  String body = server.arg("plain");
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "text/plain", "Json Error");
    return;
  }
  
  if (doc.containsKey("ssid") && doc["ssid"].is<String>()) {
    ssid = doc["ssid"].as<String>();
  }
  
  if (doc.containsKey("password") && doc["password"].is<String>()) {
    password = doc["password"].as<String>();
  }
  
  server.send(200, "text/plain", "OK");
  saveToEEPROM();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int getBatteryPercentage() {
  long adcSum = 0;

  for (int i = 0; i < numSamples; i++) {
    adcSum += analogRead(adcPin);
    delay(5);
  }
  float avgAdc = (float)adcSum / numSamples;

  float pinVoltage = (avgAdc / 4095.0) * 3.3;
  float batteryVoltage = pinVoltage * 2.0 + 0.2;

  float minVoltage = 3.4;
  float maxVoltage = 4.2;

  int percentage = (int)(((batteryVoltage - minVoltage) / (maxVoltage - minVoltage)) * 100);

  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;

  return percentage;
}