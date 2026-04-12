#include <iostream>
#include <vector>
#include <string>
#include <exception>

template <typename T>

int main(){
    T Masukkan = "Kosong";
    // std::cout <<"Menjalankan program..." << std::endl;
        // Memanggil kelas inisiasi dan menampilkan GUI.
        // T class::fungsi(); -> mengembalikan objek yang merupakan tindakan dari pengguna
    while(true){
            switch (Masukkan) {
                // case "Masuk_Start";
                // case "Masuk_Setting";
                // dan lain lain ...
                case "Keluar":
                    std::cout << "Keluar dari program..." << std::endl;
                    // destructor semua objek yang ada
                    return 0;
                default:
                    std::cout << "Masukkan tidak dikenali. Silakan coba lagi." << std::endl;
            }
    }



    return 0;
}