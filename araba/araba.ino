#include <WiFi.h>
#include <FirebaseESP32.h>

// Wi-Fi Ayarları
const char* WIFI_SSID = "SAİM";
const char* WIFI_PASSWORD = "12345678";

// Firebase Ayarları
const char* API_KEY = "AIzaSyDk7511OqPNNTwB-o1VXYVGCQ8l8dLsUBM"; 
const char* DATABASE_URL = "https://loginproject-19ca4-default-rtdb.firebaseio.com/";
const char* USER_EMAIL = "admin@admin.com";
const char* USER_PASSWORD = "123456";

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Motor, Buzzer ve LED Pinleri
const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 25;
const int IN4 = 27;
const int BUZZER_PIN = 14;
const int HEADLIGHTS_PIN = 26; // Headlights için LED pini
int forward1 = 0, forward2 = 0, backward2 = 0, left2 = 0, right2 = 0, backward = 0, left = 0, right = 0, horn = 0, headlights = 0;

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(HEADLIGHTS_PIN, OUTPUT); // Headlights pinini çıkış birimi olarak ayarla
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wi-Fi'ye bağlanılıyor");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wi-Fi'ye bağlandı");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  

  if (Firebase.getInt(firebaseData, "/araba/forward1")) {
    forward1 = firebaseData.intData();
  }
    if (Firebase.getInt(firebaseData, "/araba/forward2")) {
    forward2 = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/backward")) {
    backward = firebaseData.intData();
  }
    if (Firebase.getInt(firebaseData, "/araba/backward2")) {
    backward2 = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/left")) {
    left = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/left2")) {
    left2 = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/right")) {
    right = firebaseData.intData();
  }
   if (Firebase.getInt(firebaseData, "/araba/right2")) {
    right2 = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/horn")) {
    horn = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/headlights")) {
    headlights = firebaseData.intData();
  }

  if (forward1 == 1) {
    ileri();
    delay(5000); // 5 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/forward1", 0);
  } else if (forward2 == 1) {
    ileri();
    delay(5000); // 5 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/forward2", 0);
  }
    else if (backward == 1) {
    geri();
    delay(5000); // 5 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/backward", 0);
  } else if (backward2 == 1) {
    geri();
    delay(5000); // 5 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/backward2", 0);}
  else if (left == 1) {
    sola();
    delay(720); // 2 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/left", 0);
  } else if (left2 == 1) {
    sola();
    delay(720); // 2 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/left2", 0);
  }else if (right == 1) {
    saga();
    delay(720); // 2 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/right", 0);
  }else if (right2 == 1) {
    saga();
    delay(720); // 2 saniye bekle
    dur();
    Firebase.setInt(firebaseData, "/araba/right2", 0);
  } else {
    dur();
  }

  if (horn == 1) {
    kornaCal();
    delay(1000);
    kornaSus();
    Firebase.setInt(firebaseData, "/araba/horn", 0); // Firebase'de sıfırla
  } else {
    kornaSus();
  }

  if (headlights == 1) {
    acHeadlights();
    delay(10000);
    kapatHeadlights();
    Firebase.setInt(firebaseData, "/araba/headlights", 0); // Firebase'de sıfırla
  } else {
    kapatHeadlights();
  }

  Serial.println("Komutlar güncellendi");
  delay(1000); // 1 saniye bekle
}

void ileri() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void geri() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void saga() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void sola() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void dur() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void kornaCal() {
  tone(BUZZER_PIN, 1000); // Frekans değeri ayarlandı
  delay(500);             // Ses süresi ayarlandı
  noTone(BUZZER_PIN);
}

void kornaSus() {
  noTone(BUZZER_PIN);
}

void acHeadlights() {
  digitalWrite(HEADLIGHTS_PIN, HIGH);
}

void kapatHeadlights() {
  digitalWrite(HEADLIGHTS_PIN, LOW);
}