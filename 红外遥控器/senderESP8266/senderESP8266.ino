/*
 * IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to ESP8266 pin 0.
 * Version 0.1 June, 2015
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
 */

#include <IRremoteESP8266.h>

IRsend irsend(2); //an IR led is connected to GPIO pin 2

String Command = "";

void setup()
{
  irsend.begin();
  Serial.begin(9600);
}

bool RemoteSender()
{
  int j;
  int RawLength = Serial.parseInt();
  unsigned int RawData[RawLength];
  for (j = 0; j < RawLength; j++)
  {
    RawData[j] = Serial.parseInt();
  }
  irsend.sendRaw(RawData, RawLength, 38);
  delay(100);
  Serial.println("done");
}

void loop() {
  if (Serial.available() > 0)
  {
    //读取前10个字符，这部分作为命令识别码
    int i;
    for (i = 0; i < 10; i++)
    {
      Command += (char)Serial.read();
      delay(2);
    }
    Serial.println(Command);
    //识别命令,RemoteSend,68,8850,4500,500,600,500,650,450,650,450,1750,500,600,500,650,450,600,500,650,500,600,500,1750,450,1750,450,650,500,600,500,1750,450,1750,450,1750,500,600,500,600,500,650,450,1800,450,1750,450,650,450,1750,500,600,500,1750,450,1750,450,1750,500,650,450,650,450,1750,450,650,500,1750,450
    if (Command == "RemoteSend")
    {
      RemoteSender();
    }
    Command = "";
  }
}
