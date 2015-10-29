#include <OneWire.h>

#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);



void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  sensors.begin();
  pinMode(4, INPUT);

}

void loop() {
  // read the analog in value:
  Serial.print("DATA@");
  Serial.print(millis());
  Serial.print("--->A0=" );  int sensorValue = analogRead(A0);  Serial.print(sensorValue);
  Serial.print(",A1=" );  sensorValue = analogRead(A1);  Serial.print(sensorValue);
  Serial.print(",A2=" );  sensorValue = analogRead(A2);  Serial.print(sensorValue);
  Serial.print(",D0=" );  sensorValue = digitalRead(4);  Serial.print(sensorValue);
  Serial.print(",TEMP=" );  sensors.requestTemperatures(); // 发送命令获取温度
  Serial.println(sensors.getTempCByIndex(0));



  /*
  sensorValue = analogRead(A0);
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  Serial.print("A0 = " ); Serial.print(sensorValue);Serial.print(",outputValue = " );Serial.println(outputValue);
  sensorValue = analogRead(A1);
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  Serial.print("A1 = " ); Serial.print(sensorValue);Serial.print(",outputValue = " );Serial.println(outputValue);
  sensors.requestTemperatures(); // 发送命令获取温度
  Serial.print("D0 = " ); Serial.println(sensors.getTempCByIndex(0));
  Serial.println("+++++++++++++++++++++++++++++++++++++" );
  */
  delay(10000);
}
