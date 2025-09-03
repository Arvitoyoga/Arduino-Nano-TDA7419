#define AUTO_GAIN 1
#define VOL_THR 35
#define LOW_PASS 30
#define DEF_GAIN 80
#define FHT_N 128
#define LOG_OUT 1

#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h> 
#include <TDA7419.h>
#include <FHT.h>
#include <Encoder.h>

byte posOffset[16] = {2, 3, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};

byte maxValue;
float maxValue_f;
float k = 0.1;
byte gain_sp = DEF_GAIN;
unsigned long gainTimer;
unsigned long spktr;
byte i1;

Encoder myEnc(8, 9);
long oldPos;
long newPos;

LiquidCrystal lcd(7, 6, 2, 3, 4, 5);
TDA7419 tda;

// Tombol Mute pin 10
// Tombol Select pin 11
// Tombol Setting pin 13
// Pin Encoder DT CLK dan SW pin 8,9 dan A1
// Pin Spektrum pin A0

int muteSW=10,select=11,setting=13,sw=A1;
bool once,onceA;

int inpA=1,gainA,vol,treb,mid,bass,treb_c,mid_q,bass_q,att,center,mute,lf,rf,lt,rt,sub,sub_f,mid_c,bass_c,bar,volOK,volOK1,volume,step,cursor,trebA,midA,bassA;
int MsetA=0,MsetB,Mset,Bset,Bal;
int lastB;

byte halfChar[8]   = {B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000};
byte fullChar[8]   = {B11011,B11011,B11011,B11011,B11011,B11011,B11011,B11011};

void setup() {
Wire.begin();
for (int i = 0; i < 4; i++) {
  lcd.begin(16, 2); 
  lcd.setCursor(5, 0);
  lcd.print("DIGITAL");
  lcd.setCursor(2, 1);
  lcd.print("AUDIO CONTROL");
  delay(500);
  lcd.clear();
  delay(300);
 }
  
pinMode(mute,INPUT);
pinMode(select,INPUT);
pinMode(setting,INPUT);
pinMode(muteSW,INPUT); 

lcd.createChar(1, halfChar);
lcd.createChar(2, fullChar);

lastB=digitalRead(8);


ADMUX = 0b01100000;
ADCSRA = 0b11010100;

gainTimer = millis();

volOK1= EEPROM.read(1);
treb= EEPROM.read(2);
mid= EEPROM.read(3);
bass= EEPROM.read(4);
treb_c= EEPROM.read(5);
mid_q= EEPROM.read(6);
bass_q= EEPROM.read(7);
inpA= EEPROM.read(8);
gainA= EEPROM.read(9);
att= EEPROM.read(10);
center= EEPROM.read(11);
mute= EEPROM.read(12);
lf= EEPROM.read(13);
rf= EEPROM.read(14);
lt= EEPROM.read(15);
rt= EEPROM.read(16);
sub= EEPROM.read(17);
sub_f= EEPROM.read(18);
mid_c= EEPROM.read(19);
bass_c= EEPROM.read(20);

trebA=map(treb,0,30,-15,15);
midA=map(mid,0,30,-15,15);
bassA=map(bass,0,30,-15,15);

audio();

myEnc.write(0);
}


void loop() {
 newPos=myEnc.read()/4;

 if(digitalRead(muteSW)==1) {mute++;if(mute>=2){mute=0;}delay(200);lcd.clear();}
 
    int B=digitalRead(8);
    if (B != lastB) {
    spktr = millis();
    lastB = B;
    }

 if(mute==1) {
  if(millis()-spktr>=10000) {
     if(once==1) {EEPROMupd();}
     ShowSpec();
     MsetA=0;
     MsetB=0;
     Mset=0;
     once=0;
     onceA=0;
     if(digitalRead(select)==1||digitalRead(setting)==1||digitalRead(sw)==0) {lcd.clear();spktr=millis();}
  }
  else {

  if(once==0) {
   lcd.createChar(1, halfChar);
   lcd.createChar(2, fullChar);
   lcd.clear();
   once=1;
   }

   if(digitalRead(setting)==1) {Mset++;if(Mset>=2){Mset=0;}delay(200);lcd.clear();onceA=0;spktr=millis();}

   if(Mset==0){setA();MsetB=0;}
   else {setB();MsetA=0;}
  }
   tda.setSoft(mute, 0, 0, 0, 0);
 } 

 else{
  tda.setSoft(mute, 0, 0, 0, 0);
  lcd.setCursor(6, 0);
  lcd.print("MUTE ");
  spktr = millis();
  onceA=0;
 }
}

