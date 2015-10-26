#line 1 "UDPsenderESP8266.ino"

#define READTIMEOUT 30000
#include "IRremoteESP8266.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Arduino.h"
void setup();
void loop();
void RemoteSender(char* string);
void LearnCommand(char* string);
char* itostr(char *str, int i);
void split(char *src, const char *separator, char **dest, int *num);
#line 8
const char* ssid = "bingyu-AP";
const char* pass = "bingyuxq";
IRsend irsend(13);                                        

IRrecv irrecv(5);                                            
unsigned int localPort = 8888;                                
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];                                  
char  ReplyBuffer[] = "acknowledged";                               

                                                                  
WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
                            
  WiFi.begin(ssid, pass);
  irsend.begin();
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
}

void loop() {
                                             
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

                                         
    memset(packetBuffer, 0 , sizeof(packetBuffer));
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    if (strchr(packetBuffer, 'R')) {
      RemoteSender(packetBuffer);
    }
    if (strchr(packetBuffer, 'L')) {
      LearnCommand(packetBuffer);
    }
  }
  delay(10);
}

void RemoteSender(char* string)
{
  char *dest[500];
  int num = 0;
  split(string, ",", dest, &num);
  unsigned int RawData[num+1];
  for (int i = 1; i < num; i++) {
                                                                                                                                                                                                                                                                                                                               
    RawData[i - 1] = atoi(dest[i]);
  }
  irsend.sendRaw(RawData, num - 1, 38);        
  delay(100);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("Command Sent");
  Udp.endPacket();
}

void LearnCommand(char* string)
{
  irrecv.enableIRIn();                       
                    
                                                                                 
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("Try Me");
  Udp.endPacket();
  delay(500);
  int BeginMsec = millis() + READTIMEOUT;
  int UDPWriteSize = 0;
  do
  {
    decode_results  results;                                         

    if (irrecv.decode(&results)) {                    
      int Length = results.rawlen - 1;
      unsigned int RawData[Length];

      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      for (int i = 0;  i < results.rawlen;  i++) {
        unsigned int Data = results.rawbuf[i] * USECPERTICK;
                                                                                                      
                                                                                             
        char buff[6];
        itostr(buff, Data);
        UDPWriteSize += Udp.write(buff) + 1 ;
        if ( i < Length ) Udp.write(",");
        if (UDPWriteSize > 508) {
          UDPWriteSize = 0;
          Udp.endPacket();
          delay(100);
          Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        }
      }
      Udp.endPacket();
      delay(500);
                                                                         
      irrecv.disableIRIn();
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write("Got it");
      Udp.endPacket();
      return;
    }
    delay(10);
  } while (BeginMsec > millis());
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("Got nothing");
  Udp.endPacket();
  irrecv.disableIRIn();
}

char* itostr(char *str, int i)
{
  sprintf(str, "%d", i);
  return str;
}
                                                                
void split(char *src, const char *separator, char **dest, int *num)
{
  char *pSeparator, *pStart, *pEnd;
  unsigned int sep_len;
  int count = 0;

  if (src == NULL || strlen(src) == 0) return;

  pSeparator = (char *)malloc(16);
  if (pSeparator == NULL) return;

  if (separator == NULL || strlen(separator) == 0) strcpy(pSeparator, " ");                           
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

