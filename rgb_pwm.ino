// Pines de salida para la tira LED RGB
const int redPin = 9;    // Pin PWM para el color rojo
const int greenPin = 10;  // Pin PWM para el color verde
const int bluePin = 11;   // Pin PWM para el color azul
const int ldrPin = A0;    // Pin analógico para el LDR

// Variables para manejar el ciclo de colores
int redValue = 255;
int greenValue = 0;
int blueValue = 0;

int state = 0;  // Estado del ciclo de colores
unsigned long previousMillis = 0; // Almacena el último tiempo de actualización
int stepDelay = 1500;  // Tiempo de delay base para cada paso (lento al principio)

void setup() {
  // Configura los pines como salida
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(9600); // Para monitorear el valor del LDR (opcional)
}

void loop() {
  // Leer el valor del LDR
  int ldrValue = analogRead(ldrPin);

  // Mapeamos el valor del LDR para ajustar la velocidad
  // 1000 ms a poca luz, 0.5 ms a mucha luz
  stepDelay = map(ldrValue, 0, 1023, 1500, 0.01); // Ajustamos el mínimo a 1 ms

  // Verificar si es tiempo de actualizar el color
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= stepDelay) {
    previousMillis = currentMillis; // Guarda el tiempo actual

    // Transiciones automáticas entre los colores
    if (state == 0) {  // De rojo a amarillo
      greenValue += 20; // Aumentar más rápido
      if (greenValue >= 255) {
        greenValue = 255;
        state = 1;  // Pasar al siguiente estado
      }
    } else if (state == 1) {  // De amarillo a verde
      redValue -= 20; // Disminuir más rápido
      if (redValue <= 0) {
        redValue = 0;
        state = 2;
      }
    } else if (state == 2) {  // De verde a cian
      blueValue += 20; // Aumentar más rápido
      if (blueValue >= 255) {
        blueValue = 255;
        state = 3;
      }
    } else if (state == 3) {  // De cian a azul
      greenValue -= 20; // Disminuir más rápido
      if (greenValue <= 0) {
        greenValue = 0;
        state = 4;
      }
    } else if (state == 4) {  // De azul a magenta
      redValue += 20; // Aumentar más rápido
      if (redValue >= 255) {
        redValue = 255;
        state = 5;
      }
    } else if (state == 5) {  // De magenta a rojo
      blueValue -= 20; // Disminuir más rápido
      if (blueValue <= 0) {
        blueValue = 0;
        state = 0;  // Volver al primer estado
      }
    }

    // Enviar los valores PWM a los pines RGB
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
    
    // Monitorear el valor del LDR (opcional)
    Serial.print("LDR Value: ");
    Serial.println(ldrValue); // LDR value
  }
}
