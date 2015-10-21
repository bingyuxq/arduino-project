/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
const char* ssid = "bingyu-AP";
const char* password = "bingyuxq";
IRsend irsend(13); //an IR led is connected to GPIO pin 13

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if (i == 21) {
    Serial.print("Could not connect to"); Serial.println(ssid);
    while (1) delay(500);
  }
  //start UART and the server
  Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready!");
  Serial.print(WiFi.localIP());
  Serial.println(":23' to connect");
}

void loop() {
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.println(i);
      }
      else
      {
        Serial.print("No more client, max"); Serial.println(MAX_SRV_CLIENTS);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  //check clients for data
  String buff = ReadMsg();
  Serial.print(buff);
  if (buff.indexOf("RemoteSend") != -1) {
    RemoteSender((char*)buff.c_str());
  }

  //check UART for data
  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    //SendMsg(*sbuf, len);
  }
}

String ReadMsg() {
  String buff = "";
  for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        //for(int j=0;serverClients[i].available();j++){
        //buff[j] = serverClients[i].read();
        //}
        while (serverClients[i].available()) buff += (char)serverClients[i].read();
      }
    }
  }
  return buff;
}
//向客户端发送echo
void SendMsg(uint8_t &msg, int len) {
  for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      serverClients[i].write(&msg, len);
      delay(1);
    }
  }
}

void RemoteSender(char* string)
{
  Serial.println("sending");
  unsigned int RawData2[68] = {8900,4450,500,600,550,550,550,600,500,1700,500,600,550,600,500,550,550,550,550,600,500,1750,500,1700,500,550,550,600,500,1700,550,1650,550,1700,500,1750,500,1700,500,1750,450,650,500,550,550,600,500,1700,500,600,500,600,550,550,550,550,550,1700,500,1750,500,1700,500,600,500,1700,550};
  int RawLength = 68;
  irsend.sendRaw(RawData2, RawLength, 38);//38Hz
  delay(100);
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

