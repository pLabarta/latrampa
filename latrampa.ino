#include <WiFi.h>
#include <WebServer.h>

// Config de WiFi
const char* ssid = "ssid de tu red";
const char* password = "contraseña";

// Config pines de las luces
int pinR = 23;  // Canal R (Rojo)
int pinG = 22;  // Canal G (Verde)
int pinB = 21;  // Canal B (Azul)

// Config para las secuencias
int state;
//// Control de los tiempos
unsigned long previousMillis = 0;
unsigned long interval = 50; // Interval for updating brightness (milliseconds)

// Variables del stand-by
float brightness = 0;   // Current brightness level
float fadeAmount = 0.1; // Amount to change brightness (speed of breathing)

// Variables especificas de los efectos de las herramientas
// Taladro: strobe
int strobeCount = 0;         // Counts the number of strobe flashes
// Sierra
unsigned long rampStartMillis = 0; // Start time for the sierra effect

WebServer server(80);

const int ledPin = 23;

void setup() {
  // Configuración inicial
  state = 0;
  Serial.begin(115200);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // Inicializar la conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Handlers de las rutas HTTP
  server.on("/led/taladro", []() {
    state = 1; // Switch to strobe state
    server.send(200, "text/plain", "Taladro mode activated");
  });
  server.on("/led/sierra", []() {
    state = 2; // Switch to sierra state
    rampStartMillis = millis(); // Record the start time of the ramp
    server.send(200, "text/plain", "Sierra mode activated");
  });
  server.on("/led/martillo", []() {
    state = 3; // Switch to sierra state
    rampStartMillis = millis(); // Record the start time of the ramp
    server.send(200, "text/plain", "Martillo mode activated");
  });

  // Inciar el servidor
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Consultar ultimas request HTTP
  server.handleClient();
  // Actualizar el estado de las luces
  updateRgb();
}

// Gestión de estados de las luces
void updateRgb() {
  if (state == 0) {
    standBy();
  } else if (state == 1) {
    taladro(); // Strobe effect
  } else if (state == 2) {
    sierra();
  } else if (state == 3) {
    martillo();
  }
}

void standBy() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Calcular el brillo de la luz basado en una sinusoide
    float sineWave = (sin(brightness) + 1.0) / 2.0; // Scaled to 0-1 range
    int redBrightness = int(sineWave * 255);
    int greenBrightness = int(sineWave * 255 * 0.7); // Slightly less green for a warmer yellow
    int blueBrightness = int(sineWave * 255 * 0.1);

    // Establecer el color de la luz
    setColor(redBrightness, greenBrightness, blueBrightness);

    // Incrementar el brillo para la siguiente iteración
    brightness += fadeAmount;

    // Reiniciar el ciclo si se alcanza el máximo
    if (brightness > TWO_PI) {
      brightness = 0;
    }
  }
}

// Efecto del taladro
void taladro() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 100) { // 100ms de intervalo
    previousMillis = currentMillis;

    if (strobeCount % 2 == 0) {
      setColor(255, 255, 255); // Blanco
    } else {
      setColor(255, 0, 0); // Rojo
    }

    strobeCount++;

    if (strobeCount >= 10) { // Luego de 10 ciclos, regresar al estado de espera
      state = 0;
      strobeCount = 0;
    }
  }
}

// Efecto de la sierra
void sierra() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - rampStartMillis;
  const rampDur = 666; // Duración de la rampa: 666ms

  if (elapsedMillis <= rampDur) { 
    float rampProgress = elapsedMillis / rampDur; // Normalizar a 0-1
    int brightness = int(rampProgress * 255); // Escalar a 0-255

    setColor(brightness, brightness, brightness);
  } else {
    state = 0;
  }
}

// Efecto del martillo
void martillo() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - rampStartMillis;
  const rampDur = 400; // Duración de la rampa: 400ms

  if (elapsedMillis <= rampDur) {
    float rampProgress = 1.0 - (elapsedMillis / rampDur);
    int brightnessR = int(rampProgress * 255);
    int brightnessG = int(rampProgress * 255 * 0.1);
    int brightnessB = int(rampProgress * 255);

    setColor(brightnessR, brightnessG, brightnessB);
  } else {
    state = 0;
  }
}

// Funcion para establecer el color de las luces
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(pinR, redValue);
  analogWrite(pinG, greenValue);
  analogWrite(pinB, blueValue);
}