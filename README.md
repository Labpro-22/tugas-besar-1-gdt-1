# Nimonspoli CLI

Nimonspoli adalah permainan Monopoly berbasis C++ dengan mode CLI. README ini menjelaskan cara melakukan build, menjalankan program, dan menggunakan command yang tersedia pada mode terminal.

## Kebutuhan

Pastikan environment memiliki:

- `g++` dengan dukungan C++17
- `make`
- Terminal yang mendukung input/output teks

Mode CLI tidak membutuhkan Raylib. Raylib hanya dibutuhkan untuk build GUI.

## Build CLI

Dari root repository, jalankan:

```bash
make
```

Binary akan dibuat di:

```bash
bin/nimonspoli_cli
```

Untuk build ulang dari awal:

```bash
make rebuild
```

Untuk membersihkan hasil build:

```bash
make clean
```

## Menjalankan CLI

Jalankan langsung:

```bash
./bin/nimonspoli_cli
```

Atau gunakan target make:

```bash
make run
```

Saat program berjalan, CLI akan menampilkan prompt:

```text
>
```

Masukkan command pada prompt tersebut.

## Menu Utama

Command yang tersedia pada menu utama:

| Command | Fungsi |
| --- | --- |
| `NEW_GAME` | Memulai permainan baru dari konfigurasi default |
| `LOAD_GAME <nama_save>` | Memuat permainan tersimpan |
| `MUAT <nama_save>` | Alias untuk load game |
| `EXIT` | Keluar dari program |

Contoh:

```text
NEW_GAME
```

```text
LOAD_GAME save1
```

Jika nama save tidak memakai path, program akan mencari folder save di dalam `data/`. Jadi `LOAD_GAME save1` akan membaca `data/save1`.

## Alur Dasar Permainan

1. Jalankan program.
2. Pilih `NEW_GAME` dari menu utama.
3. Ikuti prompt untuk memasukkan jumlah dan nama pemain.
4. Pada giliran pemain, gunakan command sesuai fase giliran.
5. Gunakan `HELP` kapan saja untuk melihat command yang relevan.

Command yang paling sering digunakan:

```text
LEMPAR_DADU
AKHIRI_GILIRAN
CETAK_PAPAN
CETAK_PROPERTI
HELP
```

## Command Informasi

Command berikut dapat digunakan untuk melihat informasi game:

| Command | Fungsi |
| --- | --- |
| `HELP` | Menampilkan panduan command sesuai fase saat ini |
| `CETAK_PAPAN` | Menampilkan papan permainan |
| `CETAK_AKTA <kode>` | Menampilkan detail akta properti |
| `CETAK_DEED <kode>` | Alias untuk `CETAK_AKTA` |
| `CETAK_PROPERTI` | Menampilkan properti milik pemain aktif |
| `CETAK_KARTU` | Menampilkan kartu kemampuan pemain aktif |
| `CETAK_LOG [n]` | Menampilkan log permainan, opsional `n` log terakhir |

Contoh:

```text
CETAK_AKTA BGR
CETAK_LOG 10
```

## Command Giliran

Command utama pada giliran pemain:

| Command | Fungsi |
| --- | --- |
| `LEMPAR_DADU` | Melempar dadu secara acak |
| `ATUR_DADU <x> <y>` | Mengatur nilai dadu manual untuk testing/demo |
| `AKHIRI_GILIRAN` | Mengakhiri giliran pemain |

Contoh:

```text
LEMPAR_DADU
```

```text
ATUR_DADU 3 4
```

Nilai dadu manual harus berada pada rentang 1 sampai 6.

## Command Kartu Kemampuan

Command kartu kemampuan:

| Command | Fungsi |
| --- | --- |
| `GUNAKAN_KEMAMPUAN <nomor>` | Menggunakan kartu kemampuan berdasarkan nomor kartu |
| `BUANG_KARTU <nomor>` | Membuang kartu kemampuan berdasarkan nomor kartu |
| `CETAK_KARTU` | Menampilkan daftar kartu kemampuan |

Contoh:

```text
CETAK_KARTU
GUNAKAN_KEMAMPUAN 1
BUANG_KARTU 2
```

Catatan:

- Nomor kartu dimulai dari 1.
- Kartu kemampuan umumnya digunakan sebelum pemain melempar dadu.
- Jika pemain sudah menggunakan skill pada giliran yang sama, penggunaan skill lain akan ditolak.

