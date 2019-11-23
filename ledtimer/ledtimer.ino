// Jannis Naske, 25.10.2019

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

Timer timer;
Timer timer2;

Timer timer3;
Timer timer4;

void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  timer.func = [](){digitalWrite(8, LOW);};
  timer.duration = 1000;
  timer2.func = [](){digitalWrite(8, HIGH);};
  timer2.duration = 1000;
  timer2.time = 500;

  timer3.func = [](){digitalWrite(9, LOW);};
  timer3.duration = 800;
  timer4.func = [](){digitalWrite(9, HIGH);};
  timer4.duration = 800;
  timer4.time = 400;
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.update();
  timer2.update();
  timer3.update();
  timer4.update();
  delay(1);
}