void setA() {
 if(digitalRead(setting)==1) {lcd.clear();spktr=millis();}
 if(digitalRead(sw)==0) {MsetA++;if(MsetA>=4){MsetA=0;}delay(200);lcd.clear();onceA=0;spktr=millis();}
 switch(MsetA) {

 case 0:
  if(newPos!=oldPos) {volOK1=newPos-oldPos+volOK1;oldPos=newPos;}
  volOK1 = constrain(volOK1, 0, 100);
  lcd.setCursor(0, 0);
  lcd.print("Volume:");
  lcd.print(volOK1);
  lcd.print(" %  ");
  volOK=map(volOK1,0,100,0,40);
  if(volOK==0) {vol=0;}
  else {vol=40+volOK;}
  tda.setVolume(vol-80, 0);

  bar=map(volOK1,0,100,0,32);
  cursor=map(bar,0,31,0,15);
  if(onceA==0) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
  int step = bar - (i * 2);
  if (step >= 2) {
    lcd.write(byte(2)); 
  } else if (step == 1) {
    lcd.write(byte(1)); 
  } else {
    lcd.print(' '); 
  }
  }
  onceA=1;
  }
  lcd.setCursor(cursor, 1);
  if(bar%2==1&&bar>0) {lcd.write((byte)1);}
   else if(bar%2==0&&bar>0) {lcd.write((byte)2);}
  if(bar==0) {lcd.print(" ");}
  lcd.print(" ");
 break;

 case 1:
  if(newPos!=oldPos) {treb=newPos-oldPos+treb;oldPos=newPos;}
  treb = constrain(treb, 0, 30);
  trebA=map(treb,0,30,-15,15);
  lcd.setCursor(0, 0);
  lcd.print("Treble:");
  if(trebA>0) {lcd.print("+");}
  lcd.print(trebA);
  lcd.print(" dB  ");
  tda.setFilter_Treble(trebA, treb_c, 0);

  bar=map(trebA,-15,15,0,32);
  cursor=map(bar,0,31,0,15);
  if(onceA==0) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
  int step = bar - (i * 2);
  if (step >= 2) {
    lcd.write(byte(2)); 
  } else if (step == 1) {
    lcd.write(byte(1)); 
  } else {
    lcd.print(' '); 
  }
  }
  onceA=1;
  }
  lcd.setCursor(cursor, 1);
  if(bar%2==1&&bar>0) {lcd.write((byte)1);}
   else if(bar%2==0&&bar>0) {lcd.write((byte)2);}
  if(bar==0) {lcd.print(" ");}
  lcd.print(" ");
 break;

 case 2:
  if(newPos!=oldPos) {mid=newPos-oldPos+mid;oldPos=newPos;}
  mid = constrain(mid, 0, 30);
  midA=map(mid,0,30,-15,15);
  lcd.setCursor(0, 0);
  lcd.print("Middle:");
  if(midA>0) {lcd.print("+");}
  lcd.print(midA);
  lcd.print(" dB  ");
  tda.setFilter_Middle(midA, mid_q, 0);

  bar=map(midA,-15,15,0,32);
  cursor=map(bar,0,31,0,15);
  if(onceA==0) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
  int step = bar - (i * 2);
  if (step >= 2) {
    lcd.write(byte(2)); 
  } else if (step == 1) {
    lcd.write(byte(1)); 
  } else {
    lcd.print(' '); 
  }
  }
  onceA=1;
  }
  lcd.setCursor(cursor, 1);
  if(bar%2==1&&bar>0) {lcd.write((byte)1);}
   else if(bar%2==0&&bar>0) {lcd.write((byte)2);}
  if(bar==0) {lcd.print(" ");}
  lcd.print(" ");
 break;

 case 3:
  if(newPos!=oldPos) {bass=newPos-oldPos+bass;oldPos=newPos;}
  bass = constrain(bass, 0, 30);
  bassA=map(bass,0,30,-15,15);
  lcd.setCursor(0, 0);
  lcd.print("Bass:");
  if(bassA>0) {lcd.print("+");}
  lcd.print(bassA);
  lcd.print(" dB  ");
  tda.setFilter_Bass(bassA, bass_q, 0);

  bar=map(bassA,-15,15,0,32);
  cursor=map(bar,0,31,0,15);
  if(onceA==0) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
  int step = bar - (i * 2);
  if (step >= 2) {
    lcd.write(byte(2)); 
  } else if (step == 1) {
    lcd.write(byte(1)); 
  } else {
    lcd.print(' '); 
  }
  }
  onceA=1;
  }
  lcd.setCursor(cursor, 1);
  if(bar%2==1&&bar>0) {lcd.write((byte)1);}
   else if(bar%2==0&&bar>0) {lcd.write((byte)2);}
  if(bar==0) {lcd.print(" ");}
  lcd.print(" ");
 break;
 }
}

