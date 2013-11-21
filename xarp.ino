

#define ARP_OFF 0
#define GATE_ARP_0 0
#define GATE_ARP_1 1
#define GATE_ARP_2 2
#define NOTE_ARP_UP 3
#define NOTE_ARP_DOWN 4
#define NOTE_ARP_UP_DOWN 5
#define NOTE_ARP_CAUSAL 6
#define NOTE_ARP_RANDOM 7



unsigned char orderedBuffer[BUFFER_SIZE];



void renderArp(){
  /*
  seq.update(audioTicks());
   seq.setClockDivider(1);
   if(!slave) {
   while(seq.clockReady()) seq.clockIn(), sendClock();
   } 
   */
  if(!slave){
    seq.clockIn();
    sendClock();
  }
  while(seq.stepReady()){
    seq.stepIn();
    proceedStep(seq.getCurrentStep()); 
  }

  // sendGBclock();

}

void sendClock(){
  MIDI.sendRealTime(Clock);
}

unsigned char lastVoice;
void proceedStep(unsigned char _step){


  // showTempo(_step);
  unsigned char voice,_note;
  switch(arpType){

  case GATE_ARP_0:
    seq.setNumberOfSteps(8);
    if(_step%2==1) gate=false;
    else gate=true;
    break;

  case GATE_ARP_1:

    seq.setNumberOfSteps(8);
    if(_step%2==1) gate=false;
    else gate=true;
    break;

  case GATE_ARP_2:

    seq.setNumberOfSteps(8);
    if(_step<4){
      if(_step%2==1) gate=false;
      else gate=true;
    }
    else  gate=true;
    break;
  case NOTE_ARP_UP:

    gate=true;
    if(notesInBuffer>0){

      seq.setNumberOfSteps(notesInBuffer);
      _note=orderedBuffer[_step];





      // lastVoice=voice;
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();

      lastVoice=proceedNoteOn(_note,ARP_VELOCITY);

    }
    else{
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();
    }
    break;

  case NOTE_ARP_DOWN:

    gate=true;
    if(notesInBuffer>0){
      seq.setNumberOfSteps(notesInBuffer);
      _note=orderedBuffer[notesInBuffer-_step];

      // lastVoice=voice;
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();

      lastVoice= proceedNoteOn(_note,ARP_VELOCITY);
    }
    else{
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();
    }

    break;

  case NOTE_ARP_UP_DOWN:

    gate=true;
    if(notesInBuffer>0){
      seq.setNumberOfSteps(notesInBuffer*2);
      if(_step<notesInBuffer) _note=orderedBuffer[(2*notesInBuffer)-_step];
      else _note=orderedBuffer[_step];

      // lastVoice=voice;
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();

      lastVoice=  proceedNoteOn(_note,ARP_VELOCITY);
    }
    else{
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();
    }
    break;

  case NOTE_ARP_CAUSAL:

    gate=true;

    if(notesInBuffer>0){
      seq.setNumberOfSteps(notesInBuffer);
      _note=midiBuffer[_step];

      // lastVoice=voice;
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();

      lastVoice=proceedNoteOn(_note,ARP_VELOCITY);
      ;

    }
    else{
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();
    }
    break;

  case NOTE_ARP_RANDOM:

    gate=true;

    if(notesInBuffer>0){
      seq.setNumberOfSteps(notesInBuffer);
      _note=orderedBuffer[rand(notesInBuffer)];
      //   lastVoice=voice;
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();

      lastVoice=proceedNoteOn(_note,ARP_VELOCITY);
    }
    else{
      voiceUse[lastVoice]=255;
      ADSR[lastVoice].noteOff();
    }
    break;

  }

}




void orderBuffer(){

  for(int j=0;j<BUFFER_SIZE;j++)  orderedBuffer[j]=0;

  unsigned char nextOne=255;
  for(int i=0;i<notesInBuffer;i++){
    if(midiBuffer[i] < nextOne) nextOne=midiBuffer[i];
  } 
  orderedBuffer[0]=nextOne;

  for(int j=1;j<notesInBuffer;j++){
    nextOne=255;
    for(int i=0;i<notesInBuffer;i++){
      if(midiBuffer[i]<nextOne && midiBuffer[i] > orderedBuffer[j-1] ) nextOne=midiBuffer[i];
    } 
    orderedBuffer[j]=nextOne;
  }

}






/*
void RenderArpeggio(){
 
 
 if(doArpeggio){
 if(kArpeggioTime.ready()){
 kArpeggioTime.start();
 
 boolean flag=true;
 unsigned char temp;
 
 switch(arpeggioType){
 case 0:
 arpeggioGate=!arpeggioGate;
 break;
 
 case 1:
 
 arpeggioGate=true;
 if(arpeggioBuffer[arpeggioNote]!=0){
 SetNoteOff(arpeggioBuffer[arpeggioNote]);
 };
 arpeggioNote++;
 if(arpeggioNote>=notesInArpeggio) arpeggioNote=0; 
 if(arpeggioBuffer[arpeggioNote]!=0){ 
 SetNoteOn(arpeggioBuffer[arpeggioNote],64);
 }
 
 break;
 
 case 2:
 arpeggioGate=true;
 if(arpeggioBuffer[arpeggioNote]!=0){
 SetNoteOff(arpeggioBuffer[arpeggioNote]);
 };
 arpeggioNote++;
 if(arpeggioNote>=notesInArpeggio) arpeggioNote=0; 
 if(arpeggioBuffer[arpeggioNote]!=0){ 
 SetNoteOn(arpeggioBuffer[arpeggioNote],64);
 }
 break;
 case 3:
 arpeggioGate=true;
 if(arpeggioBuffer[arpeggioNote]!=0){
 SetNoteOff(arpeggioBuffer[arpeggioNote]);
 }
 if(notesInArpeggio!=0){
 if(arpeggioNote==0) arpeggioNote=notesInArpeggio-1;
 else arpeggioNote--;
 }
 if(arpeggioBuffer[arpeggioNote]!=0){ 
 SetNoteOn(arpeggioBuffer[arpeggioNote],64);
 }
 break;
 case 4:
 arpeggioGate=true;
 if(arpeggioBuffer[arpeggioNote]!=0){
 SetNoteOff(arpeggioBuffer[arpeggioNote]);
 }
 if(arpUp){
 arpeggioNote++;
 if(arpeggioNote>=notesInArpeggio) arpUp=false,arpeggioNote--; 
 
 }
 if(!arpUp){
 if(notesInArpeggio!=0){
 if(arpeggioNote==0) arpUp=true,  arpeggioNote++;
 else arpeggioNote--;
 }
 
 }
 if(arpeggioBuffer[arpeggioNote]!=0){ 
 SetNoteOn(arpeggioBuffer[arpeggioNote],64);
 }
 
 break;
 
 case 5:
 
 arpeggioGate=true;
 if(arpeggioBuffer[arpeggioNote]!=0){
 SetNoteOff(arpeggioBuffer[arpeggioNote]);
 };
 arpeggioNote=rand(notesInArpeggio);
 if(arpeggioBuffer[arpeggioNote]!=0){ 
 SetNoteOn(arpeggioBuffer[arpeggioNote],64);
 }
 break;
 
 }
 
 } 
 
 }
 
 }
 */




























