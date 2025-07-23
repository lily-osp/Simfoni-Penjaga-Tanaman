/*
 * KODE UNTUK SESI 6.1: MEMBERI WAJAH (OLED)
 * Tujuan: Menampilkan data sensor kelembapan pada layar OLED I2C.
 *
 * Deskripsi:
 * Kode ini mengambil data dari sensor dan menampilkannya di layar OLED,
 * membuat Sang Penjaga lebih mandiri tanpa perlu terhubung ke komputer.
 *
 * PENTING:
 * 1. Instal Library: Buka Library Manager di Arduino IDE (Sketch > Include Library > Manage Libraries)
 * dan instal "U8g2" by oliver.
 *
 * Rangkaian (OLED I2C):
 * - VCC layar -> VIN atau 5V pada ESP32
 * - GND layar -> GND pada ESP32
 * - SDA layar -> GPIO 21 pada ESP32
 * - SCL layar -> GPIO 22 pada ESP32
 * - Sensor Kelembapan tetap terhubung seperti di Sesi 5.
 */

#include <Wire.h>
#include <U8g2lib.h> // Library khusus untuk U8g2

// Inisialisasi layar OLED. Constructor ini menggunakan hardware I2C ESP32.
// U8G2_R0 = No rotation
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin sensor kelembapan
const int soilPin = 34;

// Variabel untuk kalibrasi (UBAH NILAI INI SESUAI PENGUKURAN ANDA)
const int nilaiUdara = 4095; // Nilai saat sensor kering
const int nilaiAir = 1800;   // Nilai saat sensor basah tercelup air

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Inisialisasi I2C

  // Inisialisasi layar OLED
  if (!u8g2.begin()) {
    Serial.println(F("Gagal menemukan OLED"));
    while (true); // Loop selamanya jika gagal
  }

  // Tampilkan pesan awal
  u8g2.clearBuffer();                   // Hapus buffer internal
  u8g2.setFont(u8g2_font_ncenB10_tr);   // Pilih font
  u8g2.drawStr(18, 36, "Penjaga Siap!"); // Tulis string (posisi x, y, teks)
  u8g2.sendBuffer();                    // Kirim buffer ke layar
  delay(2000);
}

void loop() {
  int nilaiRasa = analogRead(soilPin);

  // Memetakan nilai sensor ke rentang persentase 0-100%
  int persentaseLembap = map(nilaiRasa, nilaiUdara, nilaiAir, 0, 100);

  // Memastikan nilai persentase tidak di luar 0-100 (constrain)
  persentaseLembap = constrain(persentaseLembap, 0, 100);

  // --- TAMPILKAN KE OLED ---
  u8g2.clearBuffer(); // Selalu mulai dengan membersihkan buffer

  // Menampilkan persentase kelembapan dengan font besar
  u8g2.setFont(u8g2_font_logisoso18_tr);
  u8g2.setCursor(0, 22);
  u8g2.print("Lembap: ");
  u8g2.print(persentaseLembap);
  u8g2.print("%");

  // Menampilkan status dengan font yang lebih kecil
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 45);

  if (persentaseLembap < 30) {
    u8g2.print("Status: TANAH KERING");
  } else if (persentaseLembap > 85) {
    u8g2.print("Status: TERLALU BASAH");
  } else {
    u8g2.print("Status: KONDISI AMAN");
  }

  u8g2.sendBuffer(); // Perintah wajib untuk menampilkan ke layar

  delay(1000);
}
