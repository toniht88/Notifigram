#include "Notifigram.h"
#include "pantalla.h"  // ✅ Necesario para usar obtenerEfectoDesdeNombre() y nombreEfecto()

// Variables globales para guardar los nombres del color y brillo
String nombreColorActual = "desconocido";
String nivelBrilloActual = "desconocido";

bool cargarConfig() {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Error al montar SPIFFS");
    return false;
  }

  if (!SPIFFS.exists("/config.json")) {
    Serial.println("⚠️ /config.json no existe. Creándolo vacío...");
    File f = SPIFFS.open("/config.json", FILE_WRITE);
    if (f) {
      f.print("{\n  \"ssid\": \"\",\n  \"pass\": \"\",\n  \"token\": \"\",\n  \"chat_ids\": [],\n  \"last_update_id\": 0\n}");
      f.close();
      Serial.println("✅ Archivo creado, esperando configuración...");
    } else {
      Serial.println("❌ Error al crear archivo");
    }
    return false;
  }

  File f = SPIFFS.open("/config.json", FILE_READ);
  if (!f) {
    Serial.println("❌ No se pudo abrir /config.json");
    return false;
  }

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    Serial.print("❌ Error al parsear JSON: ");
    Serial.println(error.c_str());
    return false;
  }

  ssid     = doc["ssid"]  | "";  ssid.trim();
  password = doc["pass"]  | "";  password.trim();
  botToken = doc["token"] | "";  botToken.trim();
  lastUpdateId = doc["last_update_id"] | 0;

  if (doc.containsKey("chat_ids")) {
    for (JsonVariant v : doc["chat_ids"].as<JsonArray>()) {
      String id = v.as<String>();
      id.trim();
      if (std::find(chat_ids.begin(), chat_ids.end(), id) == chat_ids.end()) {
        chat_ids.push_back(id);
      }
    }
  }

  int r = doc["color_r"] | 255;
  int g = doc["color_g"] | 255;
  int b = doc["color_b"] | 255;
  textoColor = matrix.Color(r, g, b);

  int brillo = doc["brillo"] | 40;
  matrix.setBrightness(brillo);

  // Nuevos: cargar nombre del color, brillo y efecto
  nombreColorActual = doc["color_nombre"] | "desconocido";
  nivelBrilloActual = doc["brillo_nombre"] | "desconocido";
  efectoActual = obtenerEfectoDesdeNombre(doc["efecto"] | "sin_efecto");

  tiempoSalvapantallasMs = doc["tiempo_ms"] | 300000UL;
  relojDesactivado       = doc["reloj_off"] | false;

  Serial.println("✅ Configuración cargada correctamente");
  return true;
}

void guardarConfig() {
  StaticJsonDocument<1024> doc;
  doc["ssid"]  = ssid;
  doc["pass"]  = password;
  doc["token"] = botToken;
  doc["last_update_id"] = lastUpdateId;

  JsonArray ids = doc.createNestedArray("chat_ids");
  for (const String& id : chat_ids) {
    ids.add(id);
  }

  doc["color_r"] = (textoColor >> 11) * 255 / 31;
  doc["color_g"] = ((textoColor >> 5) & 0x3F) * 255 / 63;
  doc["color_b"] = (textoColor & 0x1F) * 255 / 31;
  doc["brillo"] = matrix.getBrightness();

  // Nuevos: guardar nombre del color, brillo y efecto
  doc["color_nombre"] = nombreColorActual;
  doc["brillo_nombre"] = nivelBrilloActual;
  doc["efecto"] = efectoActual;

  doc["tiempo_ms"] = tiempoSalvapantallasMs;
  doc["reloj_off"] = relojDesactivado;

  File f = SPIFFS.open("/config.json", FILE_WRITE);
  if (!f) {
    Serial.println("❌ Error al guardar configuración");
    return;
  }

  serializeJson(doc, f);
  f.close();
  Serial.println("💾 Configuración guardada");
}
