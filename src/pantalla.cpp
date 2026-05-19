#include <Arduino.h>
#include <cstddef>          // Para size_t
#include "Notifigram.h"
#include "pantalla.h"       // ✅ Muy importante para tener TipoEfectoTexto y declaraciones

// Variable global del efecto actual
TipoEfectoTexto efectoActual = EFECTO_SIN_EFECTO;

// Mostrar texto con el efecto actual, forzando scroll si no cabe.
// Todas las call sites actuales (splash, ecos de comandos, mensajes de
// estado del setup) son ecos, asi que delegamos siempre con esEco=true.
// Los mensajes reales del usuario llaman a mostrarTextoConEfecto directo.
void mostrarTexto(const String& texto, uint16_t color, int delayScroll) {
  int textoPixeles = texto.length() * 6;  // 5 px por letra + 1 espacio
  int anchoPantalla = matrix.width();

  TipoEfectoTexto efectoUsado = (textoPixeles > anchoPantalla) ? EFECTO_SCROLL : efectoActual;
  mostrarTextoConEfecto(texto, color, efectoUsado, true);
}

// Devuelve el efecto desde un comando tipo /efecto_scroll
TipoEfectoTexto obtenerEfectoDesdeComando(const String& cmd) {
  if (cmd == "/efecto_sin_efecto") return EFECTO_SIN_EFECTO;
  if (cmd == "/efecto_scroll") return EFECTO_SCROLL;
  if (cmd == "/efecto_scroll_reversa") return EFECTO_SCROLL_REVERSA;
  if (cmd == "/efecto_rebote") return EFECTO_REBOTE;
  if (cmd == "/efecto_letra_por_letra") return EFECTO_LETRA_POR_LETRA;
  if (cmd == "/efecto_parpadeo") return EFECTO_PARPADEO;
  if (cmd == "/efecto_zoom") return EFECTO_ZOOM;
  if (cmd == "/efecto_fade") return EFECTO_FADE;
  if (cmd == "/efecto_arcoiris") return EFECTO_ARCOIRIS;
  if (cmd == "/efecto_wipe") return EFECTO_WIPE;
  if (cmd == "/efecto_agitacion") return EFECTO_AGITACION;
  if (cmd == "/efecto_pulsacion") return EFECTO_PULSACION;
  if (cmd == "/efecto_invertido") return EFECTO_INVERTIDO;
  return efectoActual;
}

// Devuelve el efecto desde nombre como "scroll"
TipoEfectoTexto obtenerEfectoDesdeNombre(const String& nombre) {
  if (nombre == "sin_efecto") return EFECTO_SIN_EFECTO;
  if (nombre == "scroll") return EFECTO_SCROLL;
  if (nombre == "scroll_reversa") return EFECTO_SCROLL_REVERSA;
  if (nombre == "rebote") return EFECTO_REBOTE;
  if (nombre == "letra_por_letra") return EFECTO_LETRA_POR_LETRA;
  if (nombre == "parpadeo") return EFECTO_PARPADEO;
  if (nombre == "zoom") return EFECTO_ZOOM;
  if (nombre == "fade") return EFECTO_FADE;
  if (nombre == "arcoiris") return EFECTO_ARCOIRIS;
  if (nombre == "wipe") return EFECTO_WIPE;
  if (nombre == "agitacion") return EFECTO_AGITACION;
  if (nombre == "pulsacion") return EFECTO_PULSACION;
  if (nombre == "invertido") return EFECTO_INVERTIDO;
  return EFECTO_SIN_EFECTO;
}

// Devuelve el nombre del efecto actual como texto
String nombreEfecto(TipoEfectoTexto efecto) {
  switch (efecto) {
    case EFECTO_SIN_EFECTO: return "sin_efecto";
    case EFECTO_SCROLL: return "scroll";
    case EFECTO_SCROLL_REVERSA: return "scroll_reversa";
    case EFECTO_REBOTE: return "rebote";
    case EFECTO_LETRA_POR_LETRA: return "letra_por_letra";
    case EFECTO_PARPADEO: return "parpadeo";
    case EFECTO_ZOOM: return "zoom";
    case EFECTO_FADE: return "fade";
    case EFECTO_ARCOIRIS: return "arcoiris";
    case EFECTO_WIPE: return "wipe";
    case EFECTO_AGITACION: return "agitacion";
    case EFECTO_PULSACION: return "pulsacion";
    case EFECTO_INVERTIDO: return "invertido";
    default: return "desconocido";
  }
}

