/*
 * KODE UNTUK SESI 9.1: AUTOMASI & NOTIFIKASI TELEGRAM
 * Tujuan: Membuat sistem siram otomatis yang mengirimkan notifikasi via Telegram.
 *
 * Deskripsi:
 * Ini adalah versi "Penjaga Tanaman" yang beroperasi secara mandiri. Fitur-fiturnya:
 * - Otomatis menyiram jika tanah terdeteksi kering.
 * - Mengirim notifikasi ke Telegram setiap kali penyiraman terjadi.
 * - Versi ini telah diperbarui untuk menggunakan library "UniversalTelegramBot" yang lebih modern.
 *
 * SEBELUM MEMULAI:
 * 1. Instal Library: Buka Library Manager dan instal "Universal Telegram Bot" by Brian Lough.
 * Arduino IDE akan meminta untuk menginstal dependensinya, "ArduinoJson", setujui.
 * 2. Buat Bot Telegram:
 * - Cari "BotFather" di Telegram.
 * - Kirim perintah /newbot dan ikuti instruksinya.
 * - Catat BOT TOKEN yang diberikan.
 * 3. Dapatkan Chat ID Anda:
 * - Cari bot "IDBot" atau "userinfobot" di Telegram.
 * - Kirim perintah /start untuk mendapatkan Chat ID Anda.
 *
 * Rangkaian:
 * - Sensor Kelembapan + Relay. Tidak ada perubahan hardware.
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// --- PENGATURAN KONEKSI WIFI & TELEGRAM (UBAH SESUAI DATA ANDA) ---
#define WIFI_SSID       "NAMA_WIFI_ANDA"
#define WIFI_PASS       "PASSWORD_WIFI_ANDA"
#define BOT_TOKEN       "TOKEN_BOT_TELEGRAM_ANDA"
// Ganti angka di bawah ini dengan Chat ID numerik Anda (tanpa tanda kutip)
#define CHAT_ID         "123456789" 

// --- PENGATURAN PIN & LOGIKA ---
const int relayPin = 23;
const int soilPin = 34;
const int ambangBatasKering = 30; // Siram jika kelembapan di bawah 30%
const long durasiSiram = 2000;    // Durasi pompa menyala (2 detik)
// Jeda minimal antar penyiraman otomatis (1 jam = 3,600,000 ms)
const long jedaAntarSiramOtomatis = 3600000;

// --- PENGATURAN KALIBRASI SENSOR ---
const int nilaiUdara = 4095;
const int nilaiAir = 1800;

// --- INISIALISASI OBJECT & VARIABEL ---
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

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
  if (!bot.sendMessage(CHAT_ID, pesan, "")) {
    Serial.println("Gagal mengirim pesan Telegram");
  }
}

void siramTanaman() {
  isWatering = true;
  wateringStartTime = millis();
  waktuSiramTerakhir = millis();
  digitalWrite(relayPin, LOW); // Nyalakan pompa

  String pesan = "💧 Tanah kering terdeteksi!\nMenyiram tanaman secara otomatis.\n";
  pesan += "Kelembapan saat ini: " + String(bacaKelembapan()) + "%";
  kirimNotifikasiTelegram(pesan);
}


// --- SETUP UTAMA ---
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Pastikan relay mati saat mulai

  // Koneksi ke WiFi
  Serial.print("Menghubungkan ke WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Diperlukan untuk koneksi aman
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");

  // Cek koneksi ke Telegram
  if (bot.getMe()) {
    Serial.println("Koneksi Telegram berhasil!");
    kirimNotifikasiTelegram("✅ Sistem Penjaga Tanaman Online!");
  } else {
    Serial.println("Koneksi Telegram gagal!");
  }

  // Inisialisasi agar tidak langsung menyiram saat pertama kali dinyalakan
  waktuSiramTerakhir = millis();
}


// --- LOOP UTAMA (NON-BLOCKING) ---
void loop() {
  // 1. Cek apakah sedang dalam proses menyiram
  if (isWatering && (millis() - wateringStartTime >= durasiSiram)) {
    digitalWrite(relayPin, HIGH); // Matikan pompa
    isWatering = false;
    Serial.println("Penyiraman selesai.");
  }

  // 2. Cek apakah perlu menyiram otomatis
  // Cek hanya jika tidak sedang menyiram & sudah melewati jeda minimal
  if (!isWatering && (millis() - waktuSiramTerakhir >= jedaAntarSiramOtomatis)) {
    int persentaseLembap = bacaKelembapan();
    Serial.print("Membaca kelembapan: ");
    Serial.println(persentaseLembap);

    if (persentaseLembap < ambangBatasKering) {
      siramTanaman();
    } else {
      // Jika tidak kering, reset timer pengecekan agar tidak spamming serial monitor
      // dan anggap seolah-olah baru "disiram" agar menunggu jeda lagi.
      waktuSiramTerakhir = millis();
    }
  }
}
