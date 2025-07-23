/*
 * KODE UNTUK SESI 3: MANTRA PERTAMA (BLINK)
 * Tujuan: Memastikan ESP32 bekerja dan dapat di-upload kode.
 *
 * Deskripsi:
 * Kode ini adalah program paling dasar untuk mikrokontroler.
 * Tujuannya adalah membuat lampu LED kecil yang ada di papan ESP32
 * (disebut LED_BUILTIN) untuk berkedip (menyala dan mati secara bergantian).
 * Ini membuktikan bahwa kita bisa berkomunikasi dengan "otak" sang penjaga.
 *
 * Cara Kerja:
 * 1. setup(): Fungsi ini berjalan sekali saat ESP32 pertama kali nyala.
 * - Kita mengatur pin LED_BUILTIN sebagai OUTPUT, artinya kita akan mengirim sinyal KELUAR dari pin ini.
 * 2. loop(): Fungsi ini berjalan terus-menerus tanpa henti setelah setup() selesai.
 * - digitalWrite(LED_BUILTIN, HIGH): Mengirim tegangan tinggi ke LED, membuatnya MENYALA.
 * - delay(1000): Berhenti sejenak selama 1000 milidetik (1 detik).
 * - digitalWrite(LED_BUILTIN, LOW): Mengirim tegangan rendah ke LED, membuatnya MATI.
 * - delay(1000): Berhenti lagi selama 1 detik.
 * Proses di dalam loop() ini akan terus berulang.
 */

// Sebagian besar papan ESP32 memiliki LED internal di pin GPIO 2.
// Jika tidak berhasil, coba ganti LED_BUILTIN dengan angka 2.
#define LED_BUILTIN 2

void setup() {
  // Mengatur pin LED sebagai OUTPUT (keluaran)
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Menyalakan LED (HIGH adalah level tegangan untuk menyala)
  digitalWrite(LED_BUILTIN, HIGH);
  // Menunggu selama 1 detik
  delay(1000);
  // Mematikan LED (LOW adalah level tegangan untuk mati)
  digitalWrite(LED_BUILTIN, LOW);
  // Menunggu selama 1 detik
  delay(1000);
}
