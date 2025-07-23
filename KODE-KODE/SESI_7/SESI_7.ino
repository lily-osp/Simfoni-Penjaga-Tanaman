/*
 * KODE UNTUK SESI 7: KONEKSI KE AWAN
 * Tujuan: Menghubungkan ESP32 ke WiFi dan mengirim data sensor ke Adafruit IO.
 *
 * Deskripsi:
 * Kode ini adalah langkah pertama untuk membuat perangkat kita menjadi "IoT".
 * Ia akan membaca sensor, menghubungkannya ke internet via WiFi, lalu mengirim
 * (publish) data kelembapan ke sebuah "feed" di Adafruit IO.
 * Versi ini menggunakan library AsyncMqttClient yang lebih modern dan efisien.
 *
 * SEBELUM MEMULAI:
 * 1. Buat akun di https://io.adafruit.com/
 * 2. Buat sebuah Feed baru, contohnya dengan nama "kelembapan-tanah".
 * 3. Di halaman utama Adafruit IO, klik "My Key" atau "AIO Key" untuk mendapatkan
 * Username dan Active AIO Key Anda.
 * 4. Instal Library (via Library Manager):
 * - "AsyncMqttClient" by Marvin Roger (pastikan dependensi seperti AsyncTCP juga terinstal).
 *
 * Rangkaian:
 * - Hanya sensor kelembapan yang terhubung ke ESP32 sesuai Sesi 5.
 */

#include <WiFi.h>
#include <AsyncMqttClient.h>

// --- PENGATURAN KONEKSI (UBAH SESUAI DATA ANDA) ---
#define WIFI_SSID      "NAMA_WIFI_ANDA"
#define WIFI_PASS      "PASSWORD_WIFI_ANDA"

#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME   "USERNAME_ADAFRUIT_IO_ANDA"
#define AIO_KEY        "AIO_KEY_ADAFRUIT_IO_ANDA"

// --- PENGATURAN SENSOR & PIN ---
const int soilPin = 34;
const int nilaiUdara = 4095;
const int nilaiAir = 1800;

// --- PENGATURAN APLIKASI ---
// Atur nama feed Adafruit IO Anda di sini
const char* AIO_FEED_PATH = AIO_USERNAME "/feeds/kelembapan-tanah";
// Interval pengiriman data ke cloud (dalam milidetik)
const long PUBLISH_INTERVAL = 10000; // Kirim data setiap 10 detik

// --- INISIALISASI OBJECT ---
AsyncMqttClient mqttClient;
unsigned long lastPublishTime = 0;
TimerHandle_t mqttReconnectTimer;

// Prototipe fungsi
void connectToMqtt();

void connectToWifi() {
  Serial.print("Menghubungkan ke WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Terhubung ke Adafruit IO MQTT!");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Koneksi ke MQTT terputus. Mencoba menghubungkan kembali...");
  // Jangan panggil connect() langsung dari callback, gunakan timer
  xTimerStart(mqttReconnectTimer, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Mulai Sesi 7: Koneksi ke Awan...");

  // Buat timer untuk reconnect, tapi jangan langsung dijalankan
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, [](TimerHandle_t xTimer){
    connectToMqtt();
  });

  connectToWifi();

  mqttClient.setServer(AIO_SERVER, AIO_SERVERPORT);
  mqttClient.setCredentials(AIO_USERNAME, AIO_KEY);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);

  connectToMqtt();
}

void connectToMqtt() {
  Serial.println("Menghubungkan ke MQTT...");
  mqttClient.connect();
}

void loop() {
  // Cek apakah sudah waktunya mengirim data baru
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    // Pastikan client terhubung sebelum mengirim
    if (mqttClient.connected()) {
      // Baca dan kalibrasi sensor
      int nilaiRasa = analogRead(soilPin);
      int persentaseLembap = map(nilaiRasa, nilaiUdara, nilaiAir, 0, 100);
      persentaseLembap = constrain(persentaseLembap, 0, 100);

      Serial.print("Membaca kelembapan: ");
      Serial.print(persentaseLembap);
      Serial.println("%");

      // Kirim (publish) data ke feed Adafruit IO
      Serial.print("Mengirim data ke Adafruit IO... ");
      uint16_t packetId = mqttClient.publish(AIO_FEED_PATH, 1, true, String(persentaseLembap).c_str());

      if(packetId > 0) {
        Serial.println("Berhasil dikirim!");
      } else {
        Serial.println("Gagal dikirim!");
      }

      // Catat waktu pengiriman terakhir
      lastPublishTime = millis();
    } else {
      Serial.println("MQTT tidak terhubung. Pengiriman data ditunda.");
    }
  }
}
