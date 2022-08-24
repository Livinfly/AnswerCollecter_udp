/********************************************/
/*    ProjectName: AnswerCollector          */
/*    ProjectMaker: Luo***, Jiang*******    */
/*    BuildTime: 2022/1/25-2022/1/28        */
/********************************************/
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Keypad.h>

typedef unsigned int Uint;

const char* wifi_ssid = "*********"; // set the name of your WiFi
const char* wifi_password = "**********"; // set the password of your WiFi
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
const Uint id_delta = 0; // 0(1->4), 4, 8 ...

WiFiUDP Udp; // dim a WiFiUDP object
Uint localUdpPort = 2331; // user-defined local port monitor
Uint remoteUdpPort = 3221; // user-defined remote port monitor
char incomingPacket[255]; // save the information of the Packet;
char testReply[] = "Got it!"; // Reply message for testing;
const char hexaKeys[ROWS][COLS] = {
  {'A','B','C','D'},
  {'E','F','G','H'},
  {'I','J','K','L'},
  {'M','N','O','P'}
};
byte rowPins[ROWS] = {D7,D6,D5,D4};
byte colPins[COLS] = {D3,D2,D1,D0};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey, msg[6];


void ShowAlltheWiFi()
{
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else 
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
}

void setup()
{
  Serial.begin(9600); // Open the Serial and Set the Baud
  WiFi.mode(WIFI_STA); // set the WiFimode STA
  WiFi.disconnect();
//  delay(200);
//  ShowAlltheWiFi();
  Serial.printf("Trying to get connected to %s\n", wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password); // connect WiFi
  while(WiFi.status() != WL_CONNECTED) // wait to get connected
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nGet connected!");
}
void loop()
{
  customKey = customKeypad.getKey();
  if(customKey)
  {
    Serial.println(customKey);
    int t = customKey-'A';
    int x = t/4+1, y = t%4;
    int real_id = x+id_delta;
    msg[0] = real_id/10+'0';
    msg[1] = real_id%10+'0';
    msg[2] = '_';
    msg[3] = y+'A';
    msg[4] = 0;
    Udp.beginPacket("192.168.0.200", localUdpPort); // Set IP and Port
    Udp.write(msg); // Reply
    Udp.endPacket(); // Send
    Serial.println(msg);
  }
}
