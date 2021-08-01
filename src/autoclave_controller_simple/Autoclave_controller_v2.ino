//==========================================================
// Autoclave Controller v2.0
//
// A simple Arduino based controller for the Decatur Makers
// Community Lab autoclave. This controller monitors
// pressure and then will toggle a high power relay to keep
// pressure within specified limits. While running, the
// controller will display the current pressure, set point
// pressure, and a running timer.
//==========================================================

#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

#define SENSOR_PIN A2
#define RELAY_PIN 8
#define SET_PIN A0
#define START_STOP_PIN 7
#define RLED_PIN 10

#define SETPOINT_LOWER 0
#define SETPOINT_UPPER 20

#define LOWER_PRESS_THRESHOLD 0.5
#define UPPER_PRESS_THRESHOLD 0

#define PRESSURE_PSI(V) ((0.0364 * V) - 2.9904)

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

volatile float pressure = 0;
int pressed = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;
float previousPressure = 0;
int setPoint = 0;
int setPointReached = 0;
volatile long currentTime = 0;
volatile long previousTime = 0;

// initialize the library with the numbers of the interface pins
Adafruit_LiquidCrystal lcd(0);

void setup() {

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(START_STOP_PIN, INPUT_PULLUP);
  pinMode(RLED_PIN, OUTPUT);
  digitalWrite(RLED_PIN, LOW);
  
  lcd.begin(2,16);
  lcd.setCursor(0, 0);
  lcd.print("P: 0.0 PSI");
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
  int buttonRead = digitalRead(START_STOP_PIN);

  if(buttonRead != lastButtonState)
  {
      lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay)
  {
    if(buttonRead != buttonState)
    {
      buttonState = buttonRead;

      if(buttonState == LOW)
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
      }
    }
  }

  lastButtonState = buttonRead;

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
    digitalWrite(RLED_PIN, LOW);

    setPointReached = 0;
    
    lcd.setCursor(12, 0);
    lcd.print("STOP");
    
    currentTime = 0;
    lcd.setCursor(11,1);
    lcd.print("00:00");
    
    previousState = IDLE;
  }
  
  if(pressure != previousPressure)
  {
    lcd.setCursor(2, 0);
    if(pressure < 10)
    {
      lcd.print(" ");
    }
    
    lcd.print(pressure,1);
    lcd.print(" PSI");

    previousPressure = pressure;
  }
  
  setPoint = map(analogRead(SET_PIN), 1023, 0, SETPOINT_LOWER, SETPOINT_UPPER);
  lcd.setCursor(2, 1);
  if(setPoint < 10)
  {
    lcd.print(" ");
  }
  
  lcd.print(setPoint);
  lcd.print(".0 PSI");
  
}

void running() {
  if(previousState != RUNNING)
  {
    digitalWrite(RLED_PIN, HIGH);
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
    setPointReached = 1;
    digitalWrite(RELAY_PIN, LOW);
  }
   
  if(pressure != previousPressure)
  {
    lcd.setCursor(2, 0);
    if(pressure < 10)
    {
      lcd.print(" ");
    }

    lcd.print(pressure,1);
    lcd.print(" PSI");
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
  int sensorVoltage = 0;
  sensorVoltage = analogRead(SENSOR_PIN);
  pressure =  PRESSURE_PSI(sensorVoltage);  
  pressure = (pressure < 0) ? 0 : pressure;

  if(currentState == RUNNING && setPointReached == 1)
  {
    currentTime++;
  }
}
