/*
* Name: ovningsprojekt
* Author: Dominick Solmaz
* Date: 2025-10-13
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display, 
* Further, it measures temprature with ds3231 and displays a mapped value to a 9g-servo-motor,
* When the temperature measured with the ds3231 exceeds the threshold value, a fan spins.
*/

// Include Libraries
#include <RTClib.h>
#include <U8glib.h>
#include <Servo.h>

// Init constants
const int servoPin = 9;
const int fanPin = 10;
const int thresh_value = 24;

// Init global variables
int pos = 0;

// Construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;

void setup() {
  // Init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  rtc.begin();
  myservo.attach(servoPin);
  pinMode(fanPin, OUTPUT);
  // Settings
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_unifont);
}

void loop() {
  float temp = getTemp();
  String day = getDay();
  String time = getTime();

  oledWrite(time, day, temp);
  servoWrite(temp);
  fanWrite(temp);

  Serial.println(time);
  Serial.println(temp);

  // Delay for better operating
  delay(200);
}

//This function reads time from an ds3231 module and package the time as a String
//Parameters: Void
//Returns: time in hh:mm:ss as String
String getTime() {
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

//This function obtains the current weekday from the ds3231 and returns it as a String
//Parameters: Void
// Returns: weekday String
String getDay() {
  DateTime now = rtc.now();
  char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  return daysOfTheWeek[now.dayOfTheWeek()];
}

/*
* This function takes temperature from ds3231 and returns as a float
*Parameters: Void
*Returns: temprature as float 
*/
float getTemp() {
  return rtc.getTemperature();
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String time, String day, float temp) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 20, ("Time: " + time).c_str());
    u8g.drawStr(0, 40, ("Weekday: " + day).c_str());
    u8g.drawStr(0, 60, ("Temp: " + String(temp)).c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temperature
*Returns: void
*/
void servoWrite(float temp_value) {
  pos = map(temp_value, 20, 30, 0, 180);
  myservo.write(pos);
  delay(15);
}

/* 
* takes the temperature value and makes the fan spin when the degrees exceed the threshold value
*Parameters: - value: temperature
*Returns: void
*/
void fanWrite(float temp_value) {
  if (temp_value >= thresh_value) {
    digitalWrite(fanPin, HIGH);
  } else {
    digitalWrite(fanPin, LOW);
  }
}
