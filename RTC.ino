/*
 * Real time clock module
 *
 * Connections:
 * GND to GND
 * VCC to power
 * SDA to SDA1
 * SCL to SCL1
*/
#include <I2C_RTC.h>

static DS1307 RTC;

void setup() {
  Serial.begin(9600);
  RTC.begin();

  RTC.setHourMode(CLOCK_H12);

  if (RTC.getHourMode() == CLOCK_H12) {
    RTC.setMeridiem(HOUR_PM);
  }

  //RTC.setDate(1, 5, 24);
  //RTC.setTime(11, 47, 0);
}

void loop() {
  Serial.print(RTC.getHours());
  Serial.print(":");
  Serial.print(RTC.getMinutes());
  Serial.print(":");
  Serial.println(RTC.getSeconds());
}
