#include "Notifigram.h"

extern void procesarTelegram();
extern void inicializarContadorYTiempo();
extern void actualizarReloj();


void setup() {
  inicializarContadorYTiempo();
  iniciarSistema();
  randomSeed(analogRead(36));
}

void loop() {
  procesarBluetooth();   // Por si recibes datos desde la app
  procesarTelegram();    // Para escuchar y responder mensajes de Telegram
  actualizarReloj();     // Reloj NTP HH:MM cuando hay hueco
}

