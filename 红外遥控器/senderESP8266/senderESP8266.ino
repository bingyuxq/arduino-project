#define DEBUG
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

void RemoteSender(char* string)
{
  char *dest[500];
  int num = 0;
  split(string, ",", dest, &num);
  
  int RawLength = atoi(dest[0]);
  unsigned int RawData[RawLength];
  for (int i = 1; i < num; i++) {
    RawData[i-1] = atoi(dest[i]);
    #ifdef DEBUG
    Serial.print(i); Serial.print(":");Serial.println(RawData[i - 1] );
    #endif
  }

    #ifdef DEBUG
    Serial.println(RawLength);
    #endif
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

//char[]来源，const char分隔符，char*目标缓存数组的指针(buff为char* []类型，int元素数缓存
void split(char *src, const char *separator, char **dest, int *num)
{
    char *pSeparator, *pStart, *pEnd;
    unsigned int sep_len;
    int count = 0;
    
    if (src == NULL || strlen(src) == 0) return;
    
    pSeparator = (char *)malloc(16);
    if (pSeparator == NULL) return;
    
    if (separator == NULL || strlen(separator) == 0) strcpy(pSeparator," ");/* one blank by default */
    else strcpy(pSeparator,separator);

    sep_len = strlen(pSeparator);
        
    pStart = src;
    
    while(1)
    {
        pEnd = strstr(pStart, pSeparator);
        if (pEnd != NULL)
        {
            memset(pEnd,'\0',sep_len);
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
