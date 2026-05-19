#include "Notifigram.h"
#include "pantalla.h"
#include "config.h"

void procesarBluetooth() {
  while (SerialBT.hasClient() && SerialBT.available()) {
    String data = SerialBT.readStringUntil('\n');
    data.trim();

    Serial.println("📩 Datos BT recibidos (" + String(data.length()) + " bytes)");
    btReceived = true;

    if (data == "status") {
      SerialBT.println("OK");
      Serial.println("📶 Petición de estado respondida");
      continue;
    }

    if (data == "reset") {
      Serial.println("♻️ Reinicio solicitado por Bluetooth");
      ESP.restart();
    }

    if (data == "GET") {
      StaticJsonDocument<256> doc;
      doc["ssid"]   = ssid;
      doc["pass"]   = password;
      doc["token"]  = botToken;
      String out;
      serializeJson(doc, out);
      SerialBT.println(out);
      SerialBT.flush();
      Serial.println("📤 Enviada config JSON al cliente BT (datos ocultos)");
      continue;
    }

    // 👇 Aquí tratamos el JSON
    if (data.startsWith("{") && data.endsWith("}")) {
      Serial.println("📥 JSON recibido (" + String(data.length()) + " bytes)");
      StaticJsonDocument<512> doc;
      DeserializationError err = deserializeJson(doc, data);
      if (err) {
        Serial.print("⚠️ JSON inválido: ");
        Serial.println(err.c_str());
        return;
      }

      ssid     = doc["ssid"].as<String>();     ssid.trim();
      password = doc["pass"].as<String>();     password.trim();
      botToken = doc["token"].as<String>();    botToken.trim();

      // TODO: las globales ssid/password/botToken ya han sido sobrescritas en RAM
      // antes de este check. SPIFFS conserva la config buena (no llamamos a
      // guardarConfig), pero las globales quedan a "" hasta proximo reinicio o
      // JSON valido. En el flujo real esto no se manifiesta porque este codigo
      // se ejecuta en setup inicial con BT forzado, sin WiFi activo que use
      // estas globales. Refactor pendiente: parsear a variables temporales,
      // validar, y solo entonces asignar a las globales.
      if (ssid.length() == 0 || password.length() == 0 || botToken.length() == 0) {
        Serial.println("⚠️ JSON con campos vacíos (ssid/pass/token), no se guarda");
        SerialBT.println("ERR_EMPTY");
        mostrarTexto("Config KO", matrix.Color(255, 0, 0));
        return;
      }

      guardarConfig();
      mostrarTexto("Config OK", matrix.Color(0,255,0));
      Serial.println("✅ Config recibida, reiniciando...");
      ESP.restart();
    }
  }
}
