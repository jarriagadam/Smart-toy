////////////////////////////////// MODULO BLUETOOTH
#include <BPLib.h>
#include <SoftwareSerial.h>
////////////////////////////////// MODULO BLUETOOTH


#include <Adafruit_NeoPixel.h> // NEOPIXEL RGB
#include "SerialMP3Player.h" //MP3 SERIAL
#include <Encoder.h> //Encoder Rotatorio


////////////////////////////////////// RFID NFC
#include <PN532_HSU.h> // RFID PN532
#include <PN532.h> // RFID PN532
#include <NfcAdapter.h> //RFID PN532

PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc = NfcAdapter(pn532hsu);
///////////////////////////////////// RFID NFC


//////////////////////////////////////
#define TX 16 //MP3 SERIAL PLAYER
#define RX 17 //MP3 Serial Player
//////////////////////////////////////


//////////////////////////////////////////////// MODULO BLUETOOTH
#define RX_PIN 14 // Conectar a TX del modulo
#define TX_PIN 15 // Conectar a RX del modulo
//////////////////////////////////////////////// MODULO BLUETOOTH


//////////////////////////////////////////////// BOTONES e INTERRUPTORES
#define Boton_llave   9 //Boton de LLave
#define Boton_rojo   60 //Boton Rojo
#define Boton_azul   63 //Boton Azul
#define Boton_verde   62 //Boton Verde
#define Boton_amarillo   61 //Boton Amarillo
#define Switch_rojo   2 //Switch Rojo
#define Switch_naranjo   3 //Switch Naranjo
#define Switch_azul   5 //Switch Azul
#define Switch_verde   4 //Switch Verde
#define Boton_joystick   11 //Boton Joystick
#define Boton_rotary   8 //Boton Rotary Encoder

#define PIXEL_PIN    10 //Neopixel RGB Pin

#define PIXEL_COUNT 16 //Numero de leds Neopixel
//////////////////////////////////////////////// BOTONES e INTERRUPTORES


/////////////////////// Strings RFID TAGS
String RFID1 = "9A 1E 25 28";
String RFID2 = "3F 90 1E 29";
String RFID3 = "6A E7 23 D9";
String RFID4 = "24 FD 15 41";
String RFID5 = "F2 75 73 85"; //Tarjeta Bip
String RFID6 = "F7 8A D5 A6";
String RFID7 = "4B AA 4F F5"; //BIP Barbi
String RFID8 = "16 1A 22 3B"; //Caminando
String RFID9 = "E6 11 22 3B"; //Scooter
/////////////////////// Strings RFID TAGS


/////////////////////BLUEBEE BLUETOOTH HID
SoftwareSerial bluetoothSerial(RX_PIN, TX_PIN); //RX , TX
BPLib *BPMod;
////////////////////BLUEBEE BLUETOOTH HID


////////////////////////////////////////////////////////////////////// ROTARY ENCODER RGB
const int BRIGHTNESS = 255;        // brillo de todos los leds
const int WHEEL_SIZE = 256;        // cuantas entradas en la rueda de color
const boolean MOVE_LIGHT = false;  //Mueve una luz alrededor o todas las luces
const int ENCODER_PIN_1 = 6; //Pin 1 Encoder
const int ENCODER_PIN_2 = 7; //Pin 2 Encoder
const int ENCODER_BUTTON = 8; //Pin boton encoder
////////////////////////////////////////////////////////////////////// ROTARY ENCODER RGB


Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800); //Iniciar Adafruit Neopixel RGB
SerialMP3Player mp3(RX,TX); //Reproductor MP3 Serial
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2); //Encoder PINS

bool oldState = HIGH;
bool oldStateSwitch = LOW;
int showType = 0;

int mode = 0;
long lastPush = 0;
int autoPosition = 0;

//INICIAR EN NEGRO
void initializeToBlack() {
  for (int i =0; i < PIXEL_COUNT; i++) {
   strip.setPixelColor(i, 0);
  }  
}

