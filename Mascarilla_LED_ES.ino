
/*
 Led Mask based on LED VU meter using Adafruit NeoPixel Wheel.
 Carlos Fernández Mejías
 */

#include <Adafruit_NeoPixel.h>
#include <math.h>

#define MIC_PIN   A7  // Pin del microfono
#define LED_PIN    6  // Pin neopixels
#define dato_vel   10  // Velocidad de lectura leds
#define pico_HANG 5 //Tiempo de espera valor maximo (pico)
#define pico_FALL 2 //Tiempo de bajada valor maximo (pico)
#define VAL_MIN 150 //Manor valor de lectura del microfono (Analogico)
#define VAL_MAX 500 //Maximo valor de lectura, Mayor valor-->Mas sensibilidad(1023 = maximo)

byte pico = 16;      // Valor maximo.
unsigned int dato;

byte cuentapico = 0;  //Cuantos leds ha bajado el pico
byte dotHangCount = 0; //Valor en el que se para el pico

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() 
{
  // Serial.begin(9600); PRUEBA
  strip.begin();
  strip.show(); // Inicializa los pixeles a off

}

void loop() 
{
  unsigned long beginMillis= millis();  // Comienza muestreo de datos analogicos
  float picoTopico = 0;   // Valor MAX-MIN

  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;


  // Guarda los datos del muestreo (in mS)
  while (millis() - beginMillis < dato_vel)
  {
    dato = analogRead(MIC_PIN);
    if (dato < 1024)  // eliminacion de muestras que esten fuera del umbral
    {
      if (dato > signalMax)
      {
        signalMax = dato;  // guarda los valores maximos
      }
      else if (dato < signalMin)
      {
        signalMin = dato;  // guarda los valores minimos
      }
    }
  }
  picoTopico = signalMax - signalMin;  // max - min = amplitud pico-pico
 
  // Serial.println(picoTopico); PRUEBA


  //Para calcular la posicion del pixel de forma gradiente
  for (int i=0;i<=strip.numPixels()-1;i++){
    strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
  }


  //Combierte de lineal a logaritmico los datos
  c = fscale(VAL_MIN, VAL_MAX, strip.numPixels(), 0, picoTopico, 2);

  


  if(c < pico) {
    pico = c;        // 
    dotHangCount = 0;    // Aguanto el pico en el valor maximo
  }
  if (c <= strip.numPixels()) { // Relleno los leds no usados a off 
    drawLine(strip.numPixels(), strip.numPixels()-c, strip.Color(0, 0, 0));
  }

  // Establecer el pico para que coincida con el degradado
  y = strip.numPixels() - pico;
  
  strip.setPixelColor(y-1,Wheel(map(y,0,strip.numPixels()-1,30,150)));

  strip.show();

  // Animacion del pico
  if(dotHangCount > pico_HANG) { //Tiempo de aguente del pico
    if(++cuentapico >= pico_FALL) { //Ritmo de bajada
      pico++;
      cuentapico = 0;
    }
  } 
  else {
    dotHangCount++; 
  }
}

//Relleno los leds del minimo al maximo con los valores elegidos
void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    strip.setPixelColor(i, c);
  }
}


float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputvalor, float curve){

  float rangoOrigen = 0;
  float rangoNuevo = 0;
  float ceroRefCurVal = 0;
  float normaCurVal = 0;
  float rangovalor = 0;
  boolean invFlag = 0;


  // valor limite

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // los números positivos dan más peso al extremo alto en la salida
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function converte de lineal a logaritimo para otras funciones

  /*
   Serial.println(curve * 100, DEC);   // prueba
   Serial.println(); 
   */

  // Chequea los valores que estan fuera del umbral
  if (inputvalor < originalMin) {
    inputvalor = originalMin;
  }
  if (inputvalor > originalMax) {
    inputvalor = originalMax;
  }

  // Valores iniciales
  rangoOrigen = originalMax - originalMin;

  if (newEnd > newBegin){ 
    rangoNuevo = newEnd - newBegin;
  }
  else
  {
    rangoNuevo = newBegin - newEnd; 
    invFlag = 1;
  }

  ceroRefCurVal = inputvalor - originalMin;
  normaCurVal  =  ceroRefCurVal / rangoOrigen;   // Hacemos casting a float

  // Chequeo para el valor maximo original sea mayor que el minimo original
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangovalor =  (pow(normaCurVal, curve) * rangoNuevo) + newBegin;

  }
  else     // invierto los rangos
  {   
    rangovalor =  newBegin - (pow(normaCurVal, curve) * rangoNuevo); 
  }

  return rangovalor;
}



// TRANSICION DE VERDE A ROJO
/*uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos*3, 255, 0);
  } 
  else if(WheelPos < 130) {
    WheelPos -= 85;
    return strip.Color(255, 255 - WheelPos*3, 0);
  } 
  else {
     WheelPos -= 130;
    return strip.Color(255,0, 0);
  }
}*/
//  TRANSICION ARCOIRIS
 uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
