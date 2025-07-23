
/*
 * KODE UNTUK SESI 5: MEMBERI INDRA PERASA
 * Tujuan: Membaca data dari sensor kelembapan tanah.
 *
 * Deskripsi:
 * Kode ini akan membaca nilai analog dari sensor kelembapan tanah
 * dan menampilkannya ke Serial Monitor di Arduino IDE.
 * Serial Monitor berfungsi sebagai "jendela" kita untuk melihat "bisikan"
 * atau data yang dikirim oleh ESP32.
 *
 * Rangkaian:
 * - VCC sensor -> 3.3V pada ESP32
 * - GND sensor -> GND pada ESP32
 * - A0 (Analog Out) sensor -> GPIO 34 pada ESP32 (atau pin ADC lainnya seperti 35, 36, 39)
 *
 * Cara Kerja:
 * 1. const int soilPin = 34;: Kita memberi nama 'soilPin' untuk pin GPIO 34 agar mudah diingat.
 * 2. setup():
 * - Serial.begin(115200): Memulai komunikasi serial dengan kecepatan 115200 bps.
 * Angka ini harus sama dengan yang diatur di Serial Monitor.
 * 3. loop():
 * - int nilaiRasa = analogRead(soilPin): Membaca nilai tegangan dari pin sensor.
 * ESP32 memiliki ADC 12-bit, sehingga nilainya akan berkisar dari 0 hingga 4095.
 * Biasanya, nilai tinggi berarti kering, dan nilai rendah berarti basah.
 * - Serial.print("Nilai Kelembapan: "): Mencetak teks ke Serial Monitor.
 * - Serial.println(nilaiRasa): Mencetak nilai yang dibaca dari sensor, lalu pindah ke baris baru.
 * - delay(1000): Memberi jeda 1 detik sebelum membaca dan mencetak lagi.
 */

// Tentukan pin GPIO mana yang terhubung ke output Analog (A0) dari sensor
const int soilPin = 34;

void setup() {
  // Mulai komunikasi serial pada baud rate 115200
  // Pastikan untuk mengatur baud rate yang sama di Serial Monitor Arduino IDE
  Serial.begin(115200);
}

void loop() {
  // Baca nilai analog dari sensor
  // Nilai akan berkisar antara 0 (sangat basah) hingga 4095 (sangat kering), tergantung sensor
  int nilaiRasa = analogRead(soilPin);

  // Cetak teks dan nilai ke Serial Monitor
  Serial.print("Nilai Rasa Tanah: ");
  Serial.println(nilaiRasa);

  // Tunggu 1 detik sebelum pembacaan berikutnya
  delay(1000);
}