void setup() { //////////////////////////////////////////////// VOID SETUP ////////////////////////////////////////////////////
  Serial.begin(9600);     // iniciar interfaz serial

  
///////////////////////////////////////
  Serial.println("NDEF Reader"); //RFID
///////////////////////////////////////


//////////////////////////////////////////// MODULO BLUETOOTH REPARAR O CAMBIAR CODIGO
/*
 //BLUETOOTH BLUEBEE
  bluetoothSerial.begin(115200);
  BPMod = new BPLib(bluetoothSerial);  
  BPMod->begin(BP_MODE_HID,BP_HID_KEYBOARD);  
 //BLUETOOTH BLUEBEE
*/
//////////////////////////////////////////// MODULO BLUETOOTH REPARAR O CAMBIAR CODIGO


 /////////////////////////////////////// ENCODER
 pinMode(ENCODER_BUTTON, INPUT);
 digitalWrite(ENCODER_BUTTON, HIGH); //Encender resistencia pull up.
 /////////////////////////////////////// ENCODER


 /////////////////////////////////////// MP3 SERIAL
 mp3.showDebug(1);       // Imprimir lo que se envía al modulo mp3
 mp3.begin(9600);        // Empezar la comunicación mp3
 delay(500);             // Esperar para iniciar
 mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //elegir sd-card
 delay(500);             // Esperar para iniciar
 menu('?',0); // Imprimir las opciones de menu.
 /////////////////////////////////////// MP3 SERIAL 


/////////////////////////////////////////
 nfc.begin();            // RFID BEGIN
 delay(500);
/////////////////////////////////////////


 //////////////////////////////////////////////////// INPUTS
  pinMode(Boton_llave, INPUT_PULLUP);
  pinMode(Boton_rojo, INPUT_PULLUP);
  pinMode(Boton_azul, INPUT_PULLUP);
  pinMode(Boton_verde, INPUT_PULLUP);  
  pinMode(Boton_amarillo, INPUT_PULLUP);
  pinMode(Switch_rojo, INPUT_PULLUP); 
  pinMode(Switch_naranjo, INPUT_PULLUP);
  pinMode(Switch_azul, INPUT_PULLUP);
  pinMode(Switch_verde, INPUT_PULLUP);
  pinMode(Boton_joystick, INPUT_PULLUP);
  pinMode(Boton_rotary, INPUT_PULLUP);
 //////////////////////////////////////////////////// INPUTS

 
  strip.begin();
  initializeToBlack();
  strip.show(); // Iniciar todos los pixeles en 'off'
}

 char c;  // char desde Serial 
 char cmd=' ';
 char cmd1=' ';


// La función loop corre una y otra vez infinitamente

long normalize(long value, long radix) {
  long rval = value % radix;
  if (rval < 0) return radix + rval;
  else return rval; 
}


void loop() { ////////////////////////////////////////////////// VOID LOOP///////////////////////////////////////////////////////////


///////////////////////////////////////////////////// BOTON ROTARY LED ENCODER
  int button= digitalRead(ENCODER_BUTTON);
  if (button == 0) {
    if ((millis() - lastPush) > 250) {
      lastPush = millis();
      mode++;
      if (mode > 4) mode = 0;
      Serial.println("Boton_encoder");
    }
  }
  long knobValue = encoder.read() / 2;

  long ledPosition = normalize(knobValue, PIXEL_COUNT);
  long colorValue = normalize(knobValue * 5, WHEEL_SIZE);
  long sleepValue = abs(knobValue) % 500;
  
  switch(mode) {
  // off
  case 0: initializeToBlack();
          break;
  // knob moves led
  case 1: initializeToBlack();
          strip.setPixelColor(ledPosition, colorWheel(BRIGHTNESS, colorValue));
          break;
  // Todos encendidos, la perilla crea un arcoiris (rainbow)
  case 2: for (int i =0; i < PIXEL_COUNT; i++) {
            strip.setPixelColor(i, colorWheel(BRIGHTNESS, colorValue));
          }
          break;
  // Una luz que se mueve sola, la perilla ajusta la velocidad
  case 3: 
          delay(sleepValue);
          initializeToBlack();    
          strip.setPixelColor(autoPosition, colorWheel(BRIGHTNESS, autoPosition * (WHEEL_SIZE /PIXEL_COUNT)));
          autoPosition++;
          if (autoPosition >= PIXEL_COUNT) {
            autoPosition = 0;        
          }
          break;
  // Luz parpadeante se mueve por el aro de color, la perilla ajusta la velocidad
  case 4:
          const int SLUG_SIZE = 15;
          
          delay(sleepValue);
          initializeToBlack();
          
          for (int i = 0; i < SLUG_SIZE; i++) {
            strip.setPixelColor((i + autoPosition) % PIXEL_COUNT, colorWheel(BRIGHTNESS, i * (WHEEL_SIZE / SLUG_SIZE)));
          }
          
          autoPosition++;
          if (autoPosition >= PIXEL_COUNT) {
            autoPosition = 0;         
          }
          break;
  }
  strip.show();

///////////////////////////////////////////////////// BOTON ROTARY LED ENCODER


////////////////////////////////////////////////// MP3 SERIAL
  if (Serial.available()){
    c = Serial.read();
    decode_c(); // Decode c.
  }
  // Buscar la respuesta
  if (mp3.available()){
    Serial.println(mp3.decodeMP3Answer()); // Imprimir respuesta decodificada desde mp3
  }
////////////////////////////////////////////////// MP3 SERIAL


////////////////////////////////////////////////////////////// BOTON ROJO
  // ESTADO ACTUAL DEL BOTON
  bool newState = digitalRead(Boton_rojo);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    //delay(2);
    // Check if button is still low after debounce.
    newState = digitalRead(Boton_rojo);
    if (newState == LOW) {
      showType++;
      if (showType > 9)
        showType=0;
      startShowRojo(showType);
      Serial.println("Boton_rojo");
      //BPMod->sendString("r");
    }
  }
  oldState = newState;
