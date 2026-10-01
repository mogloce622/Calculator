void saveToEEPROM() {
  int addr = 0;
  
  EEPROM.write(addr, 42); 
  addr += 1;
  
  EEPROM.write(addr, calcMode);
  addr += sizeof(int);
  
  EEPROM.write(addr, angleUnit);
  addr += sizeof(int);
  
  EEPROM.write(addr, displayMode);
  addr += sizeof(int);
  
  EEPROM.write(addr, displayParameterFix);
  addr += sizeof(int);
  
  EEPROM.write(addr, displayParameterSci);
  addr += sizeof(int);
  
  EEPROM.write(addr, displayParameterNorm);
  addr += sizeof(int);
  
  EEPROM.write(addr, defaultFrac ? 1 : 0);
  addr += 1;
  
  byte ssidLen = ssid.length();
  EEPROM.write(addr, ssidLen);
  addr += 1;
  for (int i = 0; i < ssidLen; i++) {
    EEPROM.write(addr + i, ssid.charAt(i));
  }
  addr += 32;
  
  byte passLen = password.length();
  EEPROM.write(addr, passLen);
  addr += 1;
  for (int i = 0; i < passLen; i++) {
    EEPROM.write(addr + i, password.charAt(i));
  }
  addr += 64;
  
  byte keyLen = apiKey.length();
  EEPROM.write(addr, keyLen);
  addr += 1;
  for (int i = 0; i < keyLen; i++) {
    EEPROM.write(addr + i, apiKey.charAt(i));
  }
  addr += 64;
  
  EEPROM.commit();
}

void readFromEEPROM() {
  int addr = 1;
  
  calcMode = EEPROM.read(addr);
  addr += sizeof(int);
  
  angleUnit = EEPROM.read(addr);
  addr += sizeof(int);
  
  displayMode = EEPROM.read(addr);
  addr += sizeof(int);
  
  displayParameterFix = EEPROM.read(addr);
  addr += sizeof(int);
  
  displayParameterSci = EEPROM.read(addr);
  addr += sizeof(int);
  
  displayParameterNorm = EEPROM.read(addr);
  addr += sizeof(int);
  
  defaultFrac = (EEPROM.read(addr) == 1);
  addr += 1;
  
  byte ssidLen = EEPROM.read(addr);
  addr += 1;
  ssid = "";
  if (ssidLen <= 32) {
    for (int i = 0; i < ssidLen; i++) {
      ssid += (char)EEPROM.read(addr + i);
    }
  }
  addr += 32;
  
  byte passLen = EEPROM.read(addr);
  addr += 1;
  password = "";
  if (passLen <= 64) {
    for (int i = 0; i < passLen; i++) {
      password += (char)EEPROM.read(addr + i);
    }
  }
  addr += 64;
  
  byte keyLen = EEPROM.read(addr);
  addr += 1;
  apiKey = "";
  if (keyLen <= 64) {
    for (int i = 0; i < keyLen; i++) {
      apiKey += (char)EEPROM.read(addr + i);
    }
  }
  addr += 64;
}

void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("Error mounting LittleFS file system!");
    return;
  }
  Serial.println("LittleFS successfully initialized.");
}

void saveArrays() {
  JsonDocument doc;

  JsonArray nTitles = doc["noteTitles"].to<JsonArray>();
  JsonArray nTexts  = doc["noteTexts"].to<JsonArray>();
  JsonArray fTitles = doc["formulaTitles"].to<JsonArray>();
  JsonArray fTexts  = doc["formulaTexts"].to<JsonArray>();

  for (int i = 0; i < 30; i++) {
    nTitles.add(noteTitles[i]);
    nTexts.add(noteTexts[i]);
    fTitles.add(formulaTitles[i]);
    fTexts.add(formulaTexts[i]);
  }

  File file = LittleFS.open("/calc_data.json", "w");
  if (!file) {
    Serial.println("Failed to open file for writing!");
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write data to file!");
  } else {
    Serial.println("Data successfully saved to LittleFS.");
  }

  file.close();
}

void readArrays() {
  File file = LittleFS.open("/calc_data.json", "r");
  if (!file) {
    Serial.println("File does not exist or failed to open.");
    return;
  }

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("JSON deserialization error: ");
    Serial.println(error.c_str());
    return;
  }

  for (int i = 0; i < 30; i++) {
    noteTitles[i]    = doc["noteTitles"][i]    | "";
    noteTexts[i]     = doc["noteTexts"][i]     | "";
    formulaTitles[i] = doc["formulaTitles"][i] | "";
    formulaTexts[i]  = doc["formulaTexts"][i]  | "";
  }

  Serial.println("Data successfully loaded from LittleFS.");
}
