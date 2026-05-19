#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

bool cargarConfig();
void guardarConfig();

extern String nombreColorActual;
extern String nivelBrilloActual;

#endif
