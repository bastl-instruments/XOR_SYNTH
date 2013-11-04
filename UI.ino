

boolean pokemon=false;
unsigned char page;
unsigned char sound,lastSound;
boolean midiNote;
#define NUMBER_OF_PAGES 3
#define KNOB_MODE SMALL_BUTTON_2
#define DEFAULT_VELOCITY 127

boolean combo;
int var[NUMBER_OF_VARIABLES];
unsigned char currentSound[NUMBER_OF_VOICES];
long lastTicks;
unsigned char xorAmt;
unsigned char xorType;
unsigned char lfoAmt;
boolean arp;
unsigned char arpType;
void UI(){

  //if(pokemon) pokemonMode();

  //else{

  hw.setColor(page+1);
  renderSmallButtons();
  renderCombo();
  renderBigButtons();
  renderKnobs();
  renderEnvelope();
  renderTweaking(sound,page);
  renderLfo();

  renderArp();

  // }

}

void setAllValues(unsigned char _SOUND){


  // for(int i=0;i<NUMBER_OF_VARIABLES;i++) var[i]=getVar(_SOUND,i);


  for(int i=0;i<NUMBER_OF_VOICES;i++) aOsc[i].setTable(WAVE_TABLES[getBits(getVar(_SOUND,OSC_CHAR),2,2)]); 
  xorAmt=getVar(_SOUND,XOR_AMT)<<1;
  xorType=getBits(getVar(_SOUND,OSC_CHAR),0,2);

  seq.setTempo(40+getVar(_SOUND,ARP_RATE)<<1);

  LFO.setAll(getVar(_SOUND,LFO_RATE)<<3,0,getVar(_SOUND,LFO_RES)); //getVar(_SOUND,LFO_SHAPE)
  lfoAmt=getVar(_SOUND,LFO_AMT)<<1;
  //LFO.reset();

  for(int i=0;i<NUMBER_OF_VOICES;i++) {
    ADSR[i].setAttack(scale(getVar(_SOUND,ATACK),variableDepth[ATACK],ADSR_BITS)) ;
    ADSR[i].setDecay(scale(getVar(_SOUND,DECAY),variableDepth[DECAY],ADSR_BITS)) ;
    ADSR[i].setSustain(scale(getVar(_SOUND,SUSTAIN),variableDepth[SUSTAIN],SUSTAIN_BITS));
    ADSR[i].setRelease(scale(getVar(_SOUND,RELEASE),variableDepth[RELEASE],ADSR_BITS));
  }

}


void playSound(unsigned char _SOUND, unsigned char _VELOCITY){

  unsigned char voice=_SOUND%3;
  currentSound[voice]=_SOUND;
  setAllValues(_SOUND);
  midiNote=false;
  //ADSR[voice].noteOff();
  // ADSR[voice].noteOn(_VELOCITY);


}


float freq[NUMBER_OF_VOICES];

void playSound(unsigned char _SOUND, unsigned char voice, unsigned char _PITCH, unsigned char _VELOCITY){

  //  unsigned char voice=_SOUND%3;
  // currentSound[voice]=_SOUND;


  // setAllValues(_SOUND); // nemusi byt

  freq[voice]=float(mtof(_PITCH));
  aOsc[voice].setFreq(freq[voice]);

  midiNote=true;
  ADSR[voice].noteOff();
  ADSR[voice].noteOn(_VELOCITY);

}


void renderEnvelope(){

  for(int voice=0;voice<NUMBER_OF_VOICES;voice++){
    ADSR[voice].update();
  }

}



void renderTweaking(unsigned char _SOUND, unsigned char _page){


  //tweaking

  switch(_page){

  case OSC_PAGE:

    if(!hw.knobFreezed(RIGHT_KNOB) && hw.knobMoved(RIGHT_KNOB)){
      xorType=getBits(getVar(_SOUND,OSC_CHAR),0,2);
      for(int i=0;i<NUMBER_OF_VOICES;i++) aOsc[i].setTable(WAVE_TABLES[getBits(getVar(_SOUND,OSC_CHAR),2,2)]); 
    }
    if(!hw.knobFreezed(LEFT_KNOB)&& hw.knobMoved(LEFT_KNOB)) xorAmt=getVar(_SOUND,XOR_AMT);

    if(!hw.knobFreezed(TOP_KNOB)&& hw.knobMoved(TOP_KNOB)) seq.setTempo(40+getVar(_SOUND,ARP_RATE)<<1);

    break;

  case LFO_PAGE:
    LFO.setAll(getVar(_SOUND,LFO_RATE)<<3,0,getVar(_SOUND,LFO_RES));
    if(!hw.knobFreezed(LEFT_KNOB) && hw.knobMoved(LEFT_KNOB)) lfoAmt=getVar(_SOUND,LFO_AMT)<<1;
    break;

  case ADSR_PAGE:
    for(int i=0;i<NUMBER_OF_VOICES;i++) {
      ADSR[i].setAttack(scale(getVar(_SOUND,ATACK),variableDepth[ATACK],ADSR_BITS)) ;
      ADSR[i].setDecay(scale(getVar(_SOUND,DECAY),variableDepth[DECAY],ADSR_BITS)) ;
      ADSR[i].setSustain(scale(getVar(_SOUND,SUSTAIN),variableDepth[SUSTAIN],SUSTAIN_BITS));
      ADSR[i].setRelease(scale(getVar(_SOUND,RELEASE),variableDepth[RELEASE],ADSR_BITS));
    }
    break;
  }


  //tweaking end 

}
int lfoNow;

