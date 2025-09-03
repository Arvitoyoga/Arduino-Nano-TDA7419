# Arduino-Nano-TDA7419
=====================================================================
Program Kontrol Audio TDA7419 dengan Arduino
=====================================================================

Deskripsi:
Program ini digunakan untuk mengontrol IC audio processor TDA7419 
menggunakan Arduino. Kontrol dilakukan melalui beberapa tombol dan 
rotary encoder. Selain itu, terdapat input spektrum audio untuk 
visualisasi atau analisis sinyal.

Fitur Utama:
- Tombol Mute (pin 10)       : Membisukan output audio.
- Tombol Select (pin 11)     : Memilih menu/fungsi pada sistem.
- Tombol Setting (pin 13)    : Masuk ke mode pengaturan parameter.
- Rotary Encoder (pin 8, 9, A1):
    - DT (pin 8)             : Data dari encoder.
    - CLK (pin 9)            : Clock dari encoder.
    - SW (pin A1)            : Push button encoder.
- Input Spektrum (pin A0)    : Membaca level spektrum audio.

Tujuan:
Program ini memungkinkan pengguna untuk mengatur fungsi audio 
(Tone control, Volume, Balance, dsb.) pada IC TDA7419 melalui 
interface sederhana berupa tombol dan rotary encoder.
