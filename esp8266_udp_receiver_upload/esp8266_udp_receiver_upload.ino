/********************************************/
/*    ProjectName: AnswerCollector          */
/*    ProjectMaker: Luo***, Jiang*******    */
/*    BuildTime: 2022/1/25-2022/1/28        */
/********************************************/
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

typedef unsigned int Uint;

const char* wifi_ssid = "*********"; // set the name of your WiFi
const char* wifi_password = "**********"; // set the password of your WiFi

WiFiUDP Udp; // dim a WiFiUDP object
Uint localUdpPort = 2331; // user-defined local port monitor
Uint remoteUdpPort = 3221; // user-defined remote port monitor
char incomingPacket[255]; // save the information of the Packet;
char testReply[] = "Got it!"; // Reply message for testing;
int sum;
int ans_rec[110], res[10];

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
void SetStillIP()
{
  IPAddress local_IP(192, 168, 0, 200);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(local_IP, gateway, subnet);
}
void PrintByOLED()
{
  static char t[6];
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  for(int i = 1; i <= 4; i ++)
  {
    t[0] = 'A'+i-1;
    t[1] = ':';
    t[2] = ' ';
    t[3] = res[i]/10+'0';
    t[4] = res[i]%10+'0';
    t[5] = 0;
    u8g2.drawStr(0, 16*i, t);
    if(sum)
    {
      t[0] = res[i]*1000/sum/1000+'0';
      t[1] = (res[i]*1000/sum/100)%10+'0';
      t[2] = (res[i]*1000/sum/10)%10+'0';
      t[3] = '.';
      t[4] = (res[i]*1000/sum)%10+'0';
      t[5] = '%';
      t[6] = 0;
      int t_add = 0;
      if(t[0] == '0') t_add = 1;
      u8g2.drawStr(50, 16*i, t+t_add);
    }
  }
  u8g2.drawStr(100, 48, "All");
  t[0] = sum/10+'0';
  t[1] = sum%10+'0';
  t[2] = 0;
  u8g2.drawStr(100, 64, t);
  u8g2.sendBuffer();
}
void SolveData(char str[])
{
  int k = 2, id = 0, ans;
  for(int i = 0; i < k; i ++)
  {
    id = (id<<3)+(id<<1) + (str[i]-'0');
  }
  ans = str[3]-'A'+1;
  if(ans_rec[id] == 0)
  {
    ans_rec[id] = ans;
    res[ans] ++;
    sum ++;
  }
  else if(ans_rec[id] != ans)
  {
    res[ans_rec[id]] --;
    ans_rec[id] = ans;
    res[ans] ++;
  }
}
void rec_init()
{
  sum = 0;
  memset(res, 0, sizeof res);
  memset(ans_rec, 0, sizeof ans_rec);
}

void setup()
{
  Serial.begin(9600); // Open the Serial and Set the Baud
  u8g2.begin(); // initial
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.clearBuffer();
  u8g2.drawStr(30, 40, "Loading...");
  u8g2.sendBuffer();
  WiFi.mode(WIFI_STA); // set the WiFimode STA
  WiFi.disconnect();
//  delay(200);
//  ShowAlltheWiFi();
  SetStillIP();
  Serial.printf("Trying to get connected to %s\n", wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password); // connect WiFi
  while(WiFi.status() != WL_CONNECTED) // wait to get connected
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nGet connected!");

  if(Udp.begin(localUdpPort)) // Monitor the Port
  {
    Serial.println("Monitoring:");
    Serial.printf("IP: %s, Port: %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  }
  else
  {
    Serial.println("Fail to monitor");
  }
  Serial.println(WiFi.localIP());
  PrintByOLED();
  rec_init();
}
void loop()
{
  int packetSize = Udp.parsePacket(); // Get Packet
  if(packetSize) // Get it Successfully
  {
    Serial.printf("Get the Packet from IP: %s, Port: %d, Bytes: %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort(), packetSize);
    int len = Udp.read(incomingPacket, 255);
    if(len > 0)
    {
      incomingPacket[len] = 0; // add 'null'
    }
    Serial.printf("The content is: %s\n", incomingPacket); // Show the content
//    Udp.beginPacket(Udp.remoteIP(), remoteUdpPort); // Set IP and Port
//    Udp.write(testReply); // Reply
//    Udp.endPacket(); // Send
    SolveData(incomingPacket);
    PrintByOLED();
  }
}
