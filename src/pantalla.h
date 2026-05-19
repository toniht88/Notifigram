#ifndef PANTALLA_H
#define PANTALLA_H

#include <Arduino.h>
#include "Notifigram.h"  // Aquí está el enum TipoEfectoTexto y matrix

// Mostrar texto usando el efecto actual global
void mostrarTexto(const String& texto, uint16_t color = 0xFFFF, int delayScroll = 60);

// Mostrar texto con un efecto específico
void mostrarTextoConEfecto(const String& texto, uint16_t color, TipoEfectoTexto efecto, bool esEco);

// Obtener efecto desde un comando como /efecto_scroll
TipoEfectoTexto obtenerEfectoDesdeComando(const String& cmd);

// Obtener efecto desde un nombre simple ("scroll", "rebote", etc.)
TipoEfectoTexto obtenerEfectoDesdeNombre(const String& nombre);

// Obtener el nombre del efecto desde su enum
String nombreEfecto(TipoEfectoTexto efecto);

#endif  // PANTALLA_H
