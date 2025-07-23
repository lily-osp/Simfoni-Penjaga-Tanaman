/*
 * KODE UNTUK SESI 8: DASHBOARD & KONTROL JARAK JAUH
 * Tujuan: Menerima perintah dari dashboard Adafruit IO untuk mengontrol relay.
 *
 * Deskripsi:
 * Kode ini adalah pengembangan dari Sesi 7. Selain mengirim data,
 * perangkat kini juga "mendengarkan" (subscribe) sebuah feed kontrol.
 * Jika kita mengirim pesan ke feed tersebut (misalnya, dengan menekan tombol di dashboard),
 * perangkat akan merespons dengan menyalakan pompa.
 * Versi ini menggunakan library AsyncMqttClient yang lebih modern dan efisien.
 *
 * SEBELUM MEMULAI:
 * 1. Di Adafruit IO, buat sebuah Feed baru, contohnya "kontrol-pompa".
 * 2. Di Dashboard, tambahkan elemen "Toggle Button" atau "Push Button".
 * 3. Hubungkan tombol tersebut ke feed "kontrol-pompa". Atur nilai "ON" menjadi 1 dan "OFF" menjadi 0.
 *
 * Rangkaian:
 * - Tambahkan modul Relay.
 * - IN relay -> GPIO 23
 * - VCC relay -> 5V, GND relay -> GND
 * - Sensor kelembapan tetap terhubung.
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

// --- PENGATURAN PIN ---
const int relayPin = 23;
const int soilPin = 34;
const int nilaiUdara = 4095;
const int nilaiAir = 1800;

// --- PENGATURAN APLIKASI ---
// Feed untuk mengirim data kelembapan
const char* AIO_PUBLISH_FEED = AIO_USERNAME "/feeds/kelembapan-tanah";
// Feed untuk menerima perintah kontrol
const char* AIO_SUBSCRIBE_FEED = AIO_USERNAME "/feeds/kontrol-pompa";

// Interval pengiriman data (30 detik)
const long PUBLISH_INTERVAL = 30000;
// Durasi pompa menyala saat diperintah (2 detik)
const long WATERING_DURATION = 2000;

// --- INISIALISASI OBJECT & VARIABEL ---
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;

unsigned long lastPublishTime = 0;
unsigned long wateringStartTime = 0;
bool isWatering = false;

// --- PROTOTIPE FUNGSI ---
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

// --- KONEKSI & SETUP ---
void connectToWifi() {
  Serial.print("Menghubungkan ke WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Pastikan relay mati (HIGH-triggered) atau LOW (LOW-triggered)

  Serial.println("Mulai Sesi 9: Kontrol Jarak Jauh...");

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, [](TimerHandle_t xTimer) {
    connectToMqtt();
  });

  connectToWifi();

  mqttClient.setServer(AIO_SERVER, AIO_SERVERPORT);
  mqttClient.setCredentials(AIO_USERNAME, AIO_KEY);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);

  connectToMqtt();
}


// --- FUNGSI CALLBACK MQTT ---
void connectToMqtt() {
  Serial.println("Menghubungkan ke MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Terhubung ke Adafruit IO MQTT!");
  // Langganan (subscribe) ke feed kontrol setelah berhasil terhubung
  uint16_t packetIdSub = mqttClient.subscribe(AIO_SUBSCRIBE_FEED, 1);
  Serial.print("Berlangganan ke feed '");
  Serial.print(AIO_SUBSCRIBE_FEED);
  Serial.print("'...");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Koneksi ke MQTT terputus. Mencoba menghubungkan kembali...");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

// Fungsi ini akan otomatis terpanggil saat ada pesan masuk
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // Ubah payload menjadi string yang bisa dibaca
  String message;
  for (size_t i = 0; i < len; i++) {
    message += (char)payload[i];
  }

  Serial.print("Pesan diterima dari topik: ");
  Serial.println(topic);
  Serial.print("Isi pesan: ");
  Serial.println(message);

  // Cek apakah pesan berasal dari feed kontrol yang benar
  if (String(topic) == String(AIO_SUBSCRIBE_FEED)) {
    // Jika pesan adalah "1" dan pompa tidak sedang menyala
    if (message == "1" && !isWatering) {
      Serial.println("Perintah siram manual diterima! Menyiram...");
      digitalWrite(relayPin, LOW); // Nyalakan pompa
      isWatering = true;
      wateringStartTime = millis(); // Catat waktu mulai
    }
  }
}

// --- LOOP UTAMA ---
void loop() {
  // Bagian 1: Mengirim data kelembapan secara berkala
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    if (mqttClient.connected()) {
      int persentaseLembap = map(analogRead(soilPin), nilaiUdara, nilaiAir, 0, 100);
      persentaseLembap = constrain(persentaseLembap, 0, 100);

      Serial.print("Mengirim kelembapan: "); Serial.println(persentaseLembap);
      mqttClient.publish(AIO_PUBLISH_FEED, 1, true, String(persentaseLembap).c_str());
      lastPublishTime = millis();
    }
  }

  // Bagian 2: Mengontrol durasi pompa (non-blocking)
  if (isWatering && (millis() - wateringStartTime >= WATERING_DURATION)) {
    digitalWrite(relayPin, HIGH); // Matikan pompa
    isWatering = false;
    Serial.println("Siram manual selesai.");
  }
}
