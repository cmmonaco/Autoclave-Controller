//==========================================================
// Autoclave Controller v1.0
//
// A simple Arduino based controller for the Decatur Makers
// Community Lab autoclave. This controller monitors
// pressure and then will toggle a high power relay to keep
// pressure within specified limits. While running, the
// controller will display the current pressure, set point
// pressure, and a running timer.
//==========================================================

#include <LiquidCrystal.h>

#define SENSOR_PIN A0
#define RELAY_PIN 8
#define SET_PIN A4
#define START_STOP_PIN 2

#define SETPOINT_LOWER 0
#define SETPOINT_UPPER 140

#define LOWER_PRESS_THRESHOLD 10
#define UPPER_PRESS_THRESHOLD 10

#define PRESSURE_KPA(V) ((7.7 * V) - 3.18) * 6.89476

//Define states and modes
enum state {
  IDLE,
  RUNNING
};
volatile state currentState = IDLE;
state previousState = (state)-1;

//Function prototypes
void idle_state();
void running();

volatile int pressure = 0;
int pressed = 0;
int previousPressure = 0;
int setPoint = 0;
volatile long currentTime = 0;
volatile long previousTime = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 7);

void setup() {

  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(START_STOP_PIN, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(START_STOP_PIN), pin_ISR, FALLING);
  
  lcd.begin(2,16);
  lcd.setCursor(0, 0);
  lcd.print("P:  0 kPa");
  lcd.setCursor(12, 0);
  lcd.print("STOP");
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.setCursor(11, 1);
  lcd.print("00:00");
  
  //Set-up Timer1 for a 1 sec interrupt
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

void loop()
{
  if(digitalRead(START_STOP_PIN) == LOW && pressed == 0)
  {
    Serial.print("pressed");
    pressed = 1;
    if(currentState == IDLE)
    {
      currentState = RUNNING;
      previousState = IDLE;
    }
    else if(currentState == RUNNING)
    {
      currentState = IDLE;
      previousState = RUNNING;
    }
  }
  else if(digitalRead(START_STOP_PIN) == HIGH)
  {
    pressed = 0;
  }

  switch(currentState)
  {
    case IDLE: idle_state(); break;
    case RUNNING: running();
  }
}
 
void idle_state() {
  
  int setPointReading = 0;
  
  if(previousState != IDLE)
  {
    //Set relay off
    digitalWrite(RELAY_PIN, LOW);
    
    lcd.setCursor(12, 0);
    lcd.print("STOP");
    
    lcd.setCursor(11,1);
    lcd.print("00:00");
    
    previousState = IDLE;
  }
  
  if(pressure != previousPressure)
  {
    lcd.setCursor(2, 0);
    if(pressure < 10)
    {
      lcd.print("  ");
    }
    else if(pressure < 100)
    {
      lcd.print(" ");
    }
    lcd.print(pressure);
    lcd.print(" kPa");

    previousPressure = pressure;
  }
  
  setPoint = map(analogRead(SET_PIN), 0, 1023, SETPOINT_LOWER, SETPOINT_UPPER);
  lcd.setCursor(2, 1);
  if(setPoint < 10)
  {
    lcd.print("  ");
  }
  else if(setPoint < 100)
  {
    lcd.print(" ");
  }
  lcd.print(setPoint);
  lcd.print(" kPa");
  
}

void running() {
  if(previousState != RUNNING)
  {
    lcd.setCursor(12, 0);
    lcd.print(" RUN");
    
    previousState = RUNNING;
  }
  
  if(pressure <= setPoint - LOWER_PRESS_THRESHOLD)
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else if(pressure >= setPoint + UPPER_PRESS_THRESHOLD)
  {
    digitalWrite(RELAY_PIN, LOW);
  }
   
  if(pressure != previousPressure)
  {
    lcd.setCursor(2, 0);
    if(pressure < 10)
    {
      lcd.print("  ");
    }
    else if(pressure < 100)
    {
      lcd.print(" ");
    }
    lcd.print(pressure);
    lcd.print(" kPa");
  }
  
  if(currentTime != previousTime)
  {
    lcd.setCursor(11,1);
    lcd.print(((currentTime/60)%60 < 10) ? "0" + String((currentTime/60)%60) : (currentTime/60)%60);
  	lcd.print(":");
    lcd.print((currentTime%60 < 10) ? "0" + String(currentTime%60) : currentTime%60);
	
    previousTime == currentTime;
  }
}

ISR(TIMER1_COMPA_vect)
{
  double sensorVoltage = 0;

  //1. Read Sensor and store value
  sensorVoltage = analogRead(SENSOR_PIN) * 0.0049;
  pressure = PRESSURE_KPA(sensorVoltage);
  pressure = (pressure < 0) ? 0 : pressure;

  if(currentState == RUNNING)
  {
    currentTime++;
  }
}

/*void pin_ISR()
{
  if(currentState == IDLE)
  {
    currentState = RUNNING;
    previousState = IDLE;
  }
  else if(currentState == RUNNING)
  {
    currentState = IDLE;
    previousState = RUNNING;
  }
}*/