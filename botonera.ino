#include <Arduino.h>
#include <KeyboardDevice.h>
#include <BleCompositeHID.h>

#define MODKEY_RANGE_MIN KEY_MOD_LALT
#define MODKEY_RANGE_MAX KEY_MOD_RMETA

int ROJO_PIN = 23;
int AZUL_PIN = 22;
int VERD_PIN = 19;
int AMAR_PIN = 18;

BleCompositeHID compositeHID("ESP32 Keyboard", "Mystfit", 100);
KeyboardDevice* keyboard;

void setup()
{
    Serial.begin(115200);
    // Set up the button pin
    pinMode(ROJO_PIN, INPUT);
    pinMode(AZUL_PIN, INPUT);
    pinMode(VERD_PIN, INPUT);
    pinMode(AMAR_PIN, INPUT);
    keyboard = new KeyboardDevice();
    compositeHID.addDevice(keyboard);
    compositeHID.begin();

    Serial.println("Waiting for connection");
    delay(3000);
}

void loop()
{
    // Verificar que la botonera esté conectada
    if (compositeHID.isConnected())
    {
        press(ROJO_PIN, 'S');
        press(AMAR_PIN, 'D');
        press(AZUL_PIN, 'W');
        press(VERD_PIN, 'A');
    }
}


void press(int pin, char letra) {
    if (digitalRead(pin) == HIGH) {
        // Determinar qué boton se pulsó
        uint8_t keycode;

        switch (letra) {
            case 'W': keycode = 0x1A; break; // Keycode para 'W'
            case 'A': keycode = 0x04; break; // Keycode para 'A'
            case 'S': keycode = 0x16; break; // Keycode for 'S'
            case 'D': keycode = 0x07; break; // Keycode for 'D'
            default: return; // Si recibe ota letra, no hace nada
        }

        // Simular el pulsado de la tecla
        keyboard->keyPress(keycode);    // Apretar
        delay(10);                      // Delay
        keyboard->keyRelease(keycode);  // Soltar
        delay(10);                      // Delay

        // Esperar que el botón deje de estar apretado
        while (digitalRead(pin) == HIGH) {
            delay(10); // Delay de la espera
        }

        delay(100); // Intervalo mínimo entre pulsaciones
    }
}