## Command Properti

Command pengelolaan properti:

| Command | Fungsi |
| --- | --- |
| `BANGUN <kode>` | Membangun rumah/hotel pada properti street jika syarat terpenuhi |
| `BUILD <kode>` | Alias untuk `BANGUN`, harus ditulis kapital |
| `GADAI <kode>` | Menggadaikan properti |
| `TEBUS <kode>` | Menebus properti yang sedang digadai |
| `FESTIVAL <kode>` | Memilih properti untuk efek Festival |

Contoh:

```text
BANGUN BGR
GADAI BGR
TEBUS BGR
FESTIVAL BGR
```

Catatan:

- Kode properti sebaiknya ditulis kapital.
- `BANGUN` hanya valid jika pemain memenuhi syarat kepemilikan grup warna dan uang cukup.
- `TEBUS` hanya valid untuk properti yang sedang digadai.
- `FESTIVAL` hanya digunakan ketika pemain mendarat di tile Festival dan diminta memilih properti.

## Save dan Load

### Save

Gunakan:

```text
SIMPAN <nama_save>
```

Contoh:

```text
SIMPAN save1
```

Jika nama save tidak memakai path, data akan disimpan ke:

```text
data/save1
```

Catatan penting:

- `SIMPAN` hanya dapat dilakukan di awal giliran.
- Save tidak dapat dilakukan setelah pemain melakukan aksi, melempar dadu, menggunakan skill, atau sedang memiliki pending Festival.
- Jika folder save sudah ada, program akan meminta konfirmasi overwrite.

### Load

Load dilakukan dari menu utama:

```text
LOAD_GAME <nama_save>
```

Atau:

```text
MUAT <nama_save>
```

Contoh:

```text
LOAD_GAME save1
```

## Mode Khusus

Beberapa command hanya digunakan pada situasi tertentu.

### Lelang

Saat properti masuk lelang:

| Command | Fungsi |
| --- | --- |
| `BID <jumlah>` | Memberi penawaran |
| `PASS` | Lewat dari lelang |
| `LEWAT` | Alias untuk `PASS` |

Contoh:

```text
BID 500
PASS
```

### Penjara

Saat pemain berada di penjara:

```text
LEMPAR_DADU
ATUR_DADU <x> <y>
```

Pemain mencoba keluar dengan mendapatkan double. Setelah beberapa percobaan sesuai aturan game, denda penjara dapat diproses otomatis.

### Konfirmasi dan Pilihan Angka

Pada beberapa kondisi, program meminta input sederhana:

| Input | Fungsi |
| --- | --- |
| `y` / `n` | Konfirmasi beli, overwrite save, atau aksi tertentu |
| angka | Memilih opsi, misalnya pada pajak, penjara, atau likuidasi |

## Tips Penggunaan

- Ketik `HELP` jika bingung command apa yang tersedia pada fase saat ini.
- Gunakan `CETAK_PROPERTI` untuk melihat aset pemain aktif.
- Gunakan `CETAK_AKTA <kode>` sebelum membeli atau mengelola properti.
- Gunakan `CETAK_LOG [n]` untuk meninjau aksi terakhir.
- Gunakan `ATUR_DADU <x> <y>` saat demo atau testing agar hasil dadu dapat dikontrol.

## Troubleshooting

### `make` gagal karena compiler tidak ditemukan

Pastikan `g++` dan `make` sudah terpasang.

### Program gagal membaca konfigurasi

Pastikan folder berikut tersedia:

```text
data/config/default
```

Folder tersebut harus berisi file konfigurasi seperti `property.txt`, `railroad.txt`, `utility.txt`, `tax.txt`, `special.txt`, `misc.txt`, dan `aksi.txt`.

### Save tidak bisa dilakukan

Pastikan command `SIMPAN <nama_save>` dijalankan pada awal giliran sebelum melakukan aksi apapun.

### Load tidak menemukan save

Jika menggunakan:

```text
LOAD_GAME save1
```

pastikan folder berikut ada:

```text
data/save1
```

## Command Cepat

```bash
make
make run
make clean
make rebuild
```

```text
NEW_GAME
HELP
LEMPAR_DADU
ATUR_DADU 1 1
CETAK_PAPAN
CETAK_PROPERTI
CETAK_KARTU
AKHIRI_GILIRAN
SIMPAN save1
LOAD_GAME save1
EXIT
```
