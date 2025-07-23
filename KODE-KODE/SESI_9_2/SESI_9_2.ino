/*
 * KODE UNTUK SESI 9.2: AUTOMASI DENGAN LAYAR & NOTIFIKASI
 * Tujuan: Menambahkan display OLED ke sistem automasi untuk monitoring lokal.
 *
 * Deskripsi:
 * Ini adalah pengembangan dari Sesi 9. Sistem kini menampilkan statusnya secara
 * real-time pada layar OLED, membuatnya lebih mudah dipantau tanpa harus
 * membuka aplikasi Telegram atau Serial Monitor.
 *
 * SEBELUM MEMULAI:
 * 1. Instal Library: Pastikan library "CTBot" dan "U8g2" sudah terinstal.
 *
 * Rangkaian:
 * - Sensor Kelembapan + Relay + Layar OLED I2C.
 * - SDA layar -> GPIO 21
 * - SCL layar -> GPIO 22
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <CTBot.h>
#include <U8g2lib.h> // Library untuk OLED

// --- PENGATURAN KONEKSI WIFI & TELEGRAM ---
#define WIFI_SSID       "NAMA_WIFI_ANDA"
#define WIFI_PASS       "PASSWORD_WIFI_ANDA"
#define BOT_TOKEN       "TOKEN_BOT_TELEGRAM_ANDA"
#define CHAT_ID         "CHAT_ID_TELEGRAM_ANDA"

// --- PENGATURAN PIN & LOGIKA ---
const int relayPin = 23;
const int soilPin = 34;
const int ambangBatasKering = 30;
const long durasiSiram = 2000;
const long jedaAntarSiramOtomatis = 3600000;

// --- PENGATURAN KALIBRASI SENSOR ---
const int nilaiUdara = 4095;
const int nilaiAir = 1800;

// --- INISIALISASI OBJECT & VARIABEL ---
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
CTBot myBot;
unsigned long waktuSiramTerakhir = 0;
unsigned long wateringStartTime = 0;
bool isWatering = false;

// --- FUNGSI PEMBANTU ---
int bacaKelembapan() {
  int nilaiRasa = analogRead(soilPin);
  int persentase = map(nilaiRasa, nilaiUdara, nilaiAir, 0, 100);
  return constrain(persentase, 0, 100);
}

void kirimNotifikasiTelegram(String pesan) {
  Serial.print("Mengirim notifikasi: ");
  Serial.println(pesan);
  myBot.sendMessage(CHAT_ID, pesan);
}

void siramTanaman() {
  isWatering = true;
  wateringStartTime = millis();
  waktuSiramTerakhir = millis();
  digitalWrite(relayPin, LOW);

  String pesan = "💧 Tanah kering terdeteksi!\nMenyiram tanaman secara otomatis.\n";
  pesan += "Kelembapan saat ini: " + String(bacaKelembapan()) + "%";
  kirimNotifikasiTelegram(pesan);
}

void tampilLayar(String status, int kelembapan) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso18_tr);
    u8g2.setCursor(0, 22);
    u8g2.print("Lembap: ");
    u8g2.print(kelembapan);
    u8g2.print("%");

    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 45);
    u8g2.print("Status: ");
    u8g2.print(status);

    if (WiFi.status() == WL_CONNECTED) {
        u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
        u8g2.drawGlyph(118, 8, 0x57); // Ikon WiFi
    }

    u8g2.sendBuffer();
}

// --- SETUP UTAMA ---
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  u8g2.begin(); // Inisialisasi OLED
  u8g2.setFont(u8g2_font_ncenB08_tr);
  tampilLayar("Mulai...", 0);

  Serial.print("Menghubungkan ke WiFi...");
  tampilLayar("WiFi...", 0);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  tampilLayar("Online!", 0);

  myBot.setTelegramToken(BOT_TOKEN);
  if (myBot.testConnection()) {
    Serial.println("Koneksi Telegram berhasil!");
    kirimNotifikasiTelegram("✅ Sistem Penjaga Tanaman Online!");
  } else {
    Serial.println("Koneksi Telegram gagal!");
  }

  waktuSiramTerakhir = millis();
  delay(2000);
}

// --- LOOP UTAMA (NON-BLOCKING) ---
void loop() {
  String statusLayar = "Memantau";
  int persentaseLembap = bacaKelembapan();

  // 1. Cek apakah sedang dalam proses menyiram
  if (isWatering) {
    statusLayar = "Menyiram...";
    if (millis() - wateringStartTime >= durasiSiram) {
      digitalWrite(relayPin, HIGH);
      isWatering = false;
      Serial.println("Penyiraman selesai.");
    }
  }
  // 2. Cek apakah perlu menyiram otomatis
  else if (millis() - waktuSiramTerakhir >= jedaAntarSiramOtomatis) {
    if (persentaseLembap < ambangBatasKering) {
      siramTanaman();
    }
  }
  else {
    statusLayar = "Siaga";
  }

  // 3. Update tampilan layar
  tampilLayar(statusLayar, persentaseLembap);

  // 4. Jeda singkat untuk refresh rate layar
  delay(1000);
}
