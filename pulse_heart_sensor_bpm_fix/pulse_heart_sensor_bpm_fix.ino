#include <WiFi.h>
#include <FirebaseESP32.h>


const int PulseSensorHRWire = A0;
int UpperThreshold = 540;
int LowerThreshold = 520;

const char* ssid     = "ASD";               //--> Nama Wifi
const char* password = "123123123";         //--> Password Wifi

const char* firebaseHost = "https://pulse-sensor-c1e80-default-rtdb.firebaseio.com/";  //--> Firebase Hostname
const char* firebaseAuth = "tqyloe8eV9rHOjW7yWrpDN1GoHHW8kWlO2Lkst2Z";  //--> Firebase Authentication Token

unsigned long previousMillisGetHR = 0;
unsigned long previousMillisResultHR = 0;
const long intervalGetHR = 20; 
const long intervalResultHR = 10000;
int PulseSensorSignal;

int cntHB = 0;
boolean ThresholdStat = true;
int BPMval = 0;

FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(firebaseHost, firebaseAuth);
}

void loop() {
  GetHeartRate();
}

void GetHeartRate() {
  //----------------------------------------proses pembacaan.
  unsigned long currentMillisGetHR = millis();

  if (currentMillisGetHR - previousMillisGetHR >= intervalGetHR) {
    previousMillisGetHR = currentMillisGetHR;

    PulseSensorSignal = analogRead(PulseSensorHRWire);

    if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
      cntHB++;
      ThresholdStat = false;
      Serial.println("Beat Detected!");
      Serial.print("Beat : ");
      Serial.println(cntHB);
    }

    if (PulseSensorSignal < LowerThreshold) {
      ThresholdStat = true;
    }
  }
  //----------------------------------------proses meubah dari pembacaan ke nilai bpm
  unsigned long currentMillisResultHR = millis();

  if (currentMillisResultHR - previousMillisResultHR >= intervalResultHR) {
    previousMillisResultHR = currentMillisResultHR;

    BPMval = cntHB * 6;
    cntHB = 0;
    Serial.print("BPM : ");
    Serial.println(BPMval);

    // Kirim data BPM ke Firebase
    Firebase.setInt(firebaseData, "bpm", BPMval);
  }
}