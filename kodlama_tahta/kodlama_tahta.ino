#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <vector> // Vektör kullanımı için


/* Wi-Fi ve Firebase Tanımları */
#define WIFI_SSID "SAİM"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyDk7511OqPNNTwB-o1VXYVGCQ8l8dLsUBM"
#define DATABASE_URL "https://loginproject-19ca4-default-rtdb.firebaseio.com/"
#define USER_EMAIL "admin@admin.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Blokların GPIO pinleri
#define FORWARD1_PIN 21
#define FORWARD2_PIN 19
#define BACKWARD_PIN 22
#define BACKWARD2_PIN 18
#define RIGHT_TURN_PIN 32
#define RIGHT_TURN_PIN2 14
#define LEFT_TURN_PIN 33
#define HORN_PIN 25
#define HEADLIGHT_PIN 23

// Durum değişkenleri
std::vector<String> sentBlocks; // Firebase'e gönderilen blokları saklamak için
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200; // 200 ms debounce gecikmesi

// Firebase veri gönderme fonksiyonu
void sendCommandToFirebase(String path, int state) {
  if (Firebase.RTDB.setInt(&fbdo, path, state)) {
    Serial.print("Veri gönderildi -> ");
    Serial.print(path);
    Serial.print(": ");
    Serial.println(state);
  } else {
    Serial.print("Hata: ");
    Serial.println(fbdo.errorReason());
  }
}

// Takılan blokları kontrol et ve Firebase'e veri gönder
void checkAndSendBlocks() {
  unsigned long currentMillis = millis();

  if ((currentMillis - lastDebounceTime) > debounceDelay) { // Debounce süresi kontrolü
    // Blokları kontrol et
    if (!digitalRead(FORWARD1_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "forward1") == sentBlocks.end()) {
      Serial.println("İleri git 1 bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/forward1", 1);
      sentBlocks.push_back("forward1");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(FORWARD2_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "forward2") == sentBlocks.end()) {
      Serial.println("İleri git 2 bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/forward2", 1);
      sentBlocks.push_back("forward2");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(BACKWARD_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "backward") == sentBlocks.end()) {
      Serial.println("Geri git 1 bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/backward", 1);
      sentBlocks.push_back("backward");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(BACKWARD2_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "backward2") == sentBlocks.end()) {
      Serial.println("Geri git 2 bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/backward2", 1);
      sentBlocks.push_back("backward2");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(RIGHT_TURN_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "right") == sentBlocks.end()) {
      Serial.println("Sağa dön bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/right", 1);
      sentBlocks.push_back("right");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(RIGHT_TURN_PIN2) && std::find(sentBlocks.begin(), sentBlocks.end(), "right2") == sentBlocks.end()) {
      Serial.println("Yeni sağa dön bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/right2", 1);
      sentBlocks.push_back("right2");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(LEFT_TURN_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "left") == sentBlocks.end()) {
      Serial.println("Sola dön bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/left", 1);
      sentBlocks.push_back("left");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(HORN_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "horn") == sentBlocks.end()) {
      Serial.println("Korna çal bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/horn", 1);
      sentBlocks.push_back("horn");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
    if (!digitalRead(HEADLIGHT_PIN) && std::find(sentBlocks.begin(), sentBlocks.end(), "headlights") == sentBlocks.end()) {
      Serial.println("Far yak bloğu takıldı");
      sendCommandToFirebase("/kotlamaTahta/headlights", 1);
      sentBlocks.push_back("headlights");
      lastDebounceTime = currentMillis; // Debounce zamanını güncelle
    }
  }
}

// Wi-Fi bağlantısını kontrol et ve yeniden bağlan
void ensureWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi bağlantısı kaybedildi! Yeniden bağlanıyor...");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi bağlantısı sağlanamadı. 5 saniye sonra yeniden denenecek.");
      delay(5000);
    } else {
      Serial.println("\nWi-Fi bağlantısı tekrar kuruldu.");
      WiFi.setSleep(false);
      Serial.println("Wi-Fi güç tasarrufu modu devre dışı bırakıldı.");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Blok pinlerini giriş olarak ayarla
  pinMode(FORWARD1_PIN, INPUT_PULLUP);
  pinMode(FORWARD2_PIN, INPUT_PULLUP);
  pinMode(BACKWARD_PIN, INPUT_PULLUP);
  pinMode(BACKWARD2_PIN, INPUT_PULLUP);
  pinMode(RIGHT_TURN_PIN, INPUT_PULLUP);
  pinMode(RIGHT_TURN_PIN2, INPUT_PULLUP);
  pinMode(LEFT_TURN_PIN, INPUT_PULLUP);
  pinMode(HORN_PIN, INPUT_PULLUP);
  pinMode(HEADLIGHT_PIN, INPUT_PULLUP);

  // Wi-Fi bağlantısı
  Serial.print("Wi-Fi'ye bağlanılıyor...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print("...");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi bağlantısı sağlanamadı!");
  } else {
    Serial.println("\nWi-Fi bağlantısı kuruldu.");
    Serial.println("IP Adresi: " + WiFi.localIP().toString());
    WiFi.setSleep(false);
    Serial.println("Wi-Fi güç tasarrufu modu devre dışı bırakıldı.");
  }

  // Firebase ayarları
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);
  Serial.println("Firebase bağlantısı kuruldu.");
}

void loop() {
  ensureWiFiConnection();
  checkAndSendBlocks();
  delay(500); // Döngüde daha fazla gecikme olmaması için
}
