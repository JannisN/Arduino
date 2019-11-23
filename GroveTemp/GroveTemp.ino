#include <math.h>
#include <Wire.h>
#include "rgb_lcd.h"

struct Timer
{
  void update();
  void (*func)(); 
  unsigned long duration;
  unsigned long time;
  unsigned long oldTime;
};

void Timer::update()
{
  auto newTime = millis();

  // the variables overflowing is not a problem: if newTime > oldTime, the result of newTime - oldTime will still be newTime + (maxunsignedlong - oldTime), which gives the right value
  time += newTime - oldTime;
  oldTime = newTime;

  if (time > duration)
  {
    func();
    time -= duration;
  }
}

constexpr double b = 4275;
constexpr double r0 = 100000;
constexpr int pin = A0;

constexpr double bCustom = 4600;

constexpr auto pinBlue = 10;
constexpr auto pinGreen = A5;

rgb_lcd lcd;

Timer mainTimer;

Timer roundTimer;
int greenState = 0;
int rounds = 0;

void loopMain () {
  // put your main code here, to run repeatedly:
  double a = analogRead(pin);
  double r = (1023.0 / a - 1.0) * r0;
  double t = 1.0 / (log(r / r0) / b + 1.0 / 298.15) - 273.15;

  double aCustom = analogRead(A2);
  double rCustom = (1023.0 / aCustom - 1.0) * r0;
  double tCustom = 1.0 / (log(rCustom / r0) / bCustom + 1.0 / 298.15) - 273.15;

  double angle = analogRead(A1) / 1023.0;

  if (t > 25.0 + 5.0 * (angle - 0.5))
    lcd.setRGB(255, 0, 0);
  else
    lcd.setRGB(0, 255, 0);

  double highTrashold = 25.0 + 5.0 * (angle - 0.5) + 3.0;
  double lowTrashold = highTrashold - 3.0;

  if (tCustom > highTrashold)
  {
    analogWrite(pinBlue, 255);
  }
  else if (tCustom < lowTrashold)
  {
    analogWrite(pinBlue, 0);
  }
  
  int rpm = rounds * 15; // 60 / 4
  rounds = 0;
  
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(7, 1);
  lcd.print(tCustom);

  Serial.print(t);
  Serial.print(" ");
  Serial.print((t > 25.0 + 5.0 * (angle - 0.5)) * 50);
  Serial.print(" ");
  Serial.print(tCustom);
  Serial.print(" ");
  Serial.print(highTrashold);
  Serial.print(" ");
  Serial.print(lowTrashold);
  Serial.print(" ");
  //Serial.println(analogRead(pinGreen));
  Serial.println(rpm);
}


void countRounds()
{
  auto state = analogRead(pinGreen);
  if (state < 500)
  {
    if (greenState > 500)
    {
      greenState = state;
      rounds++;
    }
  }
  else if (state >= 500)
  {
    if (greenState < 500)
    {
      greenState = state;
      rounds++;
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(pinGreen, INPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 0, 0);
  lcd.print("Temperature:");
  mainTimer.func = loopMain;
  mainTimer.duration = 1000;
  roundTimer.func = countRounds;
  roundTimer.duration = 1;
}

void loop()
{
  mainTimer.update();
  roundTimer.update();
  delay(1);
}
