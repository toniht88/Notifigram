#ifndef NOTIFIGRAM_H
#define NOTIFIGRAM_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>

#define PIN               13
#define HTTP_TIMEOUT_MS   5000
#define TelegramCheckIntv 3000
#define HOLD_ECO_MS       5000UL

// 🟢 Variables globales externas
extern String ssid, password, botToken;
extern std::vector<String> chat_ids;
extern bool btReceived, wifiActivo, btForzado;
extern unsigned long lastCheck;
extern uint16_t textoColor;
extern bool ntpSincronizado;
extern unsigned long ultimoMensajeRealMs;
extern unsigned long ultimoEcoMs;
extern unsigned long tiempoSalvapantallasMs;
extern bool relojDesactivado;

extern BluetoothSerial SerialBT;
extern WiFiClientSecure secureClient;
extern UniversalTelegramBot* bot;
extern Adafruit_NeoMatrix matrix;
extern unsigned long lastUpdateId;

extern String nombreColorActual;
extern String nivelBrilloActual;

// 🟢 Enumeración y funciones de efectos de texto
enum TipoEfectoTexto {
  EFECTO_SIN_EFECTO,
  EFECTO_SCROLL,
  EFECTO_SCROLL_REVERSA,
  EFECTO_REBOTE,
  EFECTO_LETRA_POR_LETRA,
  EFECTO_PARPADEO,
  EFECTO_ZOOM,
  EFECTO_FADE,
  EFECTO_ARCOIRIS,
  EFECTO_WIPE,
  EFECTO_AGITACION,
  EFECTO_PULSACION,
  EFECTO_INVERTIDO
};

extern TipoEfectoTexto efectoActual;
extern Adafruit_NeoMatrix matrix;


TipoEfectoTexto obtenerEfectoDesdeComando(const String& cmd);
void mostrarTextoConEfecto(const String& texto, uint16_t color, TipoEfectoTexto efecto, bool esEco = false);

// 🟢 Funciones del sistema
void iniciarSistema();
void procesarBluetooth();
void procesarTelegram();

#endif // NOTIFIGRAM_H
