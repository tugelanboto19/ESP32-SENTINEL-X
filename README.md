# SENTINEL-X v9.0 (Batch 1) 🛡️

**Sentinel-X** adalah *desk-assistant* robotik berbasis ESP32 yang dirancang untuk menjaga area meja secara otomatis dengan teknologi *Time-of-Flight* (ToF).

## 🚀 Fitur Utama
- **Precision Scanning:** Langkah 15 derajat dengan algoritma *Sequential Sampling*.
- **Target Tracking:** Mengunci objek terdekat secara otomatis selama 3 menit.
- **Prayer Sync:** Integrasi API Aladhan untuk menampilkan jadwal sholat real-time.
- **Environment HUD:** Monitoring suhu sekitar via sensor BMP085.

## 🛠️ Spesifikasi Teknis (Akar-ke-Akar)
- **MCU:** ESP32 Lolin32 Lite.
- **Bus System:** I2C Shared Bus (OLED, ToF, BMP085).
- **Algorithm:** Finite State Machine (FSM) untuk manajemen state Scanning & Locking.
- **Power Management:** ToF laser sleep mode saat status Locked.

## 🛠️ Hardware Blueprint: Anatomi SENTINEL-X
Proyek ini menggunakan kombinasi sensor presisi tinggi dan aktuator mikro yang terintegrasi dalam satu jalur komunikasi I2C Shared Bus. Berikut adalah komponen yang membangun "tubuh" SENTINEL-X:

1. The Brain (Otak Pemroses)
ESP32 Lolin32 Lite: Microcontroller tangguh dengan dual-core processor dan konektivitas WiFi 2.4GHz terintegrasi. Dipilih karena form-factor-nya yang compact, pas untuk masuk ke dalam custom enclosure 25mm.

2. The Eye (Indra Penglihatan)
VL53L0X Time-of-Flight (ToF): Sensor jarak berbasis laser kelas satu. Berbeda dengan ultrasonik, ToF menembakkan foton infra merah untuk menghitung jarak dengan akurasi milimeter, memungkinkan SENTINEL-X melakukan Precision Tracking.

3. The Interface (Visual & HUD)
SSD1306 OLED 128x64: Layar monokrom sebagai pusat informasi (Head-Up Display). Menampilkan animasi mata yang dinamis, jam digital, serta data real-time dari sensor.

4. The Environment (Indra Perasa)
BMP085 / GY-68: Sensor barometrik dan suhu. Digunakan untuk memantau termal area meja kerja Mas Imam secara real-time.

5. The Actuator (Sistem Gerak)
MG90S Micro Servo (Metal Gear): Motor penggerak untuk rotasi radar. Pemilihan metal gear memastikan gerakan "nengok" yang lebih stabil dan tahan lama dibandingkan gear plastik biasa.

6. The Feedback (Sistem Suara)
Active/Passive Buzzer: Memberikan respon audio saat proses booting dan penanda waktu (Hourly Beep).

7. Power & Wiring
Breadboard & Jumper Wires: Jalur distribusi daya dan data.

Custom Box 25mm: Wadah utama yang menampung seluruh komponen secara presisi.

🔌 Wiring Logic (Akar ke Akar)
SENTINEL-X menerapkan efisiensi kabel dengan protokol I2C (Inter-Integrated Circuit).

SDA (Pin 19) & SCL (Pin 22): Menghubungkan OLED, VL53L0X, dan BMP085 secara paralel.

PWM (Pin 26): Jalur khusus sinyal untuk kontrol presisi Servo MG90S.

VCC/GND: Distribusi daya stabil 3.3V - 5V.
