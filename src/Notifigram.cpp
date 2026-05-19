#include "Notifigram.h"

// Variables globales
String ssid = "";
String password = "";
String botToken = "";
std::vector<String> chat_ids;

bool btReceived = false;
bool wifiActivo = false;
bool btForzado = false;
unsigned long lastCheck = 0;

bool ntpSincronizado = false;
unsigned long ultimoMensajeRealMs = 0;
unsigned long ultimoEcoMs = 0;
unsigned long tiempoSalvapantallasMs = 300000UL;
bool relojDesactivado = false;

uint16_t textoColor = 0xFFFF;

BluetoothSerial SerialBT;
WiFiClientSecure secureClient;
UniversalTelegramBot* bot = nullptr;

// Pantalla LED
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  32, 8, PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800
);
