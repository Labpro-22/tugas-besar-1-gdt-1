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


// Kelas untuk mengelola dadu pada permainan 
class DiceManager{ 
    private : 
        int die1;
        int die2;
    public :
    DiceManager(){}
        std::pair<int, int> rollRandom(){
            die1 = rand() % 6 + 1;
            die2 = rand() % 6 + 1;
            return {die1, die2};
        }
        int getDie1() const { return die1; }
        int getDie2() const { return die2; }
        int getTotal() const { return die1 + die2; }
        bool isDouble() const { return die1 == die2; }
};



// Kelas Game untuk sebagai objek yang ada selama permainan berlangsung, mengelola logika permainan, dan berinteraksi dengan pengguna melalui GUI.
class Game{ // menunggu kelas objek jadi
    private :
    public :
};

class GameEngine{
    private :
        DiceManager dices;
    public :
};

