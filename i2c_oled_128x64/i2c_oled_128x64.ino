#include <Wire.h> // support for I2C operations
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "UiFx.h"

#define LED_ON  LOW
#define LED_OFF HIGH

#define LONG_PRESS  550 // in millis thresold to detect a long press on a button

const short int BUILTIN_LED1=2;   // GPIO 2 

const short int BUTTON_LEFT=12; // GPIO 12, PIN D6
const short int BUTTON_RIGHT=14; // GPIO 14, PIN D5
 
const short int SDApin = 4;  // pin D2 for SDA , GPIO 4
const short int SCLpin = 5;  // pin D1 for SCL , GPIO 5

#define OLED_RESET 0
const short int OLED_I2C_ADDR = 0x3C; 


Adafruit_SSD1306 display(OLED_RESET);


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please patch Adafruit_SSD1306.h!");
#endif

int currentLine=0;

void setup() {
  Serial.begin( 115200 );
  Serial.print( "\n* Setup BEGIN" );

  Serial.print("  setting pinMode ...");
  pinMode(BUILTIN_LED1,OUTPUT);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), buttonLeftChange, CHANGE); //CHANGE, RISING or FALLING
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), buttonRightChange, CHANGE);

  
  Serial.print("  setting display ...");
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR); // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(500);
  display.clearDisplay();

  currentLine=0;
  constructStatusScreen(currentLine);
  display.display();
  Serial.print( "\nSetup END" );
}

int menuLineToActivate=-1;

void loop() {
  Serial.print( "\n* LOOP" );
  Serial.print( "\n    Current line : " );
  Serial.print(currentLine);
   
  //currentLine=(currentLine+1)%5;
  //writeStatusScreen(currentLine);
  //display.display();
  
  digitalWrite(BUILTIN_LED1,LED_ON);
  delay(250);

  digitalWrite(BUILTIN_LED1,LED_OFF);
  delay(1000);
}

/******************************************************************************** UI DRAW *****/

void writeHeader()
{
  display.setTextColor(BLACK, WHITE);   // 'inverted' text
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("    KVM   ");
}


void constructStatusScreen(int selectedLine)
{
  display.clearDisplay();
  writeHeader();
  display.setTextSize(1);
  display.setCursor(0,17);
  setColorStatusLine(0,selectedLine);
  display.println("KBD: NO    MOUSE: NO"); 
  display.setCursor(0,28);
  setColorStatusLine(1,selectedLine);
  display.println(" Host1: Not paired ");
  setColorStatusLine(2,selectedLine);
  display.println(" Host2: YOGANICO    ");
    setColorStatusLine(3,selectedLine);
  display.println(" Host3: Not paired ");
    setColorStatusLine(4,selectedLine);
  display.println(" Host4: Not paired ");
}


void constructConfigurationScreen()
{
  display.clearDisplay();
  writeHeader();
  display.setTextSize(1);
  display.setTextColor(BLACK,WHITE);
  display.setCursor(0,17);
  display.setCursor(0,28);
  display.println(" CONFIGSCREEN ");
  display.println("\n\n left to exit ");
}


void setColorStatusLine(int currentLine, int reverseLine)
{
  if (currentLine == reverseLine)
    display.setTextColor(BLACK,WHITE);
  else
    display.setTextColor(WHITE,BLACK);
}

/***************************************************************** UI button management ***/

unsigned long leftPressTime; // used to detect short/long button press, and debouncing
unsigned long rightPressTime; // used to detect short/long button press, and deboucing


void buttonLeftChange()
{
  Serial.print("\nLEFT CHANGE");

  if (digitalRead(BUTTON_LEFT)==LOW) // button PRESSED
  {
    leftPressTime=millis();
  }
  else // button released
  {
   if (millis()-leftPressTime<LONG_PRESS)
      buttonLeftShortPress();
    else
      buttonLeftLongPress();
  }  
}


void buttonRightChange()
{
  Serial.print("\nRIGHT CHANGE");
  if (digitalRead(BUTTON_RIGHT)==LOW) // button pressed
  {
    rightPressTime=millis();
  }
  else // button released
  {
    if (millis()-rightPressTime<LONG_PRESS)
      buttonRightShortPress();
    else
      buttonRightLongPress();
  }  
}

void buttonLeftShortPress()
{
  currentLine=(currentLine-1)%5;
  if (currentLine<0) 
    currentLine=4;
  constructStatusScreen(currentLine);
  display.display();
}


void buttonRightShortPress()
{
  currentLine=(currentLine+1)%5;
  constructStatusScreen(currentLine);
  display.display();
}

void buttonLeftLongPress()
{
    Serial.print("   LEFT LONG PRESS !");
}


void buttonRightLongPress()
{
  Serial.print("   RIGHT LONG PRESS !");
  constructConfigurationScreen();
  display.display();
}


