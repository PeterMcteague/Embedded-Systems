#include "pitches.h"
#include "notes.h"

int pins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int outputPin = 13;
int buttonPin = A0;
int rbgLedR = A1;
int rbgLedG = A2;
int rbgLedB = A3;
int arrSize = sizeof(pins) / sizeof( int );
int notes[] = {NOTE_C4 , NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4};
int playingPin;

void setRgb(int r, int g, int b)
{
  analogWrite(rbgLedR, r);
  analogWrite(rbgLedG, g);
  analogWrite(rbgLedB, b);
}

void playNoTone()
{
  noTone(outputPin);
  setRgb(0,0,0);
}

void playTone(int note)
{
  tone(outputPin, note);
  setRgb(random(1, 255),random(1, 255),random(1, 255));
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i <= arrSize; i++)
  {
    pinMode(pins[i], INPUT);
  }
  pinMode(outputPin, OUTPUT);
  pinMode (buttonPin, INPUT);
  playNoTone();
  playingPin = NULL;
}

void loop() {  
  //Button
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    playOceanMan();
  }

  //Touch Keyboard
  if (playingPin == NULL or digitalRead(playingPin) != HIGH)
  {
    playingPin = NULL;
    playNoTone();
  }
  for (int i = 0; i < arrSize; i++)
  {
    int pin = pins[i];
    if (digitalRead(pin) == HIGH)
    {
      playingPin = pin;
      playTone(notes[i]);
    }
  }
}

//https://sparks.gogo.co.nz/midi_tone.html fed with https://onlinesequencer.net/398655.
//Neither of these are created by us. notes.h includes some of the code output from midi_tone and this is the main part
//edited to allow it to stop.
void playOceanMan()
{
  delay(500); //Delaying so that our initial press doesn't count as a cancel.
  
  // 8th Octave Frequencies C8 to B8, lower octaves are calculated from this
  static const uint16_t Freq8[] PROGMEM = { 4186 , 4435 , 4699  , 4978 , 5274 , 5588 , 5920 , 6272 , 6645 , 7040 , 7459 , 7902 };

  for(uint16_t  x = 0; x < MelodyLength; x++)
  {
    int buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH)
    {
      playNoTone();
      delay(100); //Adding a small delay so we can let go of the button
      break;
    }
    uint16_t data = pgm_read_word((uint16_t *)&MelodyData[x]);
    if((data & 0xF) == 0xF) 
    {
      playNoTone();
    }
    else
    {
      uint16_t Freq = pgm_read_word(&Freq8[data&0xF]) / ( 1 << (8-(data>>4 & 0xF)) );
      playTone(Freq);    

    }    
    
    int16_t Duration = data>>8;
    while(Duration--) delay(1);
  }
}

