#include <SD.h>
#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

#define ENABLE 5
#define DIRA 3
#define DIRB 7

int i;
int tempPin = 0;
int spd;
int chipSelect = 4; //chip select pin for MicroSD Card Adapter
File file; // file object

void setup() {
  clock.begin();   // Initialize DS3231
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  clock.setDateTime(__DATE__, __TIME__);

  Serial.begin(9600);
  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
  pinMode(chipSelect, OUTPUT); //chipSelect to OUTPUT
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card.");
  }
  if (SD.exists("temp.txt")) {
    Serial.println("File exists.");
    if (SD.remove("temp.txt") == true) {
      Serial.println("Successfully removed file.");
    } else {
      Serial.println("Could not removed file.");
    }
  }
}

void loop() {
 dt = clock.getDateTime();
 digitalWrite(ENABLE,LOW); // disable
 int tempReading = analogRead(tempPin);
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK ); //  Temp K
  float tempC = tempK - 273.15; // K to C
  float tempF = (tempC * 9.0)/ 5.0 + 32.0; // C to F
  Serial.print(dt.year);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.day);    Serial.print(" ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println("");
  Serial.print("Temp = ");
  Serial.println(tempC);
  spd = tempC*7; //ADJUST!!
  if (spd >= 255){
    spd=255; //MAX=255
  }
  Serial.print("motor speed: ");
  Serial.println(spd);
  //analogWrite(ENABLE,255); //enable on
  digitalWrite(DIRA,HIGH); //one way
  digitalWrite(DIRB,LOW);
  analogWrite(ENABLE,spd); //max=255
  file = SD.open("temp.txt", FILE_WRITE);
  if (file) {
      file.print(dt.year);   file.print("-");
      file.print(dt.month);  file.print("-");
      file.print(dt.day);    file.print(" ");
      file.print(dt.hour);   file.print(":");
      file.print(dt.minute); file.print(":");
      file.print(dt.second); file.println("");
      file.print("Temp = ");
      file.println(tempC);
      file.print("motor speed: ");
      file.println(spd);
      file.close();
    Serial.print("Wrote data: "); // debug output
  } else {
    Serial.println("Could not open file (writing).");
  }
  delay(1000);
}
