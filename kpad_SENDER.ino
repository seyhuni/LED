#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Keypad.h>
uint8_t broadcastAddress2[] = {0x8C, 0xAA, 0xB5, 0x78, 0x40, 0xB1}; //8C:AA:B5:78:40:B1
const byte n_rows = 4;
const byte n_cols = 4;
char keys[n_rows][n_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

typedef struct price_message
{
  char linenumber;
  char colnumber;
  char key;
} price_message;
 
price_message pmessage;

byte colPins[n_rows] = {D3, D2, D1, D0};
byte rowPins[n_cols] = {D7, D6, D5, D4};
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols); 

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
    pmessage.colnumber++;
    pmessage.colnumber = pmessage.colnumber % 4;
  }
  else{
    Serial.println("Delivery fail");
  }
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  pmessage.linenumber = 0;
  pmessage.colnumber = 0;
}
 
void loop() {
  pmessage.key = myKeypad.getKey();
  if (pmessage.key != NO_KEY){
    if(pmessage.key == '*')
    {
      pmessage.linenumber = 0;
      pmessage.colnumber = 0;
    }
    else if(pmessage.key == '#')
    {
      pmessage.linenumber++;
      pmessage.colnumber = 3;
      pmessage.linenumber = pmessage.linenumber % 5;
      esp_now_send(broadcastAddress2, (uint8_t *) &pmessage, sizeof(pmessage));

    }
    else if ((pmessage.key >= '0') && (pmessage.key <= '9'))
    {
      esp_now_send(broadcastAddress2, (uint8_t *) &pmessage, sizeof(pmessage));
    }
  
    Serial.print("Key pressed: ");
    Serial.print(pmessage.key);
    Serial.print(" ,linenumber: ");
    Serial.print(pmessage.linenumber+0x30);
    Serial.print(" ,colnumber: ");
    Serial.println(pmessage.colnumber+0x30);    
  }
}
