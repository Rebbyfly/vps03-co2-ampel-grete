#include <MHZ19.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

// LED-Pins
const int gPinRot1 = 26;
const int gPinRot2 = 27;
const int gPinRot3 = 14;
const int gPinRot4 = 13;
const int gPinGelb1 = 4;
const int gPinGelb2 = 32;
const int gPinGelb3 = 33;
const int gPinGelb4 = 25;
const int gPinGruen1 = 21;
const int gPinGruen2 = 19;
const int gPinGruen3 = 18;
const int gPinGruen4 = 5;

const int gLeds[12] = {
  gPinRot4,
  gPinRot3,
  gPinRot2,
  gPinRot1,
  gPinGelb4,
  gPinGelb3,
  gPinGelb2,
  gPinGelb1,
  gPinGruen4,
  gPinGruen3,
  gPinGruen2,
  gPinGruen1
};

// UART für MH-Z19C
const int RX_PIN = 16;  // RX ESP <- TX Sensor
const int TX_PIN = 17;  // TX ESP -> RX Sensor

char* ssid = "";
char* password = "";


MHZ19 co2Sensor;
HardwareSerial mhzSerial(1);  // UART1 für den CO2-Sensor

void schalteAlleLedsAus() {
  for (int pin : gLeds) {
    digitalWrite(pin, LOW);
  }
}

void zeigeFehlerAn(int times = 1) {
  for(int i = 0; i < times; i++) {
    digitalWrite(gPinRot1, HIGH);
    digitalWrite(gPinRot2, HIGH);
    digitalWrite(gPinRot3, HIGH);
    digitalWrite(gPinRot4, HIGH);
    delay(500);
    schalteAlleLedsAus();
  }
}

void connectWifi() {
  if(WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    zeigeFehlerAn();
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

void setup() {

  Serial.begin(115200);
  connectWifi();
  mhzSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  co2Sensor.begin(mhzSerial);
  co2Sensor.autoCalibration(false);  // optional: automatische Kalibrierung deaktivieren
  // LED-Pins initialisieren
  for (int pin : gLeds) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Serial.println(pin);
    delay(300);
    digitalWrite(pin, LOW);
  }

  Serial.println("CO₂-Sensor gestartet.");
}

void sprecheZugehoerigeLedBeiWertAn(int pPPM) {
  schalteAlleLedsAus();

  if(pPPM < 600) {
    digitalWrite(gPinGruen1, HIGH);
  } else if(pPPM < 700) {
    digitalWrite(gPinGruen2, HIGH);
  } else if(pPPM < 800) {
    digitalWrite(gPinGruen3, HIGH);
  } else if(pPPM < 900) {
    digitalWrite(gPinGruen4, HIGH);
  } else if(pPPM < 1000) {
    digitalWrite(gPinGelb1, HIGH);
  } else if(pPPM < 1100) {
    digitalWrite(gPinGelb2, HIGH);
  } else if(pPPM < 1200) {
    digitalWrite(gPinGelb3, HIGH);
  } else if(pPPM < 1300) {
    digitalWrite(gPinGelb4, HIGH);
  } else if(pPPM < 1400) {
    digitalWrite(gPinRot1, HIGH);
  } else if(pPPM < 1600) {
    digitalWrite(gPinRot2, HIGH);
  } else if(pPPM < 1800) {
    digitalWrite(gPinRot3, HIGH);
  } else {
    digitalWrite(gPinRot4, HIGH);
  }
}

void loop() {
  connectWifi(); //connectWifi() stops if already connected

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://vm-l-vps03.leon-malte.de:3000/getBoolean");

    int code = http.GET();

    if(code > 0) {
      String payload = http.getString(); //true oder false
      if(payload == "true") {
        int vAktuellerCo2Wert = co2Sensor.getCO2();
        if(vAktuellerCo2Wert > 0) {
          Serial.print("CO₂: ");
          Serial.print(vAktuellerCo2Wert);
          Serial.println(" ppm");
          sprecheZugehoerigeLedBeiWertAn(vAktuellerCo2Wert);
        } else {
          Serial.print("Sensorfehler. Fehlercode: ");
          zeigeFehlerAn(2);
          delay(3000);
        }
      } else {
        schalteAlleLedsAus();
      }
    } else {
      zeigeFehlerAn(3);
    }
    http.end();
  }

  
  delay(5000);  // 5 Sekunden Pause (Alle 5 Sekunden wird Anzeige aktualisiert)
}
