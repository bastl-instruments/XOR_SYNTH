 // libraries
#include <MIDI.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <trinityHW.h>
#include <MozziGuts.h>
#include <mozzi_midi.h>

#include <mozzi_rand.h>

#include <mozzi_analog.h>

#include <envelope.h>  
#include <lfo.h>
#include <Oscil.h>
#include <sequencer.h>


#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h> 

#include <tables/triangle2048_int8.h>

//#include <tables/whitenoise2048_int8.h>
//#include <tables/square_no_alias512_int8.h>
//#include <python/hat.h>
unsigned char inputChannel;

const char* WAVE_TABLES[]={
  SIN2048_DATA, SAW2048_DATA, TRIANGLE2048_DATA};//SQUARE_NO_ALIAS512_DATA};//HAT_DATA};//WHITENOISE2048_DATA};

#define NUMBER_OF_WAVETABLES 4

#define NUMBER_OF_VOICES 6

#define CONTROL_RATE 64 // powers of 2 please
/*
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aOsc[NUMBER_OF_VOICES]={
 (SIN2048_DATA)};
 Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aMod[NUMBER_OF_VOICES]={
 (SIN2048_DATA)};
 */
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aOsc[NUMBER_OF_VOICES]={
  (SIN2048_DATA),(SIN2048_DATA),(SIN2048_DATA),(SIN2048_DATA),(SIN2048_DATA),(SIN2048_DATA)};


envelope ADSR[NUMBER_OF_VOICES];
lfo LFO;


trinityHW hw; // MOZZI or DEFAULT
sequencer seq(61); // set timeBase

void setup() {
  
  
  // debug();
  hw.setFreezeType(UNFREEZE_EXTERNALY);
  hw.initialize(DEFAULT);
  //Serial.begin(9600);
  //clearMemmory();
  //for(int i=0;i<1024;i++) EEPROM.write(i,0);
  animation();
  initMem();
  
  // setAllTables();
  initMidi(getMidiChannel());
  startMozzi(CONTROL_RATE);
  hw.update();
  hw.update();
  LFO.reset();
  
  seq.setNumberOfSteps(0);
 // checkForPokemon();

}



void updateControl(){

  while(MIDI.read(inputChannel)) MIDI.read(inputChannel);
  hw.update();
  while(MIDI.read(inputChannel)) MIDI.read(inputChannel);
  UI();
  while(MIDI.read(inputChannel)) MIDI.read(inputChannel);


}

void loop() {
  audioHook();
}