void mostrarTextoConEfecto(const String& texto, uint16_t color, TipoEfectoTexto efecto, bool esEco) {
  if (esEco) ultimoEcoMs = millis();
  else       ultimoMensajeRealMs = millis();
  int textoPixeles = texto.length() * 6;
  int anchoPantalla = matrix.width();
  int altoPantalla = matrix.height();
  int y = (altoPantalla - 7) / 2;

  // Forzar scroll si el texto no cabe
  if (textoPixeles > anchoPantalla) {
    efecto = EFECTO_SCROLL;
  }

  matrix.fillScreen(0);  // Limpia antes de mostrar

  switch (efecto) {
    case EFECTO_SIN_EFECTO: {
      int x = (anchoPantalla - textoPixeles) / 2;
      matrix.setTextColor(color);
      matrix.setCursor(x, y);
      matrix.print(texto);
      matrix.show();
      break;
    }
    case EFECTO_SCROLL: {
      for (int x = anchoPantalla; x > -textoPixeles; x--) {
        matrix.fillScreen(0);
        matrix.setTextColor(color);
        matrix.setCursor(x, y);
        matrix.print(texto);
        matrix.show();
        delay(30);
      }
      break;
    }
    case EFECTO_SCROLL_REVERSA: {
      for (int x = -textoPixeles; x < anchoPantalla; x++) {
        matrix.fillScreen(0);
        matrix.setTextColor(color);
        matrix.setCursor(x, y);
        matrix.print(texto);
        matrix.show();
        delay(30);
      }
      break;
    }
    case EFECTO_REBOTE: {
      for (int rep = 0; rep < 2; rep++) {
        for (int x = 0; x < anchoPantalla - textoPixeles; x++) {
          matrix.fillScreen(0);
          matrix.setTextColor(color);
          matrix.setCursor(x, y);
          matrix.print(texto);
          matrix.show();
          delay(20);
        }
        for (int x = anchoPantalla - textoPixeles; x > 0; x--) {
          matrix.fillScreen(0);
          matrix.setTextColor(color);
          matrix.setCursor(x, y);
          matrix.print(texto);
          matrix.show();
          delay(20);
        }
      }
      break;
    }
    case EFECTO_LETRA_POR_LETRA: {
      matrix.fillScreen(0);
      for (size_t i = 0; i < texto.length(); i++) {
        matrix.setCursor(i * 6, y);
        matrix.setTextColor(color);
        matrix.print(texto[i]);
        matrix.show();
        delay(150);
      }
      break;
    }
    case EFECTO_PARPADEO: {
      for (int i = 0; i < 6; i++) {
        matrix.fillScreen(i % 2 == 0 ? color : 0);
        matrix.setCursor(0, y);
        matrix.setTextColor(i % 2 == 0 ? 0 : color);
        matrix.print(texto);
        matrix.show();
        delay(200);
      }
      break;
    }
    case EFECTO_ZOOM: {
      for (int i = 0; i < 3; i++) {
        matrix.fillScreen(0);
        matrix.setCursor(2, y);
        matrix.setTextColor(matrix.Color(255, 255, 255));
        matrix.print(texto);
        matrix.show();
        delay(100);

        matrix.fillScreen(0);
        matrix.setCursor(0, y - 1); // Simula acercamiento
        matrix.setTextColor(matrix.Color(255, 255, 0));
        matrix.print(texto);
        matrix.show();
        delay(100);
      }
      break;
    }
    case EFECTO_FADE: {
      for (int b = 10; b <= 255; b += 10) {
        matrix.setBrightness(b);
        matrix.fillScreen(0);
        matrix.setTextColor(color);
        matrix.setCursor(0, y);
        matrix.print(texto);
        matrix.show();
        delay(40);
      }
      break;
    }
    case EFECTO_ARCOIRIS: {
      for (int frame = 0; frame < 50; frame++) {
        matrix.fillScreen(0);
        for (int i = 0; i < texto.length(); i++) {
          int x = i * 6;
          uint16_t col = matrix.ColorHSV(((x + frame * 3) * 65536L) / anchoPantalla, 255, 255);
          matrix.setTextColor(col);
          matrix.setCursor(x, y);
          matrix.print(texto[i]);
        }
        matrix.show();
        delay(50);
      }
      break;
    }
    case EFECTO_WIPE: {
      for (int i = 0; i <= texto.length(); i++) {
        matrix.fillScreen(0);
        matrix.setTextColor(color);
        matrix.setCursor(0, y);
        matrix.print(texto.substring(0, i));
        matrix.show();
        delay(100);
      }
      break;
    }
    case EFECTO_AGITACION: {
      for (int i = 0; i < 10; i++) {
        int offsetX = random(-2, 3);
        int offsetY = random(-1, 2);
        matrix.fillScreen(0);
        matrix.setCursor(offsetX, y + offsetY);
        matrix.setTextColor(color);
        matrix.print(texto);
        matrix.show();
        delay(80);
      }
      break;
    }
    case EFECTO_PULSACION: {
      for (int i = 0; i < 3; i++) {
        matrix.setBrightness(50);
        matrix.fillScreen(0);
        matrix.setCursor(0, y);
        matrix.setTextColor(color);
        matrix.print(texto);
        matrix.show();
        delay(150);
        matrix.setBrightness(255);
        matrix.show();
        delay(150);
      }
      break;
    }
    case EFECTO_INVERTIDO: {
      matrix.fillScreen(color);
      matrix.setTextColor(0);
      matrix.setCursor(0, y);
      matrix.print(texto);
      matrix.show();
      break;
    }
    default: {
      int x = (anchoPantalla - textoPixeles) / 2;
      matrix.setTextColor(color);
      matrix.setCursor(x, y);
      matrix.print(texto);
      matrix.show();
      break;
    }
  }
}

