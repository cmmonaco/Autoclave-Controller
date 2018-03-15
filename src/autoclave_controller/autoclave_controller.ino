#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

//LCD Control Pins
#define LCD_CS 13 // Chip Select goes to Analog 3
#define LCD_CD 12 // Command/Data goes to Analog 2
#define LCD_WR 11 // LCD Write goes to Analog 1
#define LCD_RD 10 // LCD Read goes to Analog 0 

#define Y_MINUS A1
#define X_MINUS A0
#define Y_PLUS A3
#define X_PLUS A2

#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define TS_MINX 950
#define TS_MAXX 91
#define TS_MINY 127
#define TS_MAXY 900

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SENSOR_PIN A5

//Define states and modes
enum state {
  IDLE,
  RUNNING_TIMER,
  RUNNING_HOLD,
  SET_TIME,
  SET_PRESSURE
};
volatile state currentState = IDLE;
state previousState = -1;

enum mode {
  TIMER,
  HOLD
};
mode currentMode = HOLD;

byte isPressed = 0;
long int stateChangeTime = 0;

//Function prototypes
void idle_state();
void running_hold_state();
void running_timer_state();
void set_time_state();
void set_pressure_state();

volatile int pressed = 0;
volatile float pressure = 0.0;
int setPoint = 0;
long setTime = 0;
volatile long currentTime = 0;

//define touchable areas on screen as a rectantnle with diagonal points
//defined by {x1,y1,x2,y2}
const int PRESSURE_SET_AREA[] = {0, 0, 340, 200};
const int TIME_SET_AREA[] = {90, 200, 360, 320};
const int START_STOP_AREA[] = {361, 200, 480, 320};
const int MODE_CHG_AREA[] = {0, 200, 90, 320};

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
TouchScreen ts = TouchScreen(X_PLUS, Y_PLUS, X_MINUS, Y_MINUS, 278);

void setup()
{
  Serial.begin(9600);
  
  //Get Screen Ready
	tft.reset();
	uint16_t identifier = tft.readID();
	tft.begin(identifier);
  tft.setRotation(1); //set to landscape

  //Draw start-up message
  //tft.fillScreen(WHITE);
  //Draw font "Incubator Controller v1.0"


  //Draw all static elements
	//Divide up Screen
	tft.fillScreen(BLACK);
	tft.drawFastHLine(0, 200, 480, WHITE);
	tft.drawFastVLine(360, 200, 120, WHITE);
	tft.drawFastVLine(90, 200, 120, WHITE);
	tft.drawFastVLine(340, 0, 200, WHITE);

  //Draw static text
  tft.setFont(&FreeSans18pt7b);
  tft.setCursor(265,135);
  tft.print("KPa");
  tft.setCursor(385, 80);
  tft.print("ON");
  tft.setCursor(375, 160);
  tft.print("OFF");
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(10, 180);
  tft.print("Set Point: ");
  tft.setFont(&FreeSans18pt7b);
  tft.print(setPoint);
  tft.setFont(&FreeSans12pt7b);
  tft.print("  KPa");
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(360, 20);
  tft.print("Relay Status"); 

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
  switch(currentState)
  {
    case IDLE: idle_state(); break;
    case RUNNING_HOLD: running_hold_state(); break;
    case RUNNING_TIMER: running_timer_state(); break;
    case SET_TIME: set_time_state(); break;
    case SET_PRESSURE: set_pressure_state();
  }
}

