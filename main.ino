#include <TFT.h>
#include <SPI.h>
#include <Esplora.h>

#define READ_PAUSE 50 //pause between reading another value of sensor

//following values refers to how many of the read_pause it will take
#define LIGHTS_SIZE 100 //this many past values it will remember
#define TIME_BOUND 20 //everything above is detected as -
#define PAUSE_LENGTH 40 //after this many stable detects, the char is constructed from the signals

#define DEVIATION 10 //if the signal differs more than this, then it will be detected as - or .

//-------------------------------------------------------
// Global variables

//variables for finite-state machine
enum states {WAITING, DETECTING, READING, DISPLAYING};
enum states state = WAITING, nextState;

int lights[LIGHTS_SIZE]; //array that remembers intensity of the light in last LIGHTS_SIZE * READ_PAUSE / 1000 seconds
int stable; //average light intensity intensity of the last stable interval of length  LIGHTS_SIZE * READ_PAUSE / 1000
int charsShown = 0; //how many chars are shown on the screen
//-------------------------------------------------------
//Changing screens for different states

void setupScreen()
{
  EsploraTFT.background(255, 255, 255);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.setTextSize(3);
  EsploraTFT.text("Wait", 10, 30);
  EsploraTFT.text("loading", 10, 70);
}
void waitingScreen()
{
  EsploraTFT.background(255, 255, 255);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.setTextSize(3);
  EsploraTFT.text("flash S", 10, 30);
  EsploraTFT.text("to START", 10, 70);
}
void readingScreen()
{
  EsploraTFT.background(255, 255, 255);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.setTextSize(1);
  EsploraTFT.text("Listening", 0, 0);
}
void changeToDetectingScreen()
{
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.rect(0, 0, EsploraTFT.width(), 10);
  EsploraTFT.setTextSize(1);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text("detecting", 0, 0);
}
void changeToDisplayingScreen(bool exceeded = false)
{
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.rect(0, 0, EsploraTFT.width(), 10);
  EsploraTFT.setTextSize(1);
  EsploraTFT.stroke(0, 0, 0);
  if(exceeded)
  {
    EsploraTFT.text("Limit exceeded, showing", 0, 0);
  }
  else
  {
    EsploraTFT.text("Showing results", 0, 0);
  }
  EsploraTFT.text("F->Finish, C->Continue,", 0, EsploraTFT.height() - 20);
  EsploraTFT.text("B->Backspace", 0, EsploraTFT.height() - 10);
}
void backToReadingScreen()
{
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.rect(0, 0, EsploraTFT.width(), 10);
  EsploraTFT.rect(0, EsploraTFT.height() - 20, EsploraTFT.width(), EsploraTFT.height());
  EsploraTFT.setTextSize(1);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text("Listening", 0, 0);
}
//-------------------------------------------------------
//working with light intensity

//returns light intensity at the moment
int readLight()
{
  delay(READ_PAUSE);
  return Esplora.readLightSensor();
}
//returns average light intensity in the last LIGHTS_SIZE * READ_PAUSE / 1000 seconds
int getAvg()
{
  unsigned long int sum = 0; //int is only 16 bits long
  for(int i = 0; i<LIGHTS_SIZE; ++i)
  {
     sum += lights[i];
  }
  return sum/LIGHTS_SIZE;
}
//updates stable to the average light intensity in last LIGHTS_SIZE * READ_PAUSE / 1000 seconds
void updateStable()
{
  stable = getAvg();
}
//waits until the light intensity is stable for LIGHTS_SIZE * READ_PAUSE / 1000 seconds and then sets stable
void setupStable()
{
  setupScreen();
  for(int i = 0; i<LIGHTS_SIZE; ++i)
  {
      rememberLight(readLight());
  }
  while(!isStable())
  {
    rememberLight(readLight());
  }
  updateStable();
}
//saves new light intensity and discards th last one 
void rememberLight(int l)
{
  for(int i = 0; i<LIGHTS_SIZE - 1; ++i)
  {
    lights[i] = lights[i + 1];
  }  
  lights[LIGHTS_SIZE - 1] = l; 
}
//checks if all of the measured light intensities are within a DEVIATION from average light intensity in last LIGHTS_SIZE * READ_PAUSE / 1000 seconds
bool isStable()
{
  int lmin = 1024;
  int lmax = 0;
  unsigned long int sum = 0; //int is only 16 bits long
  for(int i = 0; i<LIGHTS_SIZE; ++i)
  {
    if(lights[i] < lmin)
    {
      lmin = lights[i];  
    }
    if(lights[i] > lmax)
    {
      lmax = lights[i];  
    }
    sum += lights[i];
  }
  int avg = sum/LIGHTS_SIZE;
  return lmax - avg < DEVIATION && avg - lmin < DEVIATION;
}
//checks if last saved light intensity is within a DEVIATION from stable
bool isLastStable()
{
  return abs(lights[LIGHTS_SIZE - 1] - stable) < DEVIATION;  
}
//-------------------------------------------------------
//Interpreting signal

