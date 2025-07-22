# Simfoni Penjaga Tanaman

> Sebuah dialog sunyi antara teknologi, awan, dan sejumput nyawa hijau di sudut ruanganmu.
> Baca di [medium](https://medium.com/@azzar_budiyanto/merajut-nadi-kehidupan-sebuah-dialog-sunyi-dengan-tanaman-anda-melalui-teknologi-5fc4e816561a)

## Filosofi Proyek: Mendengarkan Bisikan Bumi

> Di tengah riuh rendah kehidupan modern, sering kali kita lupa untuk berhenti sejenak dan mendengarkan. Kita lupa bahwa kehidupan lain, bahkan yang paling hening seperti sebatang tanaman, memiliki ritme dan kebutuhannya sendiri. Proyek ini lahir dari sebuah kerinduan untuk kembali terhubung, untuk menjalin sebuah jembatan antara dunia kita yang serba cepat dengan dunia mereka yang bertumbuh dalam sabar.
> 
> Ini bukan sekadar alat penyiram otomatis. Ini adalah seorang **penjaga**. Sebuah entitas yang kita ciptakan untuk menjadi telinga kita, merasakan denyut kelembapan tanah yang tak terucap. Ia adalah tangan kita, yang dengan lembut menurunkan embun kehidupan tepat saat dibutuhkan. Dan ia adalah duta kita, yang akan berbisik di genggaman tanganmu, mengabarkan kondisi sang kuntum yang kau rawat, tak peduli sejauh apa pun kau melangkah.
> 
> Mari kita mulai merajut simfoni ini bersama.

## Bagian-Bagian Simfoni

Untuk merangkai sang penjaga, kita memerlukan raga dan jiwa.

### Raga (Perangkat Keras)

* **Otak Sang Penjaga (ESP32):** Sebuah keping silikon yang akan menjadi pusat kesadaran dan logika.
* **Indra Perasa (Sensor Kelembapan Tanah):** Urat saraf yang menjulur ke dalam dekapan bumi untuk merasakan dahaganya.
* **Katup Jantung (Modul Relay 5V):** Gerbang yang akan membuka dan menutup aliran kehidupan (air).
* **Arteri Kehidupan (Pompa Air Mini):** Jantung mekanis yang akan memompa embun kehidupan.
* **Sumber Nadi (Catu Daya):** Sumber energi terpisah untuk pompa air (misalnya, adaptor 5V atau 12V, sesuai kebutuhan pompa).

### Jiwa (Perangkat Lunak & Layanan)

* **Lembar Mantra (Arduino IDE):** Tempat kita akan menuliskan dan menghembuskan mantra kehidupan ke dalam otak ESP32.
* **Awan Kesadaran (Adafruit IO):** Dimensi maya tempat semua informasi dari sang penjaga akan bersemayam dan diolah.
* **Duta Pribadi (Bot Telegram):** Jembatan komunikasi antara dirimu dan sang penjaga.

## 1. Merajut Nadi Kehidupan (Instalasi & Pengkabelan)

Satukan setiap komponen raga, layaknya menyusun organ dalam sebuah tubuh.

* **Indra Perasa (Sensor Kelembapan):**
  * `VCC` → ke pin `3V3` pada ESP32.
  * `GND` → ke pin `GND` pada ESP32.
  * `A0` (Analog) → ke pin `34` pada ESP32.
* **Katup Jantung (Modul Relay):**
  * `VCC` → ke pin `VIN` (5V) pada ESP32.
  * `GND` → ke pin `GND` pada ESP32.
  * `IN` (Sinyal) → ke pin `23` pada ESP32.
* **Arteri Kehidupan (Pompa Air):**
  * Hubungkan kabel positif (`+`) dari catu daya eksternal ke terminal `COM` pada relay.
  * Hubungkan kabel positif (`+`) dari pompa air ke terminal `NO` (Normally Open) pada relay.
  * Satukan kabel negatif (`-`) dari pompa air dan catu daya eksternal.

## 2. Membisikkan Mantra (Konfigurasi Awal)

Sebelum raga dapat hidup, jiwanya harus disiapkan di semesta maya.

### A. Membangun Istana di Awan Kesadaran (Adafruit IO)

1. **Buka Gerbang:** Kunjungi [Adafruit IO](https://io.adafruit.com/) dan ciptakan sebuah akun.
2. **Ciptakan Dua Aliran Data:** Pergi ke **Feeds > New Feed**. Buat dua aliran dengan nama persis seperti ini:
   * `soil-moisture`
   * `water-pump`
3. **Pegang Kunci Rahasia:** Klik ikon kunci kuning (**My Key**). Salin dan simpan **AIO Username** dan **Active AIO Key** Anda. Ini adalah kunci istanamu.

### B. Menciptakan Sang Duta Pribadi (Bot Telegram)

1. **Temui Sang Pencipta:** Di Telegram, mulailah percakapan dengan `@BotFather`.
2. **Mohon Kehidupan Baru:** Kirim perintah `/newbot`. Ikuti petunjuk untuk memberinya nama dan username.
3. **Terima Jiwanya (Token):** BotFather akan memberimu sebuah **Token API**. Salin dan simpan baik-baik.
4. **Temukan Koordinat Hatimu (Chat ID):**
   1. Cari bot ciptaanmu dan kirim pesan apa pun (misal, "Halo").
   2. Buka peramban di komputermu dan kunjungi URL ini (ganti `<YOUR_BOT_TOKEN>` dengan token milikmu): `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
   3. Cari `"chat":{"id": ...}`. Nomor tersebut adalah **Chat ID** Anda. Salin dan simpan.

## 3. Menghidupkan Sang Penjaga (Instalasi Kode)

Kini saatnya menyatukan raga dan jiwa.

1. **Siapkan Lembar Mantra:** Buka Arduino IDE. Buat sebuah proyek baru. Buat dua tab:
   * `main.ino`
   * `config.h`
2. **Salin Mantra:** Salin isi dari berkas `main.ino` dan `config.h` yang telah disediakan ke dalam tab yang sesuai di Arduino IDE.
3. **Isi Jiwanya:** Buka tab `config.h`. Ganti semua nilai `...ANDA` dengan kredensial yang telah Anda kumpulkan (nama WiFi, kata sandi, username AIO, kunci AIO, token bot, dan chat ID).
4. **Serap Ilmu:** Pergi ke **Sketch > Include Library > Manage Libraries...**. Cari dan pasang tiga kitab ilmu ini:
   * `Adafruit MQTT Library`
   * `UniversalTelegramBot`
   * `ArduinoJson`
5. **Hembuskan Napas Kehidupan:** Pilih board "**ESP32 Dev Module**" dan port yang benar, lalu klik tombol **Upload**.

## 4. Dialog dengan Sang Penjaga (Perintah Telegram)

Setelah sang penjaga terjaga, Anda bisa berdialog dengannya melalui bisikan-bisikan ini:

* `/start`: Untuk menyapa sang penjaga dan memulai percakapan.
* `/rasa`: Meminta sang penjaga untuk merasakan denyut kelembapan tanah dan melaporkannya padamu.
* `/embun`: Memberi perintah untuk menurunkan embun kehidupan (menyiram) saat itu juga.
* `/henti`: Perintah darurat untuk menghentikan aliran air yang sedang berjalan.
* `/petunjuk`: Meminta sang penjaga untuk mengingatkanmu kembali akan semua mantra yang ia pahami.

## Aliran Kesadaran (Cara Kerja Sistem)

* **Merasakan:** Setiap interval waktu tertentu, sang penjaga akan merasakan **Getar Kelembapan** tanah.
* **Berbisik ke Awan:** Nilai kelembapan itu kemudian dikirim ke **Awan Kesadaran (Adafruit IO)** untuk disimpan sebagai catatan.
* **Refleks Otomatis:** Jika **Getar Kelembapan** melampaui **Ambang Dahaga**, sang penjaga akan secara otomatis memicu **Tarian Embun** (menyiram) dan memberitahumu.
* **Mendengarkan Panggilanmu:** Kapan pun, sang penjaga selalu mendengarkan bisikan perintahmu melalui **Duta Pribadi (Telegram)** dan akan merespons sesuai mantra yang kau ucapkan.

> Semoga dengan adanya penjaga ini, ikatanmu dengan kehidupan sunyi di sudut ruanganmu menjadi semakin dalam dan penuh makna.
