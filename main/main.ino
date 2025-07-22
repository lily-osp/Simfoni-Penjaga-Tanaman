// ==== mulaan kode utama ... ===

#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

WiFiClient client;
WiFiClientSecure secured_client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

Adafruit_MQTT_Publish soilMoistureFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil-moisture");
Adafruit_MQTT_Subscribe waterPumpFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/water-pump");

unsigned long lastSensorRead = 0;
unsigned long lastBotCheck = 0;
unsigned long lastMQTTRetry = 0;

bool isWatering = false;
unsigned long wateringStartTime = 0;

// ==== mulaan kode fungsi untuk mengelola air ===
void waterPlant(String pemicu) {
  if (isWatering) {
    return;
  }
  
  digitalWrite(RELAY_PIN, HIGH);
  isWatering = true;
  wateringStartTime = millis();
  
  String pesan = "💧 Embun kehidupan mulai turun... terpicu oleh " + pemicu;
  bot.sendMessage(CHAT_ID, pesan, "");
}

void stopWatering() {
  digitalWrite(RELAY_PIN, LOW);
  isWatering = false;
  
  mqtt.publish(AIO_USERNAME "/feeds/water-pump", "0");
  bot.sendMessage(CHAT_ID, "✅ Dahaga telah terobati. Semesta kembali tenang.", "");
}
// ==== akhiran kode ... ===

// ==== mulaan kode fungsi untuk menangani pesan Telegram ===
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    
    if (chat_id != CHAT_ID) {
      continue;
    }
    
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    
    if (text == "/start") {
      String sapaan = "Salam, jiwa yang terhubung. Aku adalah penjaga sunyi bagi kuntum ini.\n\n";
      sapaan += "Bisikkan padaku:\n";
      sapaan += "/rasa - untuk merasakan denyut bumi\n";
      sapaan += "/embun - untuk menurunkan berkah air\n";
      sapaan += "/henti - untuk menenangkan tarian air\n";
      sapaan += "/petunjuk - untuk mengingat kembali mantra";
      
      bot.sendMessage(CHAT_ID, sapaan, "");
      
    } else if (text == "/rasa") {
      int moisture_value = analogRead(SOIL_PIN);
      String status = "Aku merasakan denyut bumi...\n\n";
      status += "Getar Kelembapan: " + String(moisture_value) + "\n";
      status += "Ambang Dahaga: " + String(DRY_THRESHOLD) + "\n\n";
      status += "Jiwa tanah kini terasa: ";
      status += (moisture_value > DRY_THRESHOLD ? "🟡 Kering, menanti embun" : "🟢 Segar, penuh kehidupan");
      if (isWatering) {
        status += "\n\n💧 Embun sedang menari...";
      }
      
      bot.sendMessage(CHAT_ID, status, "");
      
    } else if (text == "/embun") {
      if (isWatering) {
        bot.sendMessage(CHAT_ID, "⚠️ Sabar, duhai jiwa. Embun masih menari. Biarkan ia menyelesaikan tugasnya.", "");
      } else {
        waterPlant("panggilanmu, " + from_name);
      }
      
    } else if (text == "/henti") {
      if (isWatering) {
        stopWatering();
        bot.sendMessage(CHAT_ID, "🛑 Atas pintamu, tarian embun kuhentikan.", "");
      } else {
        bot.sendMessage(CHAT_ID, "ℹ️ Tenang, embun sedang tidak menari saat ini.", "");
      }
      
    } else if (text == "/petunjuk") {
      String petunjuk = "Ingatlah mantra ini, duhai jiwa:\n\n";
      petunjuk += "/rasa - Merasakan kelembapan tanah\n";
      petunjuk += "/embun - Memulai siraman berkah\n";
      petunjuk += "/henti - Menghentikan aliran air darurat\n";
      petunjuk += "/start - Menyapa sang penjaga\n\n";
      petunjuk += "Aku akan menurunkan embun secara otomatis jika getarannya melampaui " + String(DRY_THRESHOLD);
      
      bot.sendMessage(CHAT_ID, petunjuk, "");
      
    } else {
      bot.sendMessage(CHAT_ID, "❓ Maaf, bisikan itu tak kukenali. Ucapkan /petunjuk agar kita bisa saling memahami.", "");
    }
  }
}
// ==== akhiran kode ... ===

// ==== mulaan kode fungsi untuk konektivitas ===
bool connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  Serial.print("Merajut benang ke semesta maya");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nKini kita terhubung.");
    Serial.println("Alamat di semesta: " + WiFi.localIP().toString());
    return true;
  } else {
    Serial.println("\nBenang koneksi terputus.");
    return false;
  }
}

bool connectToMQTT() {
  if (mqtt.connected()) {
    return true;
  }
  
  if (millis() - lastMQTTRetry < MQTT_RETRY_INTERVAL) {
    return false;
  }
  
  Serial.println("Membuka gerbang ke awan kesadaran...");
  
  int8_t ret = mqtt.connect();
  if (ret == 0) {
    Serial.println("Gerbang telah terbuka.");
    return true;
  } else {
    Serial.println("Gerbang gagal terbuka: " + String(mqtt.connectErrorString(ret)));
    mqtt.disconnect();
    lastMQTTRetry = millis();
    return false;
  }
}
// ==== akhiran kode ... ===

// ==== mulaan kode fungsi utama setup ===
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🌱 Sang Penjaga Tanaman Mulai Terjaga...");
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  if (!connectToWiFi()) {
    Serial.println("Gagal terhubung ke semesta. Akan kucoba lagi...");
  }
  
  secured_client.setCACert(telegram_root_ca);
  
  mqtt.subscribe(&waterPumpFeed);
  
  lastSensorRead = millis();
  lastBotCheck = millis();
  
  Serial.println("Sang penjaga siap bertugas!");
}
// ==== akhiran kode ... ===

// ==== mulaan kode fungsi utama loop ===
void loop() {
  if (!connectToWiFi()) {
    delay(5000);
    return;
  }
  
  if (isWatering && (millis() - wateringStartTime >= WATERING_DURATION)) {
    stopWatering();
  }
  
  bool mqttConnected = connectToMQTT();
  
  if (millis() - lastSensorRead >= SENSOR_INTERVAL) {
    int moistureValue = analogRead(SOIL_PIN);
    
    if (mqttConnected) {
      soilMoistureFeed.publish((int32_t)moistureValue);
    }
    
    lastSensorRead = millis();
    
    if (moistureValue > DRY_THRESHOLD && !isWatering) {
      waterPlant("bisikan dahaga dari tanah");
    }
  }
  
  if (mqttConnected) {
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(100))) {
      if (subscription == &waterPumpFeed) {
        String command = (char *)waterPumpFeed.lastread;
        if (command == "1" && !isWatering) {
          waterPlant("perintah dari awan kesadaran");
        } else if (command == "0" && isWatering) {
          stopWatering();
        }
      }
    }
  }
  
  if (millis() - lastBotCheck >= BOT_CHECK_INTERVAL) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
    }
    
    lastBotCheck = millis();
  }
  
  delay(100);
}
// ==== akhiran kode ... ===