void renderLfo(){
  LFO.update();
  lfoNow=((lfoAmt*LFO.next())>>8)-127;
}

/*
boolean isDestination(unsigned char voice,unsigned char _destination){
 return bitRead(destination[voice],_destination); 
 }
 */

void renderSmallButtons(){
  // knobMode
  if(hw.justReleased(KNOB_MODE) && !combo){ 
    if(page<NUMBER_OF_PAGES-1) page++;
    else page=0;
    hw.freezeAllKnobs();
  } 
  if(page==OSC_PAGE){
    if(hw.justReleased(SMALL_BUTTON_1) && !combo){
      arp=!arp;
      if(arp) seq.play();
      else seq.stop(), sendAllNoteOff(); 
    }
  }
  else{
    if(hw.justReleased(SMALL_BUTTON_1) && !combo) setVar(sound,page*VARIABLES_PER_PAGE+3,increaseValue(getVar(sound,page*VARIABLES_PER_PAGE+3),maxValue[page*VARIABLES_PER_PAGE+3]));
  }
}

void renderCombo(){

  //loading saving presets
  // if(hw.buttonState(SMALL_BUTTON_2) && hw.justPressed(SMALL_BUTTON_1)) loadPreset(0), hw.freezeAllKnobs(), combo=true;
  for(int i=0;i<3;i++){
    //  if(hw.buttonState(EXTRA_BUTTON_2) && hw.justPressed(i)) randomize(i+3*shift); // randomize 
    if(hw.buttonState(SMALL_BUTTON_1) && hw.buttonState(SMALL_BUTTON_2) && hw.justPressed(i)) storePreset(currentPreset), loadPreset(i), hw.freezeAllKnobs(), combo=true; // save&load
    if(hw.buttonState(SMALL_BUTTON_2) && hw.justPressed(i)) loadPreset(i), hw.freezeAllKnobs(), combo=true; // load

    if(hw.buttonState(SMALL_BUTTON_1) && hw.justPressed(SMALL_BUTTON_2)) arpType=increaseValue(arpType,7),combo=true;
  }

  if(combo){
    //turn off combo when all buttons are released 
    unsigned char _count=0; 
    for(int i=0;i<NUMBER_OF_BUTTONS;i++)  _count+=hw.buttonState(i); // if(!hw.buttonState(i)) combo=false;
    if(_count==0) combo=false;
    //else combo=true;
  } 


}

void randomize(unsigned char _sound){
  for(int i=0;i<NUMBER_OF_VARIABLES;i++) setVar(_sound,i,rand(maxValue[i])), hw.freezeAllKnobs();
}


void renderBigButtons(){

  if(!combo){
    for(int i=BIG_BUTTON_1;i<=BIG_BUTTON_3;i++) {

      if(hw.justPressed(i)){
        hw.flipSwitch(i), sound=hw.soundFromSwitches();
        // playSound(sound,DEFAULT_VELOCITY);
        hw.setLed(i,hw.switchState(i));
      }
    };
    if(sound!=lastSound)  setAllValues(sound), hw.freezeAllKnobs();
    lastSound=sound;
  } 

}

void renderKnobs(){

  for(int i=0;i<NUMBER_OF_KNOBS;i++){

    unsigned char _variable=i+VARIABLES_PER_PAGE*page;


    if(hw.knobFreezed(i)) {
      if(lastSound==sound){ // knobs are unfreezed when they come across their original value
        if(inBetween( scale(hw.knobValue(i),KNOB_BITS,variableDepth[_variable]), scale(hw.lastKnobValue(i),KNOB_BITS,variableDepth[_variable]),getVar(sound,_variable) ) ){
          hw.unfreezeKnob(i); //external unfreez
          hw.setColor(WHITE);
        }
      }  
    }

    else if(hw.knobMoved(i)) setVar(sound,_variable,scale(hw.knobValue(i),KNOB_BITS,variableDepth[_variable])); 

  }
}

int increaseValue(int _VALUE, int _OVERFLOW){
  if(_VALUE < _OVERFLOW) _VALUE++;
  else _VALUE=0;
  return _VALUE;
}

int decreaseValue(int _VALUE, int _OVERFLOW){
  if(_VALUE > _OVERFLOW) _VALUE--;
  return _VALUE;
}



int increaseValue(int _VALUE, int _OVERFLOW, int _INCREMENT){
  if(_VALUE < _OVERFLOW) _VALUE+=_INCREMENT;
  return _VALUE;
}
int decreaseValue(int _VALUE, int _OVERFLOW, int _INCREMENT){
  if(_VALUE > _OVERFLOW) _VALUE-=_INCREMENT;
  return _VALUE;
}


unsigned char getBits(unsigned char _val, unsigned char _offset, unsigned char _bits){
  unsigned char returnVal=0;
  for(int i=0;i<_bits;i++){
    bitWrite(returnVal,i,bitRead(_val,_offset+i));
  }
  return  returnVal;
}

void animation(){

  hw.setLed(LED_1,true);
  hw.update();
  delay(150);
  hw.setLed(LED_3,true);
  hw.update();
  delay(150);
  hw.setLed(LED_2,true);
  hw.update();
  delay(150);
  hw.setColor(BLUE);
  hw.update();
  delay(150);
  hw.setColor(WHITE);
  hw.update();
  delay(150);
  hw.setColor(BLACK);
  hw.setLed(LED_1,false);
  hw.setLed(LED_2,false);
  hw.setLed(LED_3,false);
  hw.update();

}



/*

 
 void pokemonMode(){
 
 if(audioTicks()-lastTicks>4000) lastTicks=audioTicks(), page=increaseValue(page,7);
 hw.setColor(page);
 shift=hw.buttonState(SMALL_BUTTON_1);
 if(hw.buttonState(SMALL_BUTTON_1) && hw.justPressed(SMALL_BUTTON_2)) makeSysExArray(sound), sendSysExArray();
 for(int i=0;i<3;i++) if(hw.buttonState(SMALL_BUTTON_2) && hw.justPressed(i)) loadPreset(i);
 
 renderBigButtons();
 renderEnvelope();
 renderLfo(); 
 }
 
 
 
 
 
 
 void checkForPokemon(){
 if(hw.buttonState(SMALL_BUTTON_1) && hw.buttonState(SMALL_BUTTON_2)) pokemon=true; 
 
 }
 
 
 
 #define NUMBER_OF_MESSAGE_BYTES 16
 #define NUMBER_OF_SYSEX_BYTES 14
 unsigned char sysExArray[NUMBER_OF_SYSEX_BYTES];
 
 #define BASTL_BYTE 0x7D
 #define CHANNEL_BYTE 0x00
 #define INSTRUMENT_BYTE 0X01 // poly 1.0
 
 void makeSysExArray(unsigned char _sound){
 unsigned char _bit=0;
 unsigned char _byte=0;
 sysExArray[0]=BASTL_BYTE;
 sysExArray[1]=INSTRUMENT_BYTE;
 for(int i=0;i<NUMBER_OF_VARIABLES;i++){
 int readFrom=getVar(_sound,i);
 for(int j=0;j<variableDepth[i];j++){
 _bit++;
 if(_bit>=7) _bit=0,_byte++;
 bitWrite(sysExArray[_byte+2],_bit, bitRead(readFrom,j));
 }
 } 
 }
 
 void sendSysExArray(){
 MIDI.sendSysEx(NUMBER_OF_MESSAGE_BYTES,sysExArray,false);
 }
 
 
 
 
 void HandleSystemExclusive(byte *array, byte size){
 
 if(array[1]==BASTL_BYTE){ 
 
 if(array[2]==CHANNEL_BYTE){
 inputChannel=array[2]; 
 array[2]++;
 MIDI.sendSysEx(size,array);
 }
 else if(array[2]==INSTRUMENT_BYTE){
 hw.setColor(WHITE);
 hw.update();
 for(int i=0;i<NUMBER_OF_SYSEX_BYTES;i++) sysExArray[i]=array[i+1];
 extractSysExArray(sound);
 hw.freezeAllKnobs();
 }
 }
 }
 
 
 
 
 
 
 
 void extractSysExArray(unsigned char _sound){
 unsigned char _bit=0;
 unsigned char _byte=0;
 for(int i=0;i<NUMBER_OF_VARIABLES;i++){
 int writeTo=0;
 for(int j=0;j<variableDepth[i];j++){
 _bit++;
 if(_bit>=7) _bit=0,_byte++;
 bitWrite(writeTo,j,bitRead(sysExArray[_byte+2],_bit));
 }
 setVar(_sound,i,writeTo);
 } 
 }
 */











