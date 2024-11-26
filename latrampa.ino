#include <WiFi.h>
#include <WebServer.h>

// Config de WiFi
const char* ssid = "Internet-2.4Ghz";
const char* password = "int3rn3t";

// Config pines de las luces
int pinR = 23;  // Canal R (Rojo)
int pinG = 22;  // Canal G (Verde)
int pinB = 21;  // Canal B (Azul)

// Config para las secuencias
int state;
// Variables for timing
unsigned long previousMillis = 0;
unsigned long interval = 50; // Interval for updating brightness (milliseconds)

// Breathing effect variables
float brightness = 0;   // Current brightness level
float fadeAmount = 0.1; // Amount to change brightness (speed of breathing)

// Taladro
// State variables
int strobeCount = 0;         // Counts the number of strobe flashes

// Sierra
unsigned long rampStartMillis = 0; // Start time for the sierra effect

WebServer server(80);

const int ledPin = 23;

void setup() {
  state = 0;
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

   // Taladro
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

  server.on("/led/off", []() {
    digitalWrite(ledPin, LOW); // Turn the LED off
    server.send(200, "text/plain", "LED is OFF");
  });

  // Start the server
  server.begin();
  Serial.println("Server started");

  
}

void loop() {
  // put your main code here, to run repeatedly:
  // Handle client requests
  server.handleClient();
  // updateRgb();
  // setColor(255,122,122);
  // analogWrite(pinG, greenValue);
  updateRgb();
  // Serial.println(millis());
}

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

    // Calculate brightness using a sine wave-like function
    float sineWave = (sin(brightness) + 1.0) / 2.0; // Scaled to 0-1 range
    int redBrightness = int(sineWave * 255);
    int greenBrightness = int(sineWave * 255 * 0.7); // Slightly less green for a warmer yellow
    int blueBrightness = int(sineWave * 255 * 0.1);

    // Set the LED color
    setColor(redBrightness, greenBrightness, blueBrightness);

    // Increment brightness
    brightness += fadeAmount;

    // Reset the cycle after a full wave
    if (brightness > TWO_PI) {
      brightness = 0;
    }
  }
}

// Strobe effect for taladro
void taladro() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 100) { // 100 ms strobe interval
    previousMillis = currentMillis;

    if (strobeCount % 2 == 0) {
      setColor(255, 255, 255); // White strobe
    } else {
      setColor(255, 0, 0); // Off
    }

    strobeCount++;

    if (strobeCount >= 10) { // After 5 strobes, return to standby
      state = 0;
      strobeCount = 0;
    }
  }
}

// Increasing ramp effect for sierra
void sierra() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - rampStartMillis;

  if (elapsedMillis <= 666) { // Ramp duration: 1 second
    float rampProgress = elapsedMillis / 666.0; // Normalize to 0-1
    int brightness = int(rampProgress * 255); // Scale to 0-255

    setColor(brightness, brightness, brightness); // White light ramp
  } else {
    state = 0; // Return to standby after the ramp
  }
}

// Decreasing ramp effect for martillo
void martillo() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - rampStartMillis;

  if (elapsedMillis <= 400) { // Ramp duration: 1 second
    float rampProgress = 1.0 - (elapsedMillis / 400.0); // Normalize to 1-0
    int brightnessR = int(rampProgress * 255);
    int brightnessG = int(rampProgress * 255 * 0.1);
    int brightnessB = int(rampProgress * 255); // Scale to 0-255

    setColor(brightnessR, brightnessG, brightnessB); // White light ramp
  } else {
    state = 0; // Return to standby after the ramp
  }
}

// void efectoTaladro() {
//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= 20) { // Intervalo de 20 ms para destellos rápidos
//     previousMillis = currentMillis;
//     static bool toggle = false;
//     toggle = !toggle; // Alternar estado
//     if (toggle) {
//       analogWrite(pinR, 255);
//       analogWrite(pinG, 255);
//       analogWrite(pinB, 255); // Blanco puro
//     } else {
//       turnOff(); // Apagar brevemente para crear destellos
//     }
//   }
//   isLedOn = true;
// }

// Function to set the RGB LED color
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(pinR, redValue);
  analogWrite(pinG, greenValue);
  analogWrite(pinB, blueValue);
}