////////////////////////////////////////////////////////////// BOTON ROJO

  
////////////////////////////////////////////////////////////// BOTON VERDE
 bool newStateVerde = digitalRead(Boton_verde);

 if (newStateVerde == LOW && oldState == HIGH) {
  //delay(2);
  newStateVerde = digitalRead(Boton_verde);
  if (newStateVerde == LOW) {
    showType++;
    if (showType > 9)
       showType=0;
    startShow(showType);
    //BPMod->sendString("g");
    Serial.println("Boton_verde");
    }
  }
  oldState = newStateVerde;
////////////////////////////////////////////////////////////// BOTON VERDE

  
////////////////////////////////////////////////////////////// BOTON AMARILLO
 bool newStateAmarillo = digitalRead(Boton_amarillo);

 if (newStateAmarillo == LOW && oldState == HIGH) {
  //delay(2);
  newStateAmarillo = digitalRead(Boton_amarillo);
  if (newStateAmarillo == LOW) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowAmarillo(showType);
 Serial.println("Boton_amarillo");
    }
  }
  oldState = newStateAmarillo;
////////////////////////////////////////////////////////////// BOTON AMARILLO

  
////////////////////////////////////////////////////////////// BOTON AZUL
 bool newStateAzul = digitalRead(Boton_azul);

 if (newStateAzul == LOW && oldState == HIGH) {
  //delay(2);
  newStateAzul = digitalRead(Boton_azul);
  if (newStateAzul == LOW) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowAzul(showType);
   Serial.println("Boton_azul");
    }
  }
  oldState = newStateAzul;
////////////////////////////////////////////////////////////// BOTON AZUL

  
////////////////////////////////////////////////////////////// BOTON LLAVE
  bool newStateLlave = digitalRead(Boton_llave);

 if (newStateLlave == LOW && oldState == HIGH) {
  //delay(2);
  newStateLlave = digitalRead(Boton_llave);
  if (newStateLlave == LOW) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowLlave(showType);
    Serial.println("Boton_Llave");   
    }
  }
  oldState = newStateLlave;
////////////////////////////////////////////////////////////// BOTON LLAVE


////////////////////////////////////////////////////////////// SWITCH ROJO
  bool newStateSwRojo = digitalRead(Switch_rojo);

 if (newStateSwRojo == HIGH && oldStateSwitch == LOW) {
  //delay(2);
  newStateSwRojo = digitalRead(Switch_rojo);
  if (newStateSwRojo == HIGH) {
    showType++;
    if (showType > 9)
       showType=1;
    startShowSwRojo(showType);
     Serial.println("Switch_Rojo");
    }
  }
  oldStateSwitch = newStateSwRojo;
////////////////////////////////////////////////////////////// SWITCH ROJO

  
////////////////////////////////////////////////////////////// SWITCH VERDE
  bool newStateSwVerde = digitalRead(Switch_verde);

 if (newStateSwVerde == HIGH && oldStateSwitch == LOW) {
  //delay(2);
  newStateSwVerde = digitalRead(Switch_verde);
  if (newStateSwVerde == HIGH) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowSwVerde(showType);
    Serial.println("Switch_Verde");
    }
  }
  oldStateSwitch = newStateSwVerde;
