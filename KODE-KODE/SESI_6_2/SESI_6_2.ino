/*
 * KODE UNTUK SESI 6.2: MEMBERI WAJAH (LCD)
 * Tujuan: Menampilkan data sensor kelembapan pada layar LCD 16x2 I2C.
 *
 * Deskripsi:
 * Kode ini mengambil data dari sensor dan menampilkannya di layar LCD,
 * membuat Sang Penjaga lebih mandiri tanpa perlu terhubung ke komputer.
 *
 * PENTING:
 * 1. Instal Library: Buka Library Manager di Arduino IDE (Sketch > Include Library > Manage Libraries)
 * dan instal "LiquidCrystal I2C" by Frank de Brabander.
 * 2. Alamat I2C: Alamat umum adalah 0x27 atau 0x3F. Jika layar tidak menyala,
 * coba ganti alamat di baris inisialisasi LCD.
 *
 * Rangkaian (LCD I2C):
 * - VCC layar -> VIN atau 5V pada ESP32
 * - GND layar -> GND pada ESP32
 * - SDA layar -> GPIO 21 pada ESP32
 * - SCL layar -> GPIO 22 pada ESP32
 * - Sensor Kelembapan tetap terhubung seperti di Sesi 5.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Library khusus untuk LCD I2C

// Inisialisasi layar LCD
// Alamat I2C umum adalah 0x27 atau 0x3F. Coba ganti jika layar tidak terdeteksi.
LiquidCrystal_I2C lcd(0x27, 16, 2); // (alamat, kolom, baris)

// Pin sensor kelembapan
const int soilPin = 34;

// Variabel untuk kalibrasi (UBAH NILAI INI SESUAI PENGUKURAN ANDA)
const int nilaiUdara = 4095; // Nilai saat sensor kering
const int nilaiAir = 1800;   // Nilai saat sensor basah tercelup air

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Inisialisasi I2C

  // Inisialisasi layar LCD
  lcd.init();
  lcd.backlight(); // Nyalakan lampu latar

  // Tampilkan pesan awal
  lcd.setCursor(0, 0);
  lcd.print("Penjaga Siap!");
  delay(2000);
}

void loop() {
  int nilaiRasa = analogRead(soilPin);

  // Memetakan nilai sensor ke rentang persentase 0-100%
  int persentaseLembap = map(nilaiRasa, nilaiUdara, nilaiAir, 0, 100);

  // Memastikan nilai persentase tidak di luar 0-100 (constrain)
  persentaseLembap = constrain(persentaseLembap, 0, 100);

  // --- TAMPILKAN KE LCD ---
  lcd.clear(); // Bersihkan layar sebelum menulis data baru

  // Baris pertama: Menampilkan nilai kelembapan
  lcd.setCursor(0, 0); // Kolom 0, Baris 0
  lcd.print("Lembap: ");
  lcd.print(persentaseLembap);
  lcd.print("%");

  // Baris kedua: Menampilkan status kondisi tanah
  lcd.setCursor(0, 1); // Kolom 0, Baris 1
  if (persentaseLembap < 30) {
    lcd.print("Status: KERING");
  } else if (persentaseLembap > 85) {
    lcd.print("Status: BASAH");
  } else {
    lcd.print("Status: AMAN");
  }

  delay(1000);
}
