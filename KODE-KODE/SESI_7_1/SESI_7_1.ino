/*
 * KODE UNTUK SESI 7.1: KONEKSI KE AWAN DAN WAJAH
 * Tujuan: Menghubungkan ESP32 ke WiFi, menampilkan data di OLED, dan mengirimnya ke Adafruit IO.
 *
 * Deskripsi:
 * Kode ini menggabungkan Sesi 6 dan 7. Ia membaca sensor, menampilkannya di layar OLED,
 * terhubung ke internet via WiFi, dan mengirim (publish) data ke Adafruit IO
 * menggunakan metode asynchronous (non-blocking) yang efisien.
 *
 * SEBELUM MEMULAI:
 * 1. Buat akun di https://io.adafruit.com/
 * 2. Buat Feed baru, contoh: "kelembapan-tanah".
 * 3. Dapatkan Username dan Active AIO Key Anda dari dasbor Adafruit IO.
 * 4. Instal Library (via Library Manager):
 * - "U8g2" by oliver
 * - "AsyncMqttClient" by Marvin Roger (pastikan dependensi seperti AsyncTCP juga terinstal)
 *
 * Rangkaian:
 * - Sama seperti Sesi 6 (Sensor + Layar OLED). Tidak ada perubahan hardware.
 */

#include <WiFi.h>
#include <U8g2lib.h>
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
const int nilaiUdara = 4095; // Nilai ADC saat sensor di udara (kering)
const int nilaiAir = 1800;   // Nilai ADC saat sensor di air (basah)

// --- PENGATURAN APLIKASI ---
// Atur nama feed Adafruit IO Anda di sini
const char* AIO_FEED_PATH = AIO_USERNAME "/feeds/kelembapan-tanah";
// Interval pengiriman data ke cloud (dalam milidetik)
const long PUBLISH_INTERVAL = 10000; // 10 detik

// --- INISIALISASI OBJECT ---
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
AsyncMqttClient mqttClient;
unsigned long lastPublishTime = 0;

// Fungsi untuk koneksi WiFi
void connectToWifi() {
  Serial.print("Menghubungkan ke WiFi...");
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 36, "Connecting to WiFi...");
  u8g2.sendBuffer();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
}

// Fungsi yang dipanggil saat MQTT berhasil terhubung
void onMqttConnect(bool sessionPresent) {
  Serial.println("Terhubung ke Adafruit IO MQTT!");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

// Fungsi yang dipanggil saat koneksi MQTT terputus
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Koneksi ke MQTT terputus.");
  if (WiFi.isConnected()) {
    mqttClient.connect();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  u8g2.begin();

  connectToWifi();

  mqttClient.setServer(AIO_SERVER, AIO_SERVERPORT);
  mqttClient.setCredentials(AIO_USERNAME, AIO_KEY);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.connect();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(18, 36, "Penjaga Siap!");
  u8g2.sendBuffer();
  delay(2000);
}

void loop() {
  // 1. Baca dan kalkulasi data sensor
  int nilaiRasa = analogRead(soilPin);
  int persentaseLembap = map(nilaiRasa, nilaiUdara, nilaiAir, 0, 100);
  persentaseLembap = constrain(persentaseLembap, 0, 100);

  // 2. Tampilkan data ke layar OLED
  u8g2.clearBuffer();

  // Tampilkan persentase kelembapan
  u8g2.setFont(u8g2_font_logisoso18_tr);
  u8g2.setCursor(0, 22);
  u8g2.print("Lembap: ");
  u8g2.print(persentaseLembap);
  u8g2.print("%");

  // Tampilkan status tanah
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 45);
  if (persentaseLembap < 30) {
    u8g2.print("Status: TANAH KERING");
  } else if (persentaseLembap > 85) {
    u8g2.print("Status: TERLALU BASAH");
  } else {
    u8g2.print("Status: KONDISI AMAN");
  }

  // Tampilkan status koneksi cloud
  if (mqttClient.connected()) {
    u8g2.setFont(u8g2_font_open_iconic_www_1x_t); // Font berisi ikon
    u8g2.drawGlyph(118, 8, 0x4A); // Gambar ikon awan (cloud)
  }

  u8g2.sendBuffer();

  // 3. Kirim data ke cloud secara berkala (non-blocking)
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    if (mqttClient.connected()) {
      Serial.print("Mengirim data: ");
      Serial.println(persentaseLembap);

      // Kirim data ke feed Adafruit IO
      mqttClient.publish(AIO_FEED_PATH, 1, true, String(persentaseLembap).c_str());
      lastPublishTime = millis();
    }
  }

  // Delay singkat agar loop tidak berjalan terlalu cepat
  delay(1000);
}