//checks end of sequence that defines letter
bool isPause()
{
  for(int i = LIGHTS_SIZE - 1; i > LIGHTS_SIZE - PAUSE_LENGTH - 1; --i)
  {
    if(abs(lights[i] - stable) > DEVIATION)
    {
        return false;
    }  
  }  
  return true;
}
//goes through lights[] until it finds the start of signal and then decides if it was - or .
bool isLong()
{
  //We don't want to check the last one because that is stable
  for(int i = LIGHTS_SIZE - 2; i>=0; --i)
  {
        if(abs(lights[i] - stable) < DEVIATION)
        {
            return (LIGHTS_SIZE - i) > TIME_BOUND;  
        }
  }  
  return true;
}
//-------------------------------------------------------
//Morse decoding

//returns letter based on sequence of signals, signals are received like this: . = 0, - = 1
char morseToChar(int code, int l)
{
 //# means undefined or invalid character
  char l1[2] = {'E', 'T'};
  char l2[4] = {'I', 'A', 'N', 'M'};
  char l3[8] = {'S', 'U', 'R', 'W', 'D', 'K', 'G', 'O'};
  char l4[16] = {'H', 'V', 'F', '#', 'L', '#', 'P', 'J', 'B', 'X', 'C', 'Y', 'Z', 'Q', '#', '#'};
  char l5[32] = {'5', '4', '#', '3', '#', '#', '#', '2', '#', '#', '+', '#', '#', '#', '#', '1', '6', '=', '/', '#', '#', '#', '#', '#', '7', '#', '#', '#', '8', '#', '9', '0'};

  if(code < pow(2, l))
  {
    switch(l)
    {
      case 0: return '@'; //this is not an invalid character from user but from the measuring so we want to distinguish them
      case 1: return l1[code];
      case 2: return l2[code];
      case 3: return l3[code];
      case 4: return l4[code];
      case 5: return l5[code];
      default: return '#';
    }
  }
  return '#';
}
//packs signals into a sequence, then through function morseToChar, it returns letter
char readChar()
{
  int res = 0, resl = 0;
  bool awaiting = true;
  while(!isPause())
  {
  rememberLight(readLight());
    if(isLastStable())
    {
      if(awaiting)
      {
        ++resl;
        res = res << 1;
        res += isLong() ? 1 : 0;
        awaiting = false;  
      }
    }
    else
    {
        awaiting = true;  
    }
  }
  return morseToChar(res, resl);
}
//-------------------------------------------------------
//showing received letters to the screen

//setting x and y coordinates to the given return variables
void getCoordinates(int &x, int &y)
{
  x = (charsShown % 14) * 10 + 10; //how many letters fit the screen + offset
  y = (charsShown / 14) * 10 + 15;
}
//shows given char to the screen at next position
bool showChar(char c)
{
  if(charsShown >= 14 * 9)
  {
    return false;  
  }
  
  int xpos,ypos;
  getCoordinates(xpos, ypos);
  EsploraTFT.setTextSize(1);
  EsploraTFT.stroke(0, 0, 0);
  char show[2];
  show[0] = c;
  show[1] = 0;
  EsploraTFT.text(show, xpos, ypos);
  ++charsShown;  
  return true;
}
//erases last character
void backspace()
{
  if(charsShown == 0)
  {
    return;
  }

  --charsShown;
  int xpos, ypos;
  getCoordinates(xpos, ypos);
  
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.fill(255, 255, 255);
  EsploraTFT.rect(xpos, ypos, xpos + 10, ypos + 10);
  EsploraTFT.stroke(0, 0, 0);
}
 
void setup()
{
  Serial.begin(9600);
  EsploraTFT.begin();
  setupStable();
  waitingScreen();
}

void loop() 
{
  rememberLight(readLight());
  Esplora.writeRed(0);
  switch(state) //finite-state machine
  {
    case WAITING:  
      if(isLastStable())
      {
        updateStable();
        nextState = WAITING;  
      }
      else
      {
        changeToDetectingScreen();
        nextState = DETECTING;
      }
      break;
    case DETECTING: 
      if(isStable())
      {
        waitingScreen();
        updateStable();
        nextState = WAITING;  
      }
      else
      {
        if(readChar() == 'S')
        {
          readingScreen();
          charsShown = 0;
          nextState = READING;
        }
        else
        {
          Esplora.writeRed(50);
          nextState = DETECTING;
        }
      }
      break;
    case READING:
      if(!isLastStable())
      {
        char c = readChar();
        if(c == '#')
        {
          Esplora.writeRed(50); 
        }
        else if(c == '@')
        {}
        else
        {
          if(showChar(c))
          {
            nextState = READING;
          }
          else
          {
            changeToDisplayingScreen(true);
            nextState = DISPLAYING;    
          }
        }
      }
      else if(isStable())
      {
        changeToDisplayingScreen();
        nextState = DISPLAYING;  
      }
      else 
      {
          nextState = READING;  
      }
      break;
    case DISPLAYING:
      if(isLastStable())
      {
        nextState = DISPLAYING;  
      }
      else
      {
        char c = readChar();
        if(c == 'F')
        {
          setupStable();
          waitingScreen();
          nextState = WAITING;  
        }  
        else if(c == 'C')
        {
          backToReadingScreen();
          nextState = READING; 
        }
        else if(c == 'B')
        {
          backspace();
          nextState = DISPLAYING;  
        }
        else
        {
          Esplora.writeRed(50);
        }
      }
      break;
  }
  state = nextState;
}