////////////////////////////////////////////////////////////// SWITCH VERDE


////////////////////////////////////////////////////////////// SWITCH AZUL
  bool newStateSwAzul = digitalRead(Switch_azul);

 if (newStateSwAzul == HIGH && oldStateSwitch == LOW) {
  //delay(2);
  newStateSwAzul = digitalRead(Switch_azul);
  if (newStateSwAzul == HIGH) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowSwAzul(showType);
    Serial.println("Switch_Azul");   
    }
  }
  oldStateSwitch = newStateSwAzul;
////////////////////////////////////////////////////////////// SWITCH AZUL

  
////////////////////////////////////////////////////////////// SWITCH NARANJO
  bool newStateSwNaranjo = digitalRead(Switch_naranjo);

 if (newStateSwNaranjo == HIGH && oldStateSwitch == LOW) {
  //delay(2);
  newStateSwNaranjo = digitalRead(Switch_naranjo);
  if (newStateSwNaranjo == HIGH) {
    showType++;
    if (showType > 9)
       showType=0;
    startShowSwNaranjo(showType);
    Serial.println("Switch_Naranjo");
    }
  }
  oldStateSwitch = newStateSwNaranjo;
////////////////////////////////////////////////////////////// SWITCH NARANJO

 
////////////////////////////////////////////////////////////// BOTON JOYSTICK
  bool newStateJoystick = digitalRead(Boton_joystick);

 if (newStateJoystick == LOW && oldState == HIGH) {
  //delay(2);
  newStateJoystick = digitalRead(Boton_joystick);
  if (newStateJoystick == LOW) {
    showType++;
    if (showType > 9)
       showType=10;
    startShowJoystick(showType);
    Serial.println("Boton_Joystick");
    }
  }
  oldState = newStateJoystick;
////////////////////////////////////////////////////////////// BOTON JOYSTICK

  
////////////////////////////////////////////////////////////// BOTON ROTARY ENCODER SIMPLE
/*  bool newStateRotary = digitalRead(Boton_rotary);

 if (newStateRotary == LOW && oldState == HIGH) {
  //delay(2);
  newStateRotary = digitalRead(Boton_rotary);           // DESACTIVADA: ROTARY LED REEMPLAZA
  if (newStateRotary == LOW) {
    showType++;
    if (showType > 9)
       showType=10;
    startShowRotary(showType);
    Serial.println("Boton_Rotary");    
    }
  }
  oldState = newStateRotary; */
////////////////////////////////////////////////////////////// BOTON ROTARY ENCODER SIMPLE

/*
//////////////////////////////////////////////////////////////// RFID SCAN TAGS
   Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        String uid = tag.getUidString();
        String RFID_value;
        if (uid == RFID1) {
          RFID_value = "1";
          Serial.write("Tag1");
          Serial.println("Tag1");
          colorWipe(strip.Color(255, 0, 0), 50);
          mp3.play(1);

          
        } else if (uid == RFID2) {
          RFID_value = "2";
          Serial.write("Tag2");
          Serial.println("Tag2");
          mp3.play(2);
          colorWipe(strip.Color(255, 100, 0), 50);
          
        } else if (uid == RFID3) {
          RFID_value = "3";
          Serial.write("Tag3");
          Serial.println("Tag3");
          colorWipe(strip.Color(0, 255, 120), 50);
          mp3.play(3);

          
        } else if (uid == RFID4) {
          RFID_value = "4";
          Serial.write("Tag4");
          Serial.println("Tag4");
          mp3.play(4);
          colorWipe(strip.Color(0, 0, 255), 50);
          
        } else if (uid == RFID5) {
          RFID_value = "5"; //Tarjeta BIP
          Serial.write("Tag5");
          Serial.println("Tag5");
          colorWipe(strip.Color(0, 255, 255), 50);
          mp3.play(5);

          
        } else if (uid == RFID6) {
          RFID_value = "6";
          Serial.write("Tag6");
          Serial.println("Tag6");
          colorWipe(strip.Color(255, 0, 0), 50);
          mp3.play(9);


        } else if (uid == RFID7) {
          RFID_value = "7";
          Serial.write("Tag7");
          Serial.println("Tag7");
          colorWipe(strip.Color(0, 255, 0), 50);  // Green
          mp3.play(7);
        //BPMod->sendString("7"); ENVIAR TECLA MODULO BLUETOOTH (NO FUNCIONA)


        } else if (uid == RFID8) {
          RFID_value = "8";
          Serial.write("Tag8");
          Serial.println("Tag8");
          colorWipe(strip.Color(0, 255, 0), 50);
          mp3.play(8);
        //BPMod->sendString("8"); ENVIAR TECLA MODULO BLUETOOTH (NO FUNCIONA)
 

         } else if (uid == RFID9) {
          RFID_value = "9";
          Serial.write("Tag9");
          Serial.println("Tag9");
          colorWipe(strip.Color(0, 0, 255), 50);
          mp3.play(11);
        //BPMod->sendString("9"); ENVIAR TECLA MODULO BLUETOOTH (NO FUNCIONA)
          
          
        } else {
          RFID_value = "unknown";
          
        }
        Serial.print("RFID Key Scanned: " + RFID_value + '\n');
        Serial.print(uid);
        //delay(1000);
       }       
//////////////////////////////////////////////////////////////// RFID SCAN TAGS
 */
} 

