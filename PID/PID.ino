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

constexpr double bCustom = 4600;
constexpr double r0 = 100000;

constexpr auto pinThermistor = A2;
constexpr auto pinBlue = 10;
constexpr auto pinGreen = A5;

constexpr auto mainTimerDelay = 1000;

rgb_lcd lcd;

Timer mainTimer;

Timer roundTimer;
int greenState = 0;
int rounds = 0;

struct PidController
{
	double wantedTemp;
	double lastError;
	double sumError;
	double Kp;
	double Ki;
	double Kd;
	int getIntensity(double temp)
	{
		double error = temp - wantedTemp;
		sumError += error;
		int intensity = max(min(128 + (Kp * error + Ki * sumError + Kd * (error - lastError)) * 5, 255), 0);
		lastError = error;
		return intensity;
	}
};

PidController pid;

void loopMain ()
{
  double aCustom = analogRead(pinThermistor);
  double rCustom = (1023.0 / aCustom - 1.0) * r0;
  double tCustom = 1.0 / (log(rCustom / r0) / bCustom + 1.0 / 298.15) - 273.15;
  
  int rpm = rounds / 4 * 60 * 1000 / mainTimerDelay;
  rounds = 0;
  
  lcd.setCursor(0, 1);
  lcd.setCursor(0, 1);
  lcd.print(tCustom);

  auto intensity = pid.getIntensity(tCustom);
  analogWrite(pinBlue, intensity);

  //Serial.print(pid.lastError);
  //Serial.print(" ");
  //Serial.print(pid.sumError);
  //Serial.print(" ");
  Serial.print(tCustom);
  //Serial.print(" ");
  //Serial.print(intensity);
  Serial.print(" ");
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


void setup()
{
  Serial.begin(9600);
  pinMode(pinThermistor, INPUT);
  pinMode(pinGreen, INPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255);
  lcd.print("Temperature:");
  mainTimer.func = loopMain;
  mainTimer.duration = mainTimerDelay;
  roundTimer.func = countRounds;
  roundTimer.duration = 1;
  pid.Kd = -30;
  pid.Ki = 1;
  pid.Kp = 50.0;
  pid.wantedTemp = 25;
  pid.sumError = 0;
  pid.lastError = 0;
}

void loop()
{
  mainTimer.update();
  roundTimer.update();
  delay(1);
}
