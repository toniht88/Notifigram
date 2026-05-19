#include "Notifigram.h"
#include <Arduino.h>
#include "Notifigram.h"
#include "pantalla.h"
#include "config.h"
#include "bluetooth.h"

void inicializarContadorYTiempo();

void iniciarSistema() {
  Serial.begin(115200);
  Serial.println("🟢 Inicializando Notifigram...");

  matrix.begin(); 
  matrix.setTextWrap(false); 
  matrix.setBrightness(100);
  textoColor = matrix.Color(255, 255, 255);
  efectoActual = EFECTO_SCROLL;
  textoColor = matrix.Color(255, 255, 255);  // Blanco
  mostrarTexto("Notifigram", textoColor);


  Serial.println("➡️ Cargando configuración...");
  if (!cargarConfig()) {
    mostrarTexto("Espera BT", matrix.Color(255,100,0));
    Serial.println("📲 Esperando comunicación con la APP...");
    WiFi.disconnect(true);
    SerialBT.begin("Notifigram");
    wifiActivo = false;
    btForzado = true;
    return;
  }

  // Conexión Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  int intentosWifi = 0;
  while (WiFi.status() != WL_CONNECTED && intentosWifi < 5) {
    Serial.println("📡 Intentando conectar al Wi-Fi...");
    delay(1000);
    intentosWifi++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No se pudo conectar a Wi-Fi tras 5 intentos");
    efectoActual = EFECTO_SCROLL;
    mostrarTexto("WifiKO", textoColor);
    efectoActual = EFECTO_SCROLL;
    mostrarTexto("APP->", textoColor);
    WiFi.disconnect(true);
    SerialBT.begin("Notifigram");
    wifiActivo = false;
    btForzado = true;
    mostrarTexto("Modo BT", matrix.Color(255, 0, 0));
    return;
  }

  // Conexión OK
  Serial.println("✅ Wi-Fi conectado: " + WiFi.localIP().toString());
  wifiActivo = true;

  // Sincronización NTP (Europe/Madrid con DST automático)
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3",
               "pool.ntp.org", "time.google.com", "time.cloudflare.com");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10000) && timeinfo.tm_year >= 120) {
    ntpSincronizado = true;
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    Serial.printf("✅ NTP sincronizado: %s\n", buf);
  } else {
    ntpSincronizado = false;
    Serial.println("❌ NTP no sincronizado, reloj desactivado");
  }

  // Bot Telegram
  secureClient.setInsecure();  // evitar errores de certificado
  bot = new UniversalTelegramBot(botToken.c_str(), secureClient);

  bool conectado = false;
  for (int i = 0; i < 5; i++) {
    Serial.println("🤖 Intentando conectar con bot...");
    if (!bot->getMe()) {
      delay(1000);
    } else {
      conectado = true;
      Serial.println("✅ Bot OK");
      break;
    }
  }

  if (!conectado) {
    Serial.println("❌ No se pudo conectar con el bot");
    efectoActual = EFECTO_SCROLL; 
    mostrarTexto("BotKO", textoColor);
    efectoActual = EFECTO_SCROLL;
    mostrarTexto("APP->", textoColor);
    WiFi.disconnect(true);
    SerialBT.begin("Notifigram");
    wifiActivo = false;
    btForzado = true;
    mostrarTexto("Modo BT", matrix.Color(255, 0, 0));
  } else {
    Serial.println("📬 Bot listo para recibir mensajes");
  }
}
