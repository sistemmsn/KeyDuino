#include <KeyDuino.h>

//A-Keys Array
uint8_t definedKeysA[16][6] = {
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 }
};

//B-Keys Array
uint8_t definedKeysB[16][6] = {
  { 0xA2, 0x2A, 0xE1, 0x29, 0xC0, 0x13 },
  { 0x49, 0xFA, 0xE4, 0xE3, 0x84, 0x9F },
  { 0x38, 0xFC, 0xF3, 0x30, 0x72, 0xE0 },
  { 0x8A, 0xD5, 0x51, 0x7B, 0x4B, 0x18 },
  { 0x50, 0x93, 0x59, 0xF1, 0x31, 0xB1 },
  { 0x6C, 0x78, 0x92, 0x8E, 0x13, 0x17 },
  { 0xAA, 0x07, 0x20, 0x01, 0x87, 0x38 },
  { 0xA6, 0xCA, 0xC2, 0x88, 0x64, 0x12 },
  { 0x62, 0xD0, 0xC4, 0x24, 0xED, 0x8E },
  { 0xE6, 0x4A, 0x98, 0x6A, 0x5D, 0x94 },
  { 0x8F, 0xA1, 0xD6, 0x01, 0xD0, 0xA2 },
  { 0x89, 0x34, 0x73, 0x50, 0xBD, 0x36 },
  { 0x66, 0xD2, 0xB7, 0xDC, 0x39, 0xEF },
  { 0x6B, 0xC1, 0xE1, 0xAE, 0x54, 0x7D },
  { 0x22, 0x72, 0x9A, 0x9B, 0xD4, 0x0F },
  { 0x48, 0x45, 0x58, 0x41, 0x43, 0x54 }
};

uint8_t zeroData[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t trailerBlockData[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

MifareClassicKeyDuino keyDuino;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; 
uint8_t uidLength;

void setup(void) {
  Serial.begin(115200);
  
  keyDuino.begin();

  keyDuino.SAMConfig();
  
  while(!Serial){}
  Serial.println("/!\\ Warning! /!\\");
  Serial.println("Be sure to know what you are doing when writing on a card!");
  Serial.println("Remember a wrong usage can lead you to make the card unusable ...");
  Serial.println("/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\/!\\");
}

void loop(void) {
  uint8_t success;
  success = keyDuino.readTargetID(uid, &uidLength);

  if (success) {
    if (uidLength == 4) {
      Serial.println("Mifare classic identified");
      for (int i = 0 ; i < 16 ; i++)
      {
	//Try authentication with defined key A, then B, then default keys. Writing won't work if key A has read-only access ... change checking order in that case (B then A).
        if (keyDuino.authenticateDefinedKey(definedKeysA[i], MIFARE_KEY_A, i) ||
            keyDuino.authenticateDefinedKey(definedKeysB[i], MIFARE_KEY_B, i) ||
            keyDuino.mifareclassic_AuthenticateSectorDefaultKeys(i))
        {
          for (int j = 0 ; j < 4 ; j++) {
            int block = i * 4 + j;
            if (block) //Be sure not to write on Block 0
              if (keyDuino.mifareclassic_IsTrailerBlock(block))
                keyDuino.writeBlock(block, trailerBlockData);
              else
                keyDuino.writeBlock(block, zeroData);
          }
        }
      }
      delay(500);
      Serial.println("Operation complete.");
      Serial.println("Press a key for next operation.");
      while (!Serial.available());
      while (Serial.available()) Serial.read();
    } else {
      Serial.println("Detected tag is not Mifare Classic.");
      delay(500);
    }
  }
}