////////////////////////////////////////////////////////////////////////////////// FINAL VOID LOOP //////////////////////////


/////////////////////////////////////////////////////////// COLOR WHEEL
// given a wheel position in 0-255 range
// return a rainbow color adjusted by intensity 0 to 1.0
uint32_t colorWheel(float intensity, byte wheelPos)
{
  const int WHEEL_THIRD = (WHEEL_SIZE - 1) / 3;
  
  if (intensity < 0.0 ) intensity = 0.0;
  if (intensity > 1.0) intensity = 1.0;

  // as wheelPos progresses from 0 to 255 once, colorIndex should progress from 0 to 255 3 times
  // find out position in current third of wheel then multiple by 3 to get full color value
  byte colorIndex = (wheelPos % WHEEL_THIRD) * 3;
  
  int fadeColor = (255 - colorIndex) * intensity;  // color going down
  int increaseColor = colorIndex * intensity;    // color going up
  
  switch (wheelPos / WHEEL_THIRD) {
    case 0:    // first third of the wheel, red fading, no green, blue increasing
      return Adafruit_NeoPixel::Color(fadeColor, 0, increaseColor);
      break;
    case 1:    // second third of the wheel no red, green increasing, blue fading
      return Adafruit_NeoPixel::Color(0, increaseColor, fadeColor);
      break;
  
    case 2:    // last third of the wheel, red increasing, green fading, no blue
      return Adafruit_NeoPixel::Color(increaseColor, fadeColor, 0);
      break;
  }
}
////////////////////////////////////////////////////// COLOR WHEEL


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON VERDE
void startShow(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
    case 1: colorWipe(strip.Color(0, 255, 0), 50);  // Green 
        mp3.play(1);
            break;
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON VERDE


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON ROJO
void startShowRojo(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
        mp3.play(2);
            break;
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON ROJO


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON AMARILLO
void startShowAmarillo(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
    case 1: colorWipe(strip.Color(255, 255, 0), 50);  // Yellow
        mp3.play(3);
            break;
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON AMARILLO


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON AZUL
void startShowAzul(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
    case 1: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
        mp3.play(4);
            break;
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON AZUL


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO LLAVE
void startShowLlave(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
  //          break;
    case 1: colorWipe(strip.Color(128, 0, 128), 50);  // Purple
        mp3.play(5);
            break;
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO LLAVE


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH ROJO
void startShowSwRojo(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(127,   0,   0), 50); // Red
        mp3.play(6);
            break;
  }
}       
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH ROJO


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH VERDE
void startShowSwVerde(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(0,   255,   0), 50); // Green
        mp3.play(7);
            break;
  }
}       
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH VERDE


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH AZUL
void startShowSwAzul(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
        mp3.play(8);
            break;
  }
}      
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH AZUL


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH NARANJO
void startShowSwNaranjo(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 70, 0), 50);  // Orange
        mp3.play(9);
            break;
  }
}    
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO SWITCH NARANJO


///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON JOYSTICK
void startShowJoystick(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
//    case 7: rainbow(20);
//            break;
    case 10: mp3.play(11);
            theaterChaseRainbow(50);
            break;
     case 11: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
         mp3.play(10);
            break;    
  }
}
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON JOYSTICK

