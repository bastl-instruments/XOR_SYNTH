#define DOWN_SHIFT 8 // zkusit min


int updateAudio(){


  int output=0;




  // output+=aOsc[voice].next();  
  //   modulation[voice]=modDepth[voice]*aMod[voice].next();
  //  output+=((aOsc[voice].next()^xorAmt)*ADSR[voice].next())>>DOWN_SHIFT;

  switch(xorType){

  case 0:
    for(int voice=0;voice<NUMBER_OF_VOICES;voice++){
      if(ADSR[voice].active()){  
        output+=((aOsc[voice].next()^xorAmt)*ADSR[voice].next())>>DOWN_SHIFT;
      }
    }
    break;
  case 1:
    for(int voice=0;voice<NUMBER_OF_VOICES;voice++){
      if(ADSR[voice].active()){  
        output+=((aOsc[voice].next()|xorAmt)*ADSR[voice].next())>>DOWN_SHIFT;
      }
    }
    break;
  case 2:
    for(int voice=0;voice<NUMBER_OF_VOICES;voice++){
      if(ADSR[voice].active()){  
        output+=((aOsc[voice].next()&xorAmt)*ADSR[voice].next())>>DOWN_SHIFT;
      }
    }
    break;
  case 3:
    for(int voice=0;voice<NUMBER_OF_VOICES;voice++){
      if(ADSR[voice].active()){  
        output+=((aOsc[voice].next()^xorAmt)*ADSR[voice].next())>>DOWN_SHIFT;
      }
    }
    break;



  }

  // 
  //  amplitude = (255+lfoNow);//*gate;
  output=(output*amplitude)>>3;//<<5;//=(output*lfoNow)>>3;

  // output*=gate;

  return output;

}







