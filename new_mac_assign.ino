#include <ESP8266WiFi.h>

// Set your new MAC Address
uint8_t newMACAddress[] = {0x8C, 0xAA, 0xB5, 0x78, 0x40, 0xB1};

void setup(){
  Serial.begin(115200);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  
  Serial.print("[OLD] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // For Soft Access Point (AP) Mode
  //wifi_set_macaddr(SOFTAP_IF, &newMACAddress[0]);
  // For Station Mode
  wifi_set_macaddr(STATION_IF, &newMACAddress[0]);
  
  Serial.print("[NEW] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
