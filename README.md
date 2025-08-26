# Smart Coding Car Platform 🚗

Bu proje, Arduino ESP32 tabanlı akıllı bir kodlama arabası platformudur. Proje, fiziksel kodlama blokları kullanarak arabanın hareketlerini kontrol etmeyi sağlar.

## 📋 Proje Özellikleri

- **Uzaktan Kontrol**: Firebase Realtime Database üzerinden arabanın kontrolü
- **Fiziksel Kodlama**: Kodlama tahtası üzerindeki bloklar ile programlama
- **Çoklu Hareket**: İleri, geri, sağa, sola dönme komutları
- **Ses ve Işık**: Korna ve far kontrolleri
- **Wi-Fi Bağlantısı**: Kablosuz iletişim

## 🔧 Donanım Gereksinimleri

### Araba Modülü (ESP32)
- ESP32 Development Board
- L298N Motor Sürücü
- DC Motorlar (x2)
- Buzzer
- LED (Far için)
- Jumper kablolar

### Kodlama Tahtası (ESP32)
- ESP32 Development Board
- Pull-up dirençler
- Kodlama blokları (fiziksel)
- Breadboard
- Jumper kablolar

## 📁 Dosya Yapısı

```
smart-coding-car-platform/
├── araba/
│   └── araba.ino          # Araba kontrol kodu
└── kodlama_tahta/
    └── kodlama_tahta.ino  # Kodlama tahtası kodu
```

## ⚙️ Kurulum

### 1. Arduino IDE Kurulumu
- Arduino IDE'yi indirin ve kurun
- ESP32 board paketini ekleyin
- Gerekli kütüphaneleri yükleyin:
  - `FirebaseESP32` (araba için)
  - `Firebase_ESP_Client` (kodlama tahtası için)

### 2. Firebase Kurulumu
1. Firebase Console'da yeni proje oluşturun
2. Realtime Database'i etkinleştirin
3. Authentication'ı etkinleştirin ve email/password ile kullanıcı oluşturun
4. API anahtarınızı ve database URL'inizi alın

### 3. Kod Yapılandırması
Her iki `.ino` dosyasında aşağıdaki bilgileri güncelleyin:
```cpp
// Wi-Fi Ayarları
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Firebase Ayarları
const char* API_KEY = "YOUR_API_KEY";
const char* DATABASE_URL = "YOUR_DATABASE_URL";
const char* USER_EMAIL = "YOUR_EMAIL";
const char* USER_PASSWORD = "YOUR_PASSWORD";
```

## 🚀 Kullanım

### Araba Modülü
- ESP32'yi arabaya monte edin
- Motor bağlantılarını yapın (IN1-IN4 pinleri)
- Buzzer ve LED bağlantılarını tamamlayın
- Kodu yükleyin ve çalıştırın

### Kodlama Tahtası
- ESP32'yi kodlama tahtasına monte edin
- Blok pinlerini bağlayın (21, 19, 22, 18, 32, 14, 33, 25, 23)
- Kodu yükleyin ve çalıştırın
- Fiziksel blokları takarak arabanı programlayın

## 📊 Firebase Veri Yapısı

```json
{
  "araba": {
    "forward1": 0,
    "forward2": 0,
    "backward": 0,
    "backward2": 0,
    "left": 0,
    "left2": 0,
    "right": 0,
    "right2": 0,
    "horn": 0,
    "headlights": 0
  },
  "kotlamaTahta": {
    "forward1": 0,
    "forward2": 0,
    "backward": 0,
    "backward2": 0,
    "left": 0,
    "right": 0,
    "right2": 0,
    "horn": 0,
    "headlights": 0
  }
}
```

## 🎮 Komutlar

| Komut | Açıklama | Süre |
|-------|----------|------|
| İleri Git | Arabayı ileri hareket ettirir | 5 saniye |
| Geri Git | Arabayı geri hareket ettirir | 5 saniye |
| Sağa Dön | Arabayı sağa döndürür | 720ms |
| Sola Dön | Arabayı sola döndürür | 720ms |
| Korna Çal | Buzzer'ı çalıştırır | 1 saniye |
| Far Yak | LED farları yakar | 10 saniye |

## 🔌 Pin Bağlantıları

### Araba Modülü
- IN1: Pin 12 (Motor 1 İleri)
- IN2: Pin 13 (Motor 1 Geri)
- IN3: Pin 25 (Motor 2 İleri)
- IN4: Pin 27 (Motor 2 Geri)
- BUZZER: Pin 14
- HEADLIGHTS: Pin 26

### Kodlama Tahtası
- İleri Git 1: Pin 21
- İleri Git 2: Pin 19
- Geri Git 1: Pin 22
- Geri Git 2: Pin 18
- Sağa Dön 1: Pin 32
- Sağa Dön 2: Pin 14
- Sola Dön: Pin 33
- Korna: Pin 25
- Far: Pin 23

## 🛠️ Sorun Giderme

- **Wi-Fi bağlantı sorunu**: SSID ve şifrenizi kontrol edin
- **Firebase bağlantı sorunu**: API anahtarı ve database URL'ini kontrol edin
- **Motor çalışmıyor**: Pin bağlantılarını ve güç kaynağını kontrol edin
- **Bloklar algılanmıyor**: Pull-up dirençleri ve pin bağlantılarını kontrol edin

## 📝 Lisans

Bu proje MIT lisansı altında lisanslanmıştır.

## 👥 Katkıda Bulunma

Projeye katkıda bulunmak için:
1. Fork yapın
2. Feature branch oluşturun
3. Değişikliklerinizi commit edin
4. Pull request gönderin

## 📞 İletişim

Sorularınız için GitHub Issues kullanabilirsiniz.