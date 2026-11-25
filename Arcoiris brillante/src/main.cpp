#include <Arduino.h>
#include <FastLED.h>
#include <Servo.h>

// --- CONFIGURACIÓN TIRA WS2811 ---
// 75 LEDs a 12V = 25 chips (1 chip cada 3 LEDs)
#define NUM_LEDS     25
#define DATA_PIN     6
#define LED_TYPE     WS2811
#define COLOR_ORDER  BRG   // Si ves colores cambiados, probá RGB o GRB

CRGB leds[NUM_LEDS];

const CRGB colores[7] = {
  CRGB::Red,     // 0: Rojo
  CRGB::Green,   // 1: Verde
  CRGB::Blue,    // 2: Azul
  CRGB::White,   // 3: Blanco
  CRGB::Yellow,  // 4: Amarillo
  CRGB::Magenta, // 5: Magenta
  CRGB::Cyan     // 6: Cian
};

// --- CONFIGURACIÓN SENSOR HC-SR04 ---
const int pinTrig = 7;
const int pinEcho = 8;

// --- CONFIGURACIÓN SERVO SG90 ---
const int pinServo = 9;
Servo miServo;

// Distancia a la que se activa el sistema (en cm)
const int distanciaActivacion = 30;
bool estadoActivo = false;

// Mide distancia en centímetros usando HC-SR04
float medirDistancia() {
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  long duracion = pulseIn(pinEcho, HIGH, 30000);
  if (duracion == 0) {
    return 999.0;
  }
  return (duracion * 0.0343) / 2.0;
}

void asignarColoresAleatorios() {
  for (int i = 0; i < NUM_LEDS; i++) {
    int indiceColor = random(0, 7);
    leds[i] = colores[indiceColor];
  }
  FastLED.show();
}

void apagarTira() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(180);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrig, LOW);

  miServo.attach(pinServo);
  miServo.write(0);

  randomSeed(analogRead(A3));
}

void loop() {
  float distancia = medirDistancia();

  if (distancia > 0 && distancia <= distanciaActivacion) {
    if (!estadoActivo) {
      miServo.write(90);
      estadoActivo = true;
    }
    asignarColoresAleatorios();
    delay(250);
  } else {
    if (estadoActivo) {
      miServo.write(0);
      apagarTira();
      estadoActivo = false;
    }
    delay(100);
  }
}