void setB(){
  if(digitalRead(sw)==0) {lcd.clear();spktr=millis();}
  if(digitalRead(select)==1) {MsetB++;if(MsetB==10){MsetB=0;}delay(200);lcd.clear();onceA=0;spktr=millis();}
  switch(MsetB) {

  case 0:
    if(digitalRead(sw)==0) {inpA++;if(inpA>=4){inpA=1;}delay(200);onceA=0;}
    lcd.setCursor(0, 0);
    lcd.print("Input:");
    lcd.print(inpA);
    if(newPos!=oldPos) {gainA=newPos-oldPos+gainA;oldPos=newPos;}
    gainA = constrain(gainA, 0, 15);
    lcd.setCursor(8, 0);
    lcd.print("Gain:");
    if(gainA>0) {lcd.print("+");}
    lcd.print(gainA);
    lcd.print(" ");
    tda.setInput(inpA, gainA, 0);
    
    if(onceA==0) {
    lcd.setCursor(0, 1);
    for (int i=0; i<gainA; i++) {
      lcd.write((byte)2);
    }
    onceA=1;
    }
    
    lcd.setCursor(gainA, 1);
    lcd.write((byte)2);
    lcd.print(" ");
    if(gainA==0) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
    }
     else{
      lcd.setCursor(0, 1);
      lcd.write((byte)2);
     }
  break;

  case 1:
    lcd.setCursor(1, 0);
    lcd.print("Treble Center");
    if(newPos!=oldPos) {treb_c=newPos-oldPos+treb_c;oldPos=newPos;}
    if(treb_c>3) {treb_c=0;}
    if(treb_c<0) {treb_c=3;}
    lcd.setCursor(5, 1);
    switch(treb_c) {
    case 0: lcd.print("10kHz  "); break;
    case 1: lcd.print("12.5kHz "); break;
    case 2: lcd.print("15kHz  "); break;
    case 3: lcd.print("17.5kHz  "); break;
    }
    tda.setFilter_Treble(treb, treb_c, 0);
  break;

  case 2:
    lcd.setCursor(1, 0);
    lcd.print("Middle Center");
    if(newPos!=oldPos) {mid_c=newPos-oldPos+mid_c;oldPos=newPos;}
    if(mid_c>3) {mid_c=0;}
    if(mid_c<0) {mid_c=3;}
    lcd.setCursor(5, 1);
    switch (mid_c) {
    case 0: lcd.print("500Hz "); break;
    case 1: lcd.print("1000Hz "); break;
    case 2: lcd.print("1500Hz "); break;
    case 3: lcd.print("2500Hz "); break;
    }
    tda.setSub_M_B(sub_f, mid_c, bass_c, 0, 0);
  break;

  case 3:
    lcd.setCursor(3, 0);
    lcd.print("Bass Center");
    if(newPos!=oldPos) {bass_c=newPos-oldPos+bass_c;oldPos=newPos;}
    if(bass_c>3) {bass_c=0;}
    if(bass_c<0) {bass_c=3;}
    lcd.setCursor(6, 1);
    switch (bass_c) {
    case 0: lcd.print("60Hz "); break;
    case 1: lcd.print("80Hz "); break;
    case 2: lcd.print("100Hz "); break;
    case 3: lcd.print("200Hz "); break;
    }
    tda.setSub_M_B(sub_f, mid_c, bass_c, 0, 0);
  break;

  case 4:
    lcd.setCursor(1, 0);
    lcd.print("Center/Sub Freq");
    if(newPos!=oldPos) {sub_f=newPos-oldPos+sub_f;oldPos=newPos;}
    if(sub_f>3) {sub_f=0;}
    if(sub_f<0) {sub_f=3;}
    lcd.setCursor(6, 1);
    switch (sub_f) {
    case 0: lcd.print("Flat ");break;
    case 1: lcd.print("80Hz ");break;
    case 2: lcd.print("120Hz ");break;
    case 3: lcd.print("160Hz ");break;
    }
    tda.setSub_M_B(sub_f, mid_c, bass_c, 0, 0);
  break;

  case 5:
    lcd.setCursor(2, 0);
    lcd.print("Mid Q Factor");
    if(newPos!=oldPos) {mid_q=newPos-oldPos+mid_q;oldPos=newPos;}
    if(mid_q>3) {mid_q=0;}
    if(mid_q<0) {mid_q=3;}
    lcd.setCursor(6, 1);
    switch (mid_q) {
    case 0: lcd.print("0.5 ");break;
    case 1: lcd.print("0.75 ");break;
    case 2: lcd.print("1.0 ");break;
    case 3: lcd.print("1.25 ");break;
    }
    tda.setFilter_Middle(mid, mid_q, 0);
  break;

  case 6:
    lcd.setCursor(2, 0);
    lcd.print("Bass Q Factor");
    if(newPos!=oldPos) {bass_q=newPos-oldPos+bass_q;oldPos=newPos;}
    if(bass_q>3) {bass_q=0;}
    if(bass_q<0) {bass_q=3;}
    lcd.setCursor(6, 1);
    switch (bass_q) {
    case 0: lcd.print("1.0 ");break;
    case 1: lcd.print("1.25 ");break;
    case 2: lcd.print("1.5 ");break;
    case 3: lcd.print("2.5 ");break;
    }
    tda.setFilter_Bass(bass, bass_q, 0);
  break;

  case 7:
    lcd.setCursor(4, 0);
    lcd.print("Loudness");
    if(newPos!=oldPos) {center=newPos-oldPos+center;oldPos=newPos;}
    if(center>3) {center=0;}
    if(center<0) {center=3;}
    lcd.setCursor(6, 1);
    switch (center) {
    case 0: lcd.print("Flat  ");break;
    case 1: lcd.print("400Hz  ");break;
    case 2: lcd.print("800Hz  ");break;
    case 3: lcd.print("2400Hz  ");break;
    }
    tda.setAtt_loudness(att, center, 0, 0);
  break;

  case 8:
    lcd.setCursor(0, 0);
    lcd.print("Attenuation:");
    if(newPos!=oldPos) {att=newPos-oldPos+att;oldPos=newPos;}
    att = constrain(att, 0, 15);
    if(att>0) {lcd.print("-");}
    lcd.print(att);
    lcd.print(" ");
    tda.setAtt_loudness(att, center, 0, 0);

    if(onceA==0) {
    lcd.setCursor(0, 1);
    for (int i=0; i<att; i++) {
      lcd.write((byte)2);
    }
    onceA=1;
    }
    
    lcd.setCursor(att, 1);
    lcd.write((byte)2);
    lcd.print(" ");
    if(att==0) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
    }
     else{
      lcd.setCursor(0, 1);
      lcd.write((byte)2);
     }
  break;

  case 9:  
   lcd.setCursor(5, 0);
   lcd.print("Balance");
   if(digitalRead(sw)==0) {Bal++;if(Bal>=5){Bal=0;}delay(200);}

   lcd.setCursor(0, 1);
   switch (Bal) {
   
   case 0:
   lcd.print("L Front:");
   if(newPos!=oldPos) {lf=newPos-oldPos+lf;oldPos=newPos;}
   lf = constrain(lf, 0, 15);
   if(lf>0) {lcd.print("+");}
   lcd.print(lf);
   lcd.print(" dB   ");
   tda.setAtt_LF(64+(lf), 0);
   break;

   case 1:
   lcd.print("R Front:");
   if(newPos!=oldPos) {rf=newPos-oldPos+rf;oldPos=newPos;}
   rf = constrain(rf, 0, 15);
   if(rf>0) {lcd.print("+");}
   lcd.print(rf);
   lcd.print(" dB   ");
   tda.setAtt_RF(64+(rf), 0);
   break;

   case 2:
   lcd.print("L Rear:");
   if(newPos!=oldPos) {lt=newPos-oldPos+lt;oldPos=newPos;}
   lt = constrain(lt, 0, 15);
   if(lt>0) {lcd.print("+");}
   lcd.print(lt);
   lcd.print(" dB   ");
   tda.setAtt_LT(64+(lt), 0); 
   break;

   case 3:
   lcd.print("R Rear:");
   if(newPos!=oldPos) {rt=newPos-oldPos+rt;oldPos=newPos;}
   rt = constrain(rt, 0, 15);
   if(rt>0) {lcd.print("+");}
   lcd.print(rt);
   lcd.print(" dB   ");
   tda.setAtt_RT(64+(rt), 0);
   break;

   case 4:
   lcd.print("Cen/Sub:");
   if(newPos!=oldPos) {sub=newPos-oldPos+sub;oldPos=newPos;}
   sub = constrain(sub, 0, 15);
   if(sub>0) {lcd.print("+");}
   lcd.print(sub);
   lcd.print(" dB   ");
   tda.setAtt_SUB(64+(sub), 0);
   break;
   }
   
   
  break;
  }
}

