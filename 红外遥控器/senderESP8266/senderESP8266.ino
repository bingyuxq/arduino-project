/*
 * IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to ESP8266 pin 0.
 * Version 0.1 June, 2015
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
 */
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>

const char* ssid = "bingyu-AP";
const char* password = "bingyuxq";
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

IRsend irsend(13); //an IR led is connected to GPIO pin 13

void setup()
{
  irsend.begin();
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

//ALib Begin//
char** cSplit(char *string, char spliter, int* arrayCount)
{
  //分割后字符串数量
  int num = 0;
  //计算数量
  int i = 0;
  int size = strlen(string);
  for (; i < size; i++) {
    if ((string[i] != spliter) && (string[i + 1] == spliter || string[i + 1] == '\0'))
      num++;
  }
  *arrayCount = num;
  //分配空间
  char **cStrings = (char**)malloc(sizeof(char*) * (num + 1));
  int j = 0;
  for (; j <= num; j++) {
    //初始化指针数组
    cStrings[j] = 0;
  }
  //保存分割后字符串
  char *buffer = (char*)malloc(sizeof(char) * (size + 1));
  int l = 0;
  for (; l < size + 1; l++) {
    buffer[l] = 0;
  }
  int counter = 0;
  int k = 0;
  for (; k < size; k++) {
    if (string[k] != spliter) {
      int i = 0;
      int j = k;
      while (string[j] != spliter && string[j] != '\0') buffer[i++] = string[j++];
      buffer[i] = '\0';
      cStrings[counter] = (char*)malloc(sizeof(char) * i);
      strcpy(cStrings[counter++], buffer);
      k = k + i;
    }
  }
  free(buffer);
  return cStrings;
}
/**
*调用此方法释放cSplit分配的内存空间
*/
void freeStringsMemory(char **strings) {
  int k = 0;
  for (; ; k++) {
    if (strings[k] != 0) free(strings[k]);
    else break;
  }
  free(strings);
}
//ALib End//

bool RemoteSender(char* string)
{
  char** stringarr;
  int arrayCount = 0;
  stringarr = cSplit(string, ',', &arrayCount);
  
  int RawLength = atoi(stringarr[0]);
  unsigned int RawData[RawLength];
  for (int i = 1; i < arrayCount; i++) {
    RawData[i-1] = atoi(stringarr[i]);
  }

  irsend.sendRaw(RawData, RawLength, 38);
  delay(100);
  Serial.println("done");

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil(',');
  if (req.indexOf("RemoteSend") != -1)
  {
    Serial.println("Get command RemoteSend");
    req = client.readStringUntil('\r');
    RemoteSender((char*)req.c_str());
  }
  client.flush();


  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\ndone";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
