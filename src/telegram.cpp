#include "Notifigram.h"
#include "pantalla.h"
#include "config.h"

#define FIRMWARE_VERSION "1.0"

unsigned long lastUpdateId = 0;
unsigned long tiempoInicio = 0;
int contadorReinicios = 0;

extern String nombreColorActual;
extern String nivelBrilloActual;

void inicializarContadorYTiempo() {
  tiempoInicio = millis();
  contadorReinicios++;
}

static bool enviarMensajeDedupe(const String& chatId, const String& texto, const String& parseMode) {
  static String ultimoChatId = "";
  static String ultimoTexto = "";
  static unsigned long ultimoEnvioMs = 0;
  unsigned long ahora = millis();
  if (chatId == ultimoChatId && texto == ultimoTexto && (ahora - ultimoEnvioMs) < 5000UL) {
    Serial.println("🔁 sendMessage duplicado bloqueado (ventana 5s)");
    return false;
  }
  bool ok = bot->sendMessage(chatId, texto, parseMode);
  if (ok) {
    ultimoChatId = chatId;
    ultimoTexto = texto;
    ultimoEnvioMs = ahora;
  }
  return ok;
}

void procesarTelegram() {
  if (!bot || WiFi.status() != WL_CONNECTED || !wifiActivo) return;
  if (millis() - lastCheck < TelegramCheckIntv) return;
  lastCheck = millis();

  int newMsgs = bot->getUpdates(lastUpdateId + 1);
  if (newMsgs <= 0) return;

  for (int i = 0; i < newMsgs; i++) {
    String id  = bot->messages[i].chat_id;
    String msg = bot->messages[i].text;
    msg.trim();

    if (!msg.startsWith("/")) {
      unsigned long ms = millis();
      unsigned long seg = (ms / 1000) % 60;
      unsigned long min = (ms / 1000 / 60) % 60;
      unsigned long hora = (ms / 1000 / 60 / 60);
      Serial.printf("🕒 %02lu:%02lu:%02lu - Mensaje recibido: %s\n", hora, min, seg, msg.c_str());
    }

    if (std::find(chat_ids.begin(), chat_ids.end(), id) == chat_ids.end()) {
      chat_ids.push_back(id);
      enviarMensajeDedupe(id, "👋 Hola Bot. Usa /help para ver comandos disponibles.", "");
    }

    else if (msg == "/color") {
      String menu = "🎨 Elige un color:\n";
      menu += "/color_rojo\n";
      menu += "/color_verde\n";
      menu += "/color_azul\n";
      menu += "/color_blanco\n";
      menu += "/color_amarillo\n";
      menu += "/color_cian\n";
      menu += "/color_magenta\n";
      menu += "/color_naranja\n";
      menu += "/color_rosa\n";
      menu += "/color_violeta\n";
      menu += "/color_gris\n";
      menu += "/color_lima\n";
      menu += "/color_aqua\n";
      menu += "/color_oro\n";
      menu += "/color_turquesa";
      enviarMensajeDedupe(id, menu, "");
    }

    else if (msg.startsWith("/color_")) {
      String nombre = msg.substring(7);
      nombre.toLowerCase();

      struct Color {
        const char* nombre;
        uint8_t r, g, b;
      };

      Color colores[] = {
        {"rojo", 255, 0, 0}, {"verde", 0, 255, 0}, {"azul", 0, 0, 255},
        {"blanco", 255, 255, 255}, {"amarillo", 255, 255, 0},
        {"cian", 0, 255, 255}, {"magenta", 255, 0, 255},
        {"naranja", 255, 128, 0}, {"rosa", 255, 102, 178},
        {"violeta", 128, 0, 255}, {"gris", 128, 128, 128},
        {"lima", 128, 255, 0}, {"aqua", 0, 128, 255},
        {"oro", 255, 215, 0}, {"turquesa", 64, 224, 208}
      };

      bool encontrado = false;
      for (Color c : colores) {
        if (nombre == c.nombre) {
          textoColor = matrix.Color(c.r, c.g, c.b);
          nombreColorActual = c.nombre;
          guardarConfig();
          enviarMensajeDedupe(id, "🎨 Color cambiado a " + nombre, "");
          mostrarTexto(nombre, textoColor, efectoActual);
          delay(5000);
          matrix.fillScreen(0);
          matrix.show();
          encontrado = true;
          break;
        }
      }

      if (!encontrado) {
        enviarMensajeDedupe(id, "❌ Color no válido. Usa /color para ver la lista.", "");
      }
    }

    else if (msg == "/brillo") {
      String menu = "💡 Elige un nivel de brillo:\n";
      menu += "/brillo_maximo\n";
      menu += "/brillo_alto\n";
      menu += "/brillo_medio\n";
      menu += "/brillo_bajo\n";
      menu += "/brillo_noche\n";
      menu += "/brillo_minimo";
      enviarMensajeDedupe(id, menu, "");
    }

    else if (msg.startsWith("/brillo_")) {
      int brillo = 128;
      if (msg == "/brillo_minimo") { brillo = 10; nivelBrilloActual = "mínimo"; }
      else if (msg == "/brillo_bajo") { brillo = 50; nivelBrilloActual = "bajo"; }
      else if (msg == "/brillo_noche") { brillo = 30; nivelBrilloActual = "noche"; }
      else if (msg == "/brillo_medio") { brillo = 128; nivelBrilloActual = "medio"; }
      else if (msg == "/brillo_alto") { brillo = 200; nivelBrilloActual = "alto"; }
      else if (msg == "/brillo_maximo") { brillo = 255; nivelBrilloActual = "máximo"; }

      matrix.setBrightness(brillo);
      guardarConfig();
      enviarMensajeDedupe(id, "💡 Brillo ajustado a " + nivelBrilloActual, "");
      mostrarTexto(nivelBrilloActual, textoColor, efectoActual);
      delay(5000);
      matrix.fillScreen(0);
      matrix.show();
    }

    else if (msg == "/reset") {
      lastUpdateId = bot->messages[i].update_id;
      enviarMensajeDedupe(id, "♻️ Reiniciando...", "");
      delay(300);
      guardarConfig();
      ESP.restart();
    }

    else if (msg == "/btON") {
      lastUpdateId = bot->messages[i].update_id;
      enviarMensajeDedupe(id, "📰 Cambiado a modo Bluetooth (Wi-Fi desactivado)", "");
      WiFi.disconnect(true);
      wifiActivo = false;
      if (!SerialBT.hasClient()) {
        SerialBT.begin("Notifigram");
      }
      btForzado = true;
      guardarConfig();
      delete bot;
      bot = nullptr;
      return;
    }

    else if (msg == "/estado") {
      unsigned long uptimeSecs = (millis() - tiempoInicio) / 1000;
      unsigned long mins = uptimeSecs / 60;
      unsigned long hours = mins / 60;
      mins %= 60;
      unsigned long secs = uptimeSecs % 60;

      String estado = "📌 Estado actual:\n";
      estado += wifiActivo ? "🔗 Modo: Wi-Fi\n" : "📦 Modo: Bluetooth\n";
      if (wifiActivo && WiFi.status() == WL_CONNECTED) {
        estado += "📱 IP: " + WiFi.localIP().toString() + "\n";
      }
      estado += "💡 Brillo: " + nivelBrilloActual + "\n";
      estado += "🎨 Color actual: " + nombreColorActual + "\n";
      estado += "🎭 Efecto: " + nombreEfecto(efectoActual) + "\n";
      estado += "🤖 Bot activo: " + String(bot ? "Sí" : "No") + "\n";
      estado += "🕒 Uptime: " + String(hours) + "h " + String(mins) + "m " + String(secs) + "s\n";
      estado += "🔀 Reinicios: " + String(contadorReinicios) + "\n";
      estado += "🧵 Versión de firmware: " FIRMWARE_VERSION "\n";
      estado += "🥒 Firmware: " __DATE__ " " __TIME__ "\n";
      estado += "⏰ Tiempo salvapantallas: ";
      if (relojDesactivado) estado += "desactivado";
      else                  estado += String(tiempoSalvapantallasMs / 1000) + "s";
      estado += "\n";
      estado += "📨 Bot escuchando mensajes...";
      enviarMensajeDedupe(id, estado, "");
    }

    else if (msg == "/efecto") {
      String menu = "🎭 Elige un efecto:\n";
      menu += "/efecto_sin_efecto\n";
      menu += "/efecto_scroll\n";
      menu += "/efecto_scroll_reversa\n";
      menu += "/efecto_rebote\n";
      menu += "/efecto_letra_por_letra\n";
      menu += "/efecto_parpadeo\n";
      menu += "/efecto_zoom\n";
      menu += "/efecto_fade\n";
      menu += "/efecto_arcoiris\n";
      menu += "/efecto_wipe\n";
      menu += "/efecto_agitacion\n";
      menu += "/efecto_pulsacion\n";
      menu += "/efecto_invertido";
      enviarMensajeDedupe(id, menu, "");
    }

    else if (msg.startsWith("/efecto_")) {
      efectoActual = obtenerEfectoDesdeComando(msg);
      guardarConfig();
      mostrarTexto(nombreEfecto(efectoActual), textoColor, efectoActual);
      delay(5000);
      matrix.fillScreen(0);
      matrix.show();
    }

    else if (msg == "/tiempo") {
      String menu = "⏰ Elige el tiempo del salvapantallas:\n";
      menu += "/tiempo_5s\n";
      menu += "/tiempo_30s\n";
      menu += "/tiempo_1min\n";
      menu += "/tiempo_3min\n";
      menu += "/tiempo_5min\n";
      menu += "/tiempo_10min\n";
      menu += "/tiempo_30min\n";
      menu += "/tiempo_off";
      enviarMensajeDedupe(id, menu, "");
    }

    else if (msg.startsWith("/tiempo_")) {
      String etiqueta = "";
      if (msg == "/tiempo_off") {
        relojDesactivado = true;
        etiqueta = "desactivado";
      } else {
        relojDesactivado = false;
        if      (msg == "/tiempo_5s")    { tiempoSalvapantallasMs =    5000UL; etiqueta = "5 segundos"; }
        else if (msg == "/tiempo_30s")   { tiempoSalvapantallasMs =   30000UL; etiqueta = "30 segundos"; }
        else if (msg == "/tiempo_1min")  { tiempoSalvapantallasMs =   60000UL; etiqueta = "1 minuto"; }
        else if (msg == "/tiempo_3min")  { tiempoSalvapantallasMs =  180000UL; etiqueta = "3 minutos"; }
        else if (msg == "/tiempo_5min")  { tiempoSalvapantallasMs =  300000UL; etiqueta = "5 minutos"; }
        else if (msg == "/tiempo_10min") { tiempoSalvapantallasMs =  600000UL; etiqueta = "10 minutos"; }
        else if (msg == "/tiempo_30min") { tiempoSalvapantallasMs = 1800000UL; etiqueta = "30 minutos"; }
      }
      if (etiqueta.length() > 0) {
        guardarConfig();
        if (relojDesactivado) enviarMensajeDedupe(id, "⏰ Reloj desactivado", "");
        else                  enviarMensajeDedupe(id, "⏰ Tiempo cambiado a " + etiqueta, "");
      } else {
        enviarMensajeDedupe(id, "❌ Tiempo no válido. Usa /tiempo para ver la lista.", "");
      }
    }

    else if (msg == "/help") {
      String ayuda = "📖 *Comandos disponibles:*\n";
      ayuda += "/color – Muestra lista de colores\n";
      ayuda += "/brillo – Muestra niveles de brillo\n";
      ayuda += "/efecto 🎭 Muestra lista de efectos\n";
      ayuda += "/tiempo – Configura tiempo del salvapantallas\n";
      ayuda += "/reset – Reinicia\n";
      ayuda += "/btON – Cambia a modo Bluetooth\n";
      ayuda += "/estado – Estado del sistema\n";
      ayuda += "/help – Ayuda general";
      enviarMensajeDedupe(id, ayuda, "Markdown");
    }

    else {
      static String ultimoTextoMostrado = "";
      static unsigned long ultimoTextoMostradoEn = 0;
      unsigned long ahora = millis();
      if (msg == ultimoTextoMostrado && (ahora - ultimoTextoMostradoEn) < 30000UL) {
        Serial.println("🔁 Mensaje duplicado ignorado (ventana 30s)");
      } else {
        enviarMensajeDedupe(id, "✅ Leído", "");
        mostrarTextoConEfecto(msg, textoColor, efectoActual);
        ultimoTextoMostrado = msg;
        ultimoTextoMostradoEn = ahora;
      }
    }

    if (bot->messages[i].update_id > lastUpdateId) {
      lastUpdateId = bot->messages[i].update_id;
    }
  }
}