void audio() {
  
  ///////////////////setA//////////////////////
  volOK=map(volOK1,0,100,0,40);
  if(volOK==0) {vol=0;}
  else {vol=40+volOK;}
  tda.setVolume(vol-80, 0); //(0..80)
  tda.setFilter_Treble(trebA, treb_c, 0); //  (-15...15, 0...3) (-15 dB....15 dB, 10kHz 12.5kHz 15 kHz 17.5 kHz)
  tda.setFilter_Middle(midA, mid_q, 0);  // (-15...15, 0...3) (-15 dB....15 dB, 0.5 0.75 1 1.25)
  tda.setFilter_Bass(bassA, bass_q, 0);  // (-15...15, 0...3) (-15 dB....15 dB, 1.0 1.25 1.5 2.0)

  //////////////////setB////////////////////////
  tda.setInput(inpA, gainA, 0); //in=0..5,gain=0..+15dB
  tda.setAtt_loudness(att, center, 0, 0); //(0..15,0..3) (0..-15db/Flat, 400 Hz, 800 Hz, 2400 Hz)
  tda.setSoft(mute, 0, 0, 0, 0); //(0.1) (on.off)
  tda.setAtt_LF(64+(lf), 0); 
  tda.setAtt_RF(64+(rf), 0);
  tda.setAtt_LT(64+(lt), 0); 
  tda.setAtt_RT(64+(rt), 0); 
  tda.setAtt_SUB(64+(sub), 0); 
  tda.setSub_M_B(sub_f, mid_c, bass_c, 0, 0); //(0..3,0..3,0.3) 
        //  (int sub) Subwoofer Cut-off Frequency:  flat, 80Hz, 120Hz, 160Hz
        //  (int mid) Middle Center Frequency:  500Hz, 1000Hz, 1500Hz, 2500Hz
        //  (int bas) Bass Center Frequency:  60Hz, 80Hz, 100Hz, 200Hz

  tda.setInput2(0, 0, 0); //Not Used
  tda.setAtt_Mix(80, 0); //Not Used
  tda.setMix_Gain_Eff(1, 0, 0, 0, 0); 
  tda.setSpektor(0, 0, 1, 0, 1, 0, 0);
} 