///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON ROTARY ENCODER
/*void startShowRotary(int i) {
  switch(i){
//    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
//            break;
//    case 7: rainbow(20);
//            break;
    case 10: mp3.play(7);
            rainbow(100);
            break;
    case 11: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
         mp3.play(10);
            break;    
  }
} */
///////////////////////////////////////////////////////////////////// LUZ Y SONIDO BOTON ROTARY ENCODER


/////////////////////////////////////////////////////////// ANIMACIONES LUCES RGB
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
/////////////////////////////////////////////////////////// ANIMACIONES LUCES RGB


//////////////////////////////////////////////////////////// MENU SERIAL REPRODUCTOR MP3
void menu(char op, int nval){ //MP3
  // Menu
  switch (op){
    case '?':
    case 'h':
        Serial.println("SerialMP3Player Basic Commands:");
        Serial.println(" ? - Display Menu options. ");
        Serial.println(" P01 - Play 01 file");
        Serial.println(" F01 - Play 01 folder");
        Serial.println(" S01 - Play 01 file in loop");
        Serial.println(" V01 - Play 01 file, volume 30");
        Serial.println(" p - Play");
        Serial.println(" a - pause");
        Serial.println(" s - stop ");
        Serial.println(" > - Next");
        Serial.println(" < - Previous");
        Serial.println(" + - Volume UP");
        Serial.println(" - - Volume DOWN");
        Serial.println(" v15 - Set Volume to 15");
        Serial.println(" c - Query current file");
        Serial.println(" q - Query status");
        Serial.println(" x - Query folder count");
        Serial.println(" t - Query total file count");
        Serial.println(" r - Reset");
        Serial.println(" e - Sleep");
        Serial.println(" w - Wake up");
        break;

    case 'P':
        Serial.println("Play");
        mp3.play(nval);
        break;

    case 'F':
        Serial.println("Play Folder");
        mp3.playF(nval);
        break;

    case 'S':
        Serial.println("Play loop");
        mp3.playSL(nval);
        break;

    case 'V':
        Serial.println("Play file at 30 volume");
        mp3.play(nval,30);
        break;


    case 'p':
        Serial.println("Play");
        mp3.play();
        break;

    case 'a':
        Serial.println("Pause");
        mp3.pause();
        break;

    case 's':
        Serial.println("Stop");
        mp3.stop();
        break;

    case '>':
        Serial.println("Next");
        mp3.playNext();
        break;

    case '<':
        Serial.println("Previous");
        mp3.playPrevious();
        break;

    case '+':
        Serial.println("Volume UP");
        mp3.volUp();
        break;

    case '-':
        Serial.println("Volume Down");
        mp3.volDown();
        break;

    case 'v':
        Serial.println("Set to Volume");
          mp3.setVol(nval);
          mp3.qVol();
        break;

    case 'c':
        Serial.println("Query current file");
        mp3.qPlaying();
        break;

    case 'q':
        Serial.println("Query status");
        mp3.qStatus();
        break;

    case 'x':
        Serial.println("Query folder count");
        mp3.qTFolders();
        break;

    case 't':
        Serial.println("Query total file count");
        mp3.qTTracks();
        break;

    case 'r':
        Serial.println("Reset");
        mp3.reset();
        break;

    case 'e':
        Serial.println("Sleep");
        mp3.sleep();
        break;

    case 'w':
        Serial.println("Wake up");
        mp3.wakeup();
        break;
  }
}
//////////////////////////////////////////////////////////// MENU SERIAL REPRODUCTOR MP3


//////////////////////// DECODE C MP3 SERIAL REPRODUCTOR
void decode_c(){
  // Decode c looking for a specific command or a digit

  // if c is a 'v', 'P', 'F', 'S' or 'V' wait for the number XX
  if (c=='v' || c=='P' || c=='F' || c=='S' || c=='V'){
    cmd=c;
  }else{
    // maybe c is part of XX number
    if(c>='0' && c<='9'){
      // if c is a digit
      if(cmd1==' '){
        // if cmd1 is empty then c is the first digit
        cmd1 = c;
      }else{
        // if cmd1 is not empty c is the second digit
        menu(cmd, ((cmd1-'0')*10)+(c-'0'));
        cmd = ' ';
        cmd1 = ' ';
      }
    }else{
      // c is not a digit nor 'v', 'P', 'F' or 'S' so just call menu(c, nval);
      menu(c, 0);
    }
  }
} 
//////////////////////// DECODE C MP3 SERIAL REPRODUCTOR
