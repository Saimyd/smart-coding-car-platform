/*
 * Smart Coding Car Platform - Kodlama Tahtası Modülü
 * 
 * Bu kod, fiziksel kodlama bloklarını algılayan ESP32 tabanlı kodlama tahtasını kontrol eder.
 * Kullanıcılar fiziksel blokları tahtaya takarak arabanın hareketlerini programlayabilir.
 * 
 * Özellikler:
 * - Fiziksel blok algılama (9 farklı komut)
 * - Wi-Fi bağlantısı ve otomatik yeniden bağlanma
 * - Firebase ile gerçek zamanlı veri gönderimi
 * - Debounce koruması (yanlış algılamayı önler)
 * - Çoklu komut desteği
 * 
 * Geliştirici: Smart Coding Car Platform
 * Tarih: 2024
 */

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <vector> // Gönderilen blokları takip etmek için vektör kullanımı

/* Wi-Fi Ağ Ayarları - Kendi ağ bilgilerinizi girin */
#define WIFI_SSID "YOUR_WIFI_NAME"      // Wi-Fi ağ adı
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"  // Wi-Fi şifresi

/* Firebase Proje Ayarları - Kendi Firebase bilgilerinizi girin */
#define API_KEY "YOUR_FIREBASE_API_KEY"  // Firebase API anahtarı
#define DATABASE_URL "https://YOUR_PROJECT_ID-default-rtdb.firebaseio.com/"  // Firebase database URL'i
#define USER_EMAIL "your_email@example.com"     // Firebase kullanıcı email'i
#define USER_PASSWORD "your_password"           // Firebase kullanıcı şifresi

// Firebase nesneleri
FirebaseData fbdo;      // Firebase veri alışverişi için
FirebaseAuth auth;      // Firebase kimlik doğrulama için
FirebaseConfig config;  // Firebase yapılandırma için

// Fiziksel Kodlama Bloklarının GPIO Pin Tanımlamaları
#define FORWARD1_PIN 21      // İleri git bloğu 1
#define FORWARD2_PIN 19      // İleri git bloğu 2
#define BACKWARD_PIN 22      // Geri git bloğu 1
#define BACKWARD2_PIN 18     // Geri git bloğu 2
#define RIGHT_TURN_PIN 32    // Sağa dön bloğu 1
#define RIGHT_TURN_PIN2 14   // Sağa dön bloğu 2
#define LEFT_TURN_PIN 33     // Sola dön bloğu
#define HORN_PIN 25          // Korna bloğu
#define HEADLIGHT_PIN 23     // Far bloğu

// Sistem Durum Değişkenleri
std::vector<String> sentBlocks;     // Firebase'e gönderilen blokları takip etmek için
unsigned long lastDebounceTime = 0; // Son debounce zamanı
unsigned long debounceDelay = 200;   // 200ms debounce gecikmesi (yanlış algılamayı önler)

// ========== FONKSİYON TANIMLARI ==========

/**
 * Firebase'e komut gönderen fonksiyon
 * @param path: Firebase veritabanındaki veri yolu
 * @param state: Gönderilecek değer (0 veya 1)
 */
void sendCommandToFirebase(String path, int state) {
  if (Firebase.RTDB.setInt(&fbdo, path, state)) {
    Serial.print("✓ Komut gönderildi -> ");
    Serial.print(path);
    Serial.print(": ");
    Serial.println(state);
  } else {
    Serial.print("✗ Firebase Hatası: ");
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

/**
 * Başlangıç kurulum fonksiyonu
 * Tüm pin ayarlarını, Wi-Fi ve Firebase bağlantılarını kurar
 */
void setup() {
  // Seri haberleşmeyi başlat
  Serial.begin(115200);
  Serial.println("\n=== Smart Coding Car Platform - Kodlama Tahtası ===");
  Serial.println("Sistem başlatılıyor...");

  // Fiziksel blok pinlerini giriş olarak ayarla (INPUT_PULLUP ile)
  // INPUT_PULLUP: Pin normalde HIGH, blok takıldığında LOW olur
  pinMode(FORWARD1_PIN, INPUT_PULLUP);   // İleri git bloğu 1
  pinMode(FORWARD2_PIN, INPUT_PULLUP);   // İleri git bloğu 2
  pinMode(BACKWARD_PIN, INPUT_PULLUP);   // Geri git bloğu 1
  pinMode(BACKWARD2_PIN, INPUT_PULLUP);  // Geri git bloğu 2
  pinMode(RIGHT_TURN_PIN, INPUT_PULLUP); // Sağa dön bloğu 1
  pinMode(RIGHT_TURN_PIN2, INPUT_PULLUP);// Sağa dön bloğu 2
  pinMode(LEFT_TURN_PIN, INPUT_PULLUP);  // Sola dön bloğu
  pinMode(HORN_PIN, INPUT_PULLUP);       // Korna bloğu
  pinMode(HEADLIGHT_PIN, INPUT_PULLUP);  // Far bloğu
  
  Serial.println("✓ Blok pinleri ayarlandı");

  // Wi-Fi bağlantısını kur
  Serial.print("Wi-Fi'ye bağlanılıyor");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();

  // 10 saniye boyunca bağlantı dene
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  // Wi-Fi bağlantı durumunu kontrol et
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n✗ Wi-Fi bağlantısı sağlanamadı!");
  } else {
    Serial.println("\n✓ Wi-Fi bağlantısı kuruldu");
    Serial.println("IP Adresi: " + WiFi.localIP().toString());
    WiFi.setSleep(false);  // Güç tasarrufu modunu kapat (daha stabil bağlantı için)
    Serial.println("✓ Wi-Fi güç tasarrufu modu devre dışı bırakıldı");
  }

  // Firebase bağlantısını kur
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectNetwork(true);  // Ağ bağlantısı kesilirse otomatik yeniden bağlan
  Firebase.begin(&config, &auth);
  Serial.println("✓ Firebase bağlantısı kuruldu");
  
  Serial.println("\n🚀 Kodlama tahtası hazır! Blokları takabilirsiniz...");
  Serial.println("==========================================\n");
}

/**
 * Ana döngü fonksiyonu
 * Wi-Fi bağlantısını kontrol eder ve blokları sürekli tarar
 */
void loop() {
  ensureWiFiConnection();  // Wi-Fi bağlantısını kontrol et
  checkAndSendBlocks();    // Blokları kontrol et ve komutları gönder
  delay(500);              // CPU yükünü azaltmak için kısa bekleme
}