void actualizarReloj() {
  if (relojDesactivado) return;
  if (!ntpSincronizado || !wifiActivo) return;
  unsigned long ahora = millis();
  if ((ahora - ultimoMensajeRealMs) < tiempoSalvapantallasMs) return;
  if ((ahora - ultimoEcoMs) < HOLD_ECO_MS) return;

  static int ultimoMinutoPintado = -1;
  static unsigned long ultimoBumpeoRealVisto = 0;
  static unsigned long ultimoBumpeoEcoVisto = 0;
  if (ultimoMensajeRealMs != ultimoBumpeoRealVisto || ultimoEcoMs != ultimoBumpeoEcoVisto) {
    ultimoMinutoPintado = -1;
    ultimoBumpeoRealVisto = ultimoMensajeRealMs;
    ultimoBumpeoEcoVisto = ultimoEcoMs;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  if (timeinfo.tm_min == ultimoMinutoPintado) return;

  char dh1 = '0' + (timeinfo.tm_hour / 10);
  char dh2 = '0' + (timeinfo.tm_hour % 10);
  char dm1 = '0' + (timeinfo.tm_min / 10);
  char dm2 = '0' + (timeinfo.tm_min % 10);

  int x = (matrix.width() - 26) / 2;
  int y = (matrix.height() - 7) / 2;

  matrix.fillScreen(0);
  matrix.setTextColor(textoColor);
  matrix.setCursor(x, y);      matrix.write(dh1);
  matrix.setCursor(x + 6, y);  matrix.write(dh2);
  matrix.drawPixel(x + 12, y + 2, textoColor);
  matrix.drawPixel(x + 12, y + 5, textoColor);
  matrix.drawPixel(x + 13, y + 2, textoColor);
  matrix.drawPixel(x + 13, y + 5, textoColor);
  matrix.setCursor(x + 15, y); matrix.write(dm1);
  matrix.setCursor(x + 21, y); matrix.write(dm2);
  matrix.show();

  ultimoMinutoPintado = timeinfo.tm_min;
}
