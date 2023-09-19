#include "DMDESP.h"
#include "droidsans24.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

#define PRICELINE 0

#define DISPLAYS_WIDE 3 // columns of Panel
#define DISPLAYS_HIGH 2 // rows of panel
#define FontDefault Droid_Sans_24
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Number of P10 Panels used (COLUMS, ROWS)

char priceChars[] = "12,34";


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
  if(PRICELINE == pm->linenumber)
  {
    if(pm->colnumber<2)
      priceChars[pm->colnumber]=pm->key;
    else
      priceChars[pm->colnumber+1]=pm->key;
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
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
}

void loop() {
  Disp.loop();
  String stringPrice;
  stringPrice = priceChars;
  Disp.drawText(0,0,stringPrice);
}