void ShowSpec() {
  byte v1[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01110};
  byte v2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01110, 0b01110};
  byte v3[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01110, 0b01110, 0b01110};
  byte v4[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b01110, 0b01110, 0b01110, 0b01110};
  byte v5[8] = {0b00000, 0b00000, 0b00000, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110};
  byte v6[8] = {0b00000, 0b00000, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110};
  byte v7[8] = {0b00000, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110};
  byte v8[8] = {0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110};
  lcd.createChar(0, v1);
  lcd.createChar(1, v2);
  lcd.createChar(2, v3);
  lcd.createChar(3, v4);
  lcd.createChar(4, v5);
  lcd.createChar(5, v6);
  lcd.createChar(6, v7);
  lcd.createChar(7, v8);

  analyzeAudio();

  maxValue = 0;

  for (int pos = 0; pos < 16; pos++) {
    if (fht_log_out[posOffset[pos]] > maxValue) {
      maxValue = fht_log_out[posOffset[pos]];
    }

    int currentLevel = map(fht_log_out[posOffset[pos]], LOW_PASS, gain_sp, 0, 15);
    currentLevel = constrain(currentLevel, 0, 15);

    lcd.setCursor(pos, 0);
    if (currentLevel >= 8) {
      lcd.write((uint8_t)(currentLevel - 8));
    } else {
      lcd.print(" ");
    }

    lcd.setCursor(pos, 1);
    if (currentLevel >= 8) {
      lcd.write((uint8_t)7);
    } else {
      lcd.write((uint8_t)currentLevel);
    }
  }

  if (AUTO_GAIN) {
    maxValue_f = maxValue * k + maxValue_f * (1 - k);
    if (millis() - gainTimer > 1500) {
      if (maxValue_f > VOL_THR) {
        gain_sp = maxValue_f;
      } else {
        gain_sp = 100;
      }
      gainTimer = millis();
    }
  } else {
    gain_sp = DEF_GAIN;
  }
}

void analyzeAudio() {
  cli();

  i1 = 0;
  while (i1 < FHT_N) {
    i1++;
    do {
      ADCSRA |= (1 << ADSC);
    } while ((ADCSRA & (1 << ADIF)) == 0);
    fht_input[i1] = (ADCL | ADCH << 8);
  }
  
  fht_window();
  fht_reorder();
  fht_run();
  fht_mag_log();
  
  sei();
}

void EEPROMupd() {
  EEPROM.update(1, volOK1);
  EEPROM.update(2, treb);
  EEPROM.update(3, mid);
  EEPROM.update(4, bass);
  EEPROM.update(5, treb_c);
  EEPROM.update(6, mid_q);
  EEPROM.update(7, bass_q);
  EEPROM.update(8, inpA);
  EEPROM.update(9, gainA);
  EEPROM.update(10, att);
  EEPROM.update(11, center);
  EEPROM.update(12, mute);
  EEPROM.update(13, lf);
  EEPROM.update(14, rf);
  EEPROM.update(15, lt);
  EEPROM.update(16, rt);
  EEPROM.update(17, sub);
  EEPROM.update(18, sub_f);
  EEPROM.update(19, mid_c);
  EEPROM.update(20, bass_c);
}