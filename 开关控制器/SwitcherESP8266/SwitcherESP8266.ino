#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "bingyu-AP";
const char* pass = "bingyuxq";


unsigned int localPort = 8888;      // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
#define maxPINnum 6

// An WifiUDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
uint8_t AvalablePIN[maxPINnum] = {2, 4, 5, 13, 14, 16};

void setup() {
  Serial.begin(115200);
  // start the Wifi and UDP:
  WiFi.begin(ssid, pass);

  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if (i == 21) {
    Serial.print("Could not connect to"); Serial.println(ssid);
    while (1) delay(500);
  }
  Udp.begin(localPort);
  Serial.print("Ready!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  for (int i = 0;  i < maxPINnum; i++) {
    pinMode(AvalablePIN[i], OUTPUT);
    digitalWrite(AvalablePIN[i], LOW);
  }
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    memset(packetBuffer, 0 , sizeof(packetBuffer));
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    if (strstr(packetBuffer, "ON")) {
      Serial.println("on command:");
      SwitchON(packetBuffer);
    }
    if (strstr(packetBuffer, "OFF")) {
      Serial.println("off command:");
      SwitchOFF(packetBuffer);
    }
    if (strstr(packetBuffer, "ALLON")) {
      Serial.println("all on command:");
      AllON();
    }
    if (strstr(packetBuffer, "ALLOFF")) {
      Serial.println("all off command:");
      AllOFF();
    }
  }
  delay(10);
}

void SwitchON(char* string)
{
  char pin[2] = "";
  pin[0] = string[3]; pin[1] = string[4];
  int targetPIN = atoi(pin);
  for (int i = 0; i < maxPINnum; i++) {
    if (targetPIN == AvalablePIN[i]) {
      digitalWrite(targetPIN, HIGH);
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(targetPIN); Udp.write(" ON");
      Udp.endPacket();
    }
  }
  delay(100);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(targetPIN); Udp.write("is not a usable PIN");
  Udp.endPacket();
}

void SwitchOFF(char* string)
{
  char pin[2] = "";
  pin[0] = string[4]; pin[1] = string[5];
  int targetPIN = atoi(pin);
  for (int i = 0; i < maxPINnum; i++) {
    if (targetPIN == AvalablePIN[i]) {
      digitalWrite(targetPIN, LOW);
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(targetPIN); Udp.write(" OFF");
      Udp.endPacket();
    }
  }
  delay(100);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(targetPIN); Udp.write("is not a usable PIN");
  Udp.endPacket();
}

void AllON()
{
  for (int i = 0; i < maxPINnum; i++) {
    digitalWrite(AvalablePIN[i], HIGH);
    delay(100);
  }
  delay(100);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("ALL ON");
  Udp.endPacket();
}

void AllOFF()
{

  for (int i = 0; i < maxPINnum; i++) {
    digitalWrite(AvalablePIN[i], LOW);
    delay(100);
  }
  delay(100);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("ALL OFF");
  Udp.endPacket();
}

char* itostr(char *str, int i)
{
  sprintf(str, "%d", i);
  return str;
}
//char[]来源，const char分隔符，char*目标缓存数组的指针(buff为char* []类型，int元素数缓存
void split(char *src, const char *separator, char **dest, int *num)
{
  char *pSeparator, *pStart, *pEnd;
  unsigned int sep_len;
  int count = 0;

  if (src == NULL || strlen(src) == 0) return;

  pSeparator = (char *)malloc(16);
  if (pSeparator == NULL) return;

  if (separator == NULL || strlen(separator) == 0) strcpy(pSeparator, " "); /* one blank by default */
  else strcpy(pSeparator, separator);

  sep_len = strlen(pSeparator);

  pStart = src;

  while (1)
  {
    pEnd = strstr(pStart, pSeparator);
    if (pEnd != NULL)
    {
      memset(pEnd, '\0', sep_len);
      *dest++ = pStart;
      pEnd = pEnd + sep_len;
      pStart = pEnd;
      ++count;
    }
    else
    {
      *dest = pStart;
      ++count;
      break;
    }
  }

  *num = count;

  if (pSeparator != NULL) free(pSeparator);
}
