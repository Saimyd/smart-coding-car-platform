/*
 * Smart Coding Car Platform - Araba Kontrol Modülü
 * 
 * Bu kod, ESP32 tabanlı akıllı arabanın hareket kontrolünü sağlar.
 * Firebase Realtime Database üzerinden gelen komutları alır ve
 * arabayı buna göre hareket ettirir.
 * 
 * Özellikler:
 * - Wi-Fi bağlantısı
 * - Firebase ile gerçek zamanlı veri alışverişi
 * - Motor kontrolü (ileri, geri, sağa, sola)
 * - Buzzer kontrolü (korna)
 * - LED kontrolü (farlar)
 * 
 * Geliştirici: Smart Coding Car Platform
 * Tarih: 2024
 */

#include <WiFi.h>
#include <FirebaseESP32.h>

// Wi-Fi Ağ Ayarları - Kendi ağ bilgilerinizi girin
const char* WIFI_SSID = "SAİM";           // Wi-Fi ağ adı
const char* WIFI_PASSWORD = "12345678";    // Wi-Fi şifresi

// Firebase Proje Ayarları - Kendi Firebase bilgilerinizi girin
const char* API_KEY = "AIzaSyDk7511OqPNNTwB-o1VXYVGCQ8l8dLsUBM";  // Firebase API anahtarı
const char* DATABASE_URL = "https://loginproject-19ca4-default-rtdb.firebaseio.com/";  // Firebase database URL'i
const char* USER_EMAIL = "admin@admin.com";     // Firebase kullanıcı email'i
const char* USER_PASSWORD = "123456";           // Firebase kullanıcı şifresi

// Firebase nesneleri
FirebaseData firebaseData;  // Firebase veri alışverişi için
FirebaseAuth auth;          // Firebase kimlik doğrulama için
FirebaseConfig config;      // Firebase yapılandırma için

// Motor Sürücü Pin Tanımlamaları (L298N)
const int IN1 = 12;  // Sol motor ileri pini
const int IN2 = 13;  // Sol motor geri pini
const int IN3 = 25;  // Sağ motor ileri pini
const int IN4 = 27;  // Sağ motor geri pini

// Diğer Donanım Pin Tanımlamaları
const int BUZZER_PIN = 14;      // Buzzer (korna) pini
const int HEADLIGHTS_PIN = 26;  // LED farlar pini

// Hareket komut değişkenleri - Firebase'den alınan değerler
int forward1 = 0, forward2 = 0;     // İleri git komutları
int backward = 0, backward2 = 0;    // Geri git komutları
int left = 0, left2 = 0;           // Sola dön komutları
int right = 0, right2 = 0;         // Sağa dön komutları
int horn = 0;                      // Korna komutu
int headlights = 0;                // Far komutu

/**
 * Başlangıç kurulum fonksiyonu
 * Bu fonksiyon sadece bir kez çalışır ve tüm başlangıç ayarlarını yapar
 */
void setup() {
  // Seri haberleşmeyi başlat (hata ayıklama için)
  Serial.begin(115200);
  Serial.println("Smart Coding Car Platform başlatılıyor...");
  
  // Motor kontrol pinlerini çıkış olarak ayarla
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Buzzer ve LED pinlerini çıkış olarak ayarla
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(HEADLIGHTS_PIN, OUTPUT);
  
  // Wi-Fi bağlantısını başlat
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wi-Fi'ye bağlanılıyor");
  
  // Wi-Fi bağlantısı kurulana kadar bekle
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wi-Fi'ye başarıyla bağlandı!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());

  // Firebase yapılandırmasını ayarla
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Firebase bağlantısını başlat
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("Firebase bağlantısı kuruldu!");
  Serial.println("Araba hazır - komutlar bekleniyor...");
}

/**
 * Ana döngü fonksiyonu
 * Bu fonksiyon sürekli çalışır ve Firebase'den komutları alarak arabayı kontrol eder
 */
void loop() {
  // Firebase'den tüm hareket komutlarını oku
  
  // İleri git komutlarını kontrol et
  if (Firebase.getInt(firebaseData, "/araba/forward1")) {
    forward1 = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/forward2")) {
    forward2 = firebaseData.intData();
  }
  
  // Geri git komutlarını kontrol et
  if (Firebase.getInt(firebaseData, "/araba/backward")) {
    backward = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/backward2")) {
    backward2 = firebaseData.intData();
  }
  
  // Sola dön komutlarını kontrol et
  if (Firebase.getInt(firebaseData, "/araba/left")) {
    left = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/left2")) {
    left2 = firebaseData.intData();
  }
  
  // Sağa dön komutlarını kontrol et
  if (Firebase.getInt(firebaseData, "/araba/right")) {
    right = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, "/araba/right2")) {
    right2 = firebaseData.intData();
  }
  
  // Korna komutunu kontrol et
  if (Firebase.getInt(firebaseData, "/araba/horn")) {
    horn = firebaseData.intData();
  }
  
  // Far komutunu kontrol et
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

// ========== MOTOR KONTROL FONKSİYONLARI ==========

/**
 * Arabayı ileri hareket ettirir
 * Her iki motoru da ileri yönde çalıştırır
 */
void ileri() {
  digitalWrite(IN1, HIGH);  // Sol motor ileri
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // Sağ motor ileri
  digitalWrite(IN4, LOW);
  Serial.println("Hareket: İleri");
}

/**
 * Arabayı geri hareket ettirir
 * Her iki motoru da geri yönde çalıştırır
 */
void geri() {
  digitalWrite(IN1, LOW);   // Sol motor geri
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);   // Sağ motor geri
  digitalWrite(IN4, HIGH);
  Serial.println("Hareket: Geri");
}

/**
 * Arabayı sağa döndürür
 * Sol motoru ileri, sağ motoru geri çalıştırarak sağa dönüş yapar
 */
void saga() {
  digitalWrite(IN1, LOW);   // Sol motor geri
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);  // Sağ motor ileri
  digitalWrite(IN4, LOW);
  Serial.println("Hareket: Sağa Dön");
}

/**
 * Arabayı sola döndürür
 * Sol motoru geri, sağ motoru ileri çalıştırarak sola dönüş yapar
 */
void sola() {
  digitalWrite(IN1, HIGH);  // Sol motor ileri
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // Sağ motor geri
  digitalWrite(IN4, HIGH);
  Serial.println("Hareket: Sola Dön");
}

/**
 * Arabayı durdurur
 * Tüm motorları kapatır
 */
void dur() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Hareket: Dur");
}

// ========== SES VE IŞIK KONTROL FONKSİYONLARI ==========

/**
 * Kornayı çalıştırır
 * 1000 Hz frekansta 500ms süreyle ses çıkarır
 */
void kornaCal() {
  tone(BUZZER_PIN, 1000);   // 1000 Hz frekansta ses
  delay(500);               // 500ms süreyle çal
  noTone(BUZZER_PIN);       // Sesi durdur
  Serial.println("Ses: Korna Çalındı");
}

/**
 * Kornayı susturur
 */
void kornaSus() {
  noTone(BUZZER_PIN);
}

/**
 * Farları yakar
 * LED'i HIGH konumuna getirir
 */
void acHeadlights() {
  digitalWrite(HEADLIGHTS_PIN, HIGH);
  Serial.println("Işık: Farlar Açıldı");
}

/**
 * Farları kapatır
 * LED'i LOW konumuna getirir
 */
void kapatHeadlights() {
  digitalWrite(HEADLIGHTS_PIN, LOW);
  Serial.println("Işık: Farlar Kapatıldı");
}