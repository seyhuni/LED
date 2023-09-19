#include "DMDESP.h"
#include "fixednums15x31.h"
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>

#define PRICELINE 3
uint8_t newMACAddress[] = {0x8C, 0xAA, 0xB5, 0x78, 0x40, 0xB1};

#define DISPLAYS_WIDE 3 // columns of Panel
#define DISPLAYS_HIGH 2 // rows of panel
#define FontDefault FIXEDNUMS15x31
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Number of P10 Panels used (COLUMS, ROWS)

char priceChars[] = "12,34";
char priceCharsA[] = "12,34";
int eaddress = 0;
bool pricechanged = 0;
unsigned long pricechangedMillis;
unsigned long timediff;

typedef struct price_message
{
  char linenumber;
  char colnumber;
  char key;
} price_message; 

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  price_message* pm = (price_message*) incomingData;
  Serial.print("Key pressed: ");
  Serial.print(pm->key);
  Serial.print(" ,linenumber: ");
  Serial.print(pm->linenumber+0x30);
  Serial.print(" ,colnumber: ");
  Serial.println(pm->colnumber+0x30);
  if(pm->key == '#')
  {
    pricechanged = 0;
    pm->colnumber = 4;
  }
  else
  {
    if(PRICELINE == pm->linenumber)
    {
    memcpy(priceCharsA, priceChars, 5);
    if(pm->colnumber<2)
    {
      priceChars[pm->colnumber]=pm->key;
      priceCharsA[pm->colnumber]='*';
    }
    else
    {
      priceChars[pm->colnumber+1]=pm->key;
      priceCharsA[pm->colnumber+1]='*';
    }
    EEPROM.write((pm->colnumber), pm->key);
    EEPROM.commit();
    pricechanged = 1;
    pricechangedMillis = millis();
    }
    else
    {
    pricechanged = 0;
    pm->colnumber = 4;
    }
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  wifi_set_macaddr(STATION_IF, &newMACAddress[0]);

  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  Disp.start(); // Run the DMDESP library
  Disp.setBrightness(5); // brightness level
  Disp.setFont(FontDefault); // Determine the letters
  EEPROM.begin(512);
  if((EEPROM.read(eaddress)>='0')&&(EEPROM.read(eaddress)<='9'))
  {
    priceChars[0] = EEPROM.read(eaddress);
    priceChars[1] = EEPROM.read(eaddress+1);
    priceChars[3] = EEPROM.read(eaddress+2);
    priceChars[4] = EEPROM.read(eaddress+3);
    priceCharsA[0] = EEPROM.read(eaddress);
    priceCharsA[1] = EEPROM.read(eaddress+1);
    priceCharsA[3] = EEPROM.read(eaddress+2);
    priceCharsA[4] = EEPROM.read(eaddress+3);
    pricechanged = 1;
    pricechangedMillis = millis();
  }
}

void loop() {
  String stringPrice;
  String stringPriceA;
  stringPrice = priceChars;
  stringPriceA = priceCharsA;
  Disp.loop();
  if(pricechanged>0)
  {
    timediff = millis()-pricechangedMillis;
    timediff = timediff/1000;
    if(timediff%2==0)
    {
      Disp.drawText(8,2,stringPrice);
    }
    else
    {
      Disp.drawText(8,2,stringPriceA);
    }
  }
  else
  {
    Disp.drawText(8,2,stringPrice);
  }
}