void idle_state()
{
  Serial.println("IDLE STATE");

  if(previousState != IDLE)
  {
    //Set relay off
    //Draw Relay status off (function)
    tft.drawRect(350, 30, 120, 80, BLACK);
    tft.drawRect(351, 31, 118, 78, BLACK);
    tft.drawRect(352, 32, 116, 76, BLACK);
    tft.drawRect(350, 110, 120, 80, RED);
    tft.drawRect(351, 111, 118, 78, RED);
    tft.drawRect(352, 112, 116, 76, RED);
    
    //Draw Start/stop button to START (function)
    tft.fillRect(395, 230, 61,61, BLACK);
    tft.fillTriangle(395 ,290, 395, 230, 455, 260, GREEN);
    
    //draw currentTime
    if(currentMode == TIMER)
    {
      tft.setTextSize(1);
      tft.setFont(&FreeSans9pt7b);
      tft.setCursor(105, 225);
      tft.print("Remaining Time");
      tft.setFont();
      tft.setCursor(105, 245);
      tft.setTextSize(6);
      tft.print("0:00:00"); //function to make string
    }
    else if(currentMode == HOLD)
    {
      tft.setTextSize(1);
      tft.setFont(&FreeSans9pt7b);
      tft.setCursor(105, 225);
      tft.print("Running Time");
      tft.setFont();
      tft.setCursor(105, 245);
      tft.setTextSize(6);
      tft.print("0:00:00");
    }
    
    previousState = IDLE;
  }

  //print updated pressure reading
  tft.setFont();
  tft.setCursor(5,30);
  tft.setTextSize(15);
  tft.print("000"); //tft.print(pressure);
     
  TSPoint p = ts.getPoint();
  int x, y;
  Serial.println(p.z);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE && isPressed == 0)
  {
    isPressed = 1;
    x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    
    //if within pressure area go to set pressure mode 
    if((x > PRESSURE_SET_AREA[0] && x < PRESSURE_SET_AREA[2]) && (y > PRESSURE_SET_AREA[1] && y < PRESSURE_SET_AREA[3]))
    {
      currentState = SET_PRESSURE;
    }
    //if withing time area AND mode is timer, go to set time mode
    else if(((x > TIME_SET_AREA[0] && x < TIME_SET_AREA[2]) && (y > TIME_SET_AREA[1] && y < TIME_SET_AREA[3])) && currentMode == TIMER)
    {
      currentState = SET_TIME;
    }
    //if within start stop mode, go to correct running mode
    else if((x > START_STOP_AREA[0] && x < START_STOP_AREA[2]) && (y > START_STOP_AREA[1] && y < START_STOP_AREA[3]))
    {
      tft.drawRect(350, 110, 120, 80, BLACK);
      tft.drawRect(351, 111, 118, 78, BLACK);
      tft.drawRect(352, 112, 116, 76, BLACK);
      tft.drawRect(350, 30, 120, 80, GREEN);
      tft.drawRect(351, 31, 118, 78, GREEN);
      tft.drawRect(352, 32, 116, 76, GREEN);
      tft.fillRect(395, 230, 61,61, BLACK);
      tft.fillRect(395, 230, 60,60, RED);
      
      if(currentMode == TIMER)
      {
        currentState = RUNNING_TIMER;
        currentTime = setTime;
      }
      else if(currentMode == HOLD)
      {
        currentState = RUNNING_HOLD;
      }
    }    
    //if within mode change chane mode from timer to/from hold
    else if((x > MODE_CHG_AREA[0] && x < MODE_CHG_AREA[2]) && (y > MODE_CHG_AREA[1] && y < MODE_CHG_AREA[3]))
    {
      if(currentMode == TIMER)
      {
        currentMode = HOLD;
        //Draw hold text
        Serial.println("MODE = HOLD");
      }
      else if(currentMode == HOLD)
      {
        currentMode = TIMER;
        //draw timer text
        Serial.println("MODE = TIMER");
      }
    }
  }
  else if(p.z < MINPRESSURE && p.z >= 0)
  {
    isPressed = 0;  
  }
}

void running_hold_state()
{
  Serial.println("RUNNING HOLD STATE");  
  //Upddate current pressure
  //update current time

  TSPoint p = ts.getPoint();
  int x, y;
  Serial.println(p.z);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE && isPressed == 0)
  {
    isPressed = 1;
    x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());

    Serial.print(x); Serial.print(", "); Serial.println(y);
        
    if((x > START_STOP_AREA[0] && x < START_STOP_AREA[2]) && (y > START_STOP_AREA[1] && y < START_STOP_AREA[3]))
    {
      //setTime to zero
      currentTime = setTime;
      currentState = IDLE;
      previousState = RUNNING_HOLD;
    }
  }
  else if(p.z < MINPRESSURE && p.z >= 0)
  {
    isPressed = 0;
  }
}

void running_timer_state()
{
  Serial.println("RUNNING TIMER STATE");

  //update pressure
  //update time

  TSPoint p = ts.getPoint();
  int x, y;

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE && isPressed == 0)
  {
    isPressed = 1;
    x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());

    if((x > START_STOP_AREA[0] && x < START_STOP_AREA[2]) && (y > START_STOP_AREA[1] && y < START_STOP_AREA[3]))
    {
      //currentTime to initial setTime
      currentState = IDLE;
      previousState = RUNNING_TIMER;
    }
  }
  else if(p.z < MINPRESSURE && p.z >= 0)
  {
    isPressed == 0;
  }
}

void set_pressure_state()
{
  Serial.println("SET PRESSURE STATE");

  TSPoint p = ts.getPoint();
  int x, y;

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());

    //ok area
    if((x > START_STOP_AREA[0] && x < START_STOP_AREA[2]) && (y > START_STOP_AREA[1] && y < START_STOP_AREA[3]))
    {
      //set set-point
      currentState = IDLE;
      previousState = SET_PRESSURE;
    }
  }
}

void set_time_state()
{
  Serial.println("SET TIMER STATE");

  //setTime is whatever user chooses
  currentTime = setTime;
  
  TSPoint p = ts.getPoint();
  int x, y;

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());

    if((x > START_STOP_AREA[0] && x < START_STOP_AREA[2]) && (y > START_STOP_AREA[1] && y < START_STOP_AREA[3]))
    {
      //set time
      currentState = IDLE;
      previousState = SET_TIME;
    }
  }
}


ISR(TIMER1_COMPA_vect)
{
  pressed = 0;
  //1. Read Sensor and store value
  pressure = analogRead(SENSOR_PIN);

  //2. increment or decrement timer by one second
  if(currentState == RUNNING_TIMER)
  {
    currentTime--;
  }
  else if(currentState == RUNNING_HOLD)
  {
    currentTime++;
  }
}

