/************************************************************************************************
* Kelompok 2                                                                                    *
* Feri Gunawan 241402013                                                                        *
* Chrisa Grant Banuarea 241402057                                                               *
* Muhammad Faiz Pasaribu 241402118                                                              *
* Tugas Proyek                                                                                  *
* Program C++ untuk mensimulasikan antrian (queue) dengan waktu yang tepat dan random serta     *
* realistis dengan fitur montir yang tersedia.                                                  *
************************************************************************************************/

#include <iostream>
#include <ctime>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <algorithm>

// ====== ANSI COLOR & EFFECT DEFINES ======
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define UNDERLINE   "\033[4m"

#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

#define BRIGHT_YELLOW "\033[1;33m"
#define BRIGHT_BLUE   "\033[1;34m"
#define BRIGHT_CYAN   "\033[1;36m"
#define BRIGHT_WHITE  "\033[1;37m"
#define BRIGHT_MAGENTA "\033[1;35m"
#define BRIGHT_RED    "\033[1;31m"
#define BRIGHT_GREEN   "\033[1;32m"
// =========================================

using namespace std;

struct Customer
{
    int id;
    string nama;
    string motor;
    string plat;
    time_t kedatangan;
    int waktuPelayanan; // dalam menit
    time_t waktuMulai;
    time_t waktuSelesai;
    int assignedMontir;
};

struct Node
{
    Customer data;
    Node *next;
};

struct Montir
{
    int id;
    time_t availableAt;
    Montir(int id) : id(id), availableAt(0) {}
};

class Queue
{
private:
    Node *front;
    Node *rear;
    int count;
    time_t lastArrival;
    vector<Montir> montirs;

public:
    Queue(int jumlahMontir) : front(nullptr), rear(nullptr), count(0), lastArrival(0)
    {
        for (int i = 0; i < jumlahMontir; ++i)
        {
            montirs.push_back(Montir(i + 1));
        }
    }

    ~Queue()
    {
        while (front)
        {
            Node *temp = front;
            front = front->next;
            delete temp;
        }
    }
    void enqueue();
    void processQueue();
    void printQueue();
    void clearQueue();
};

void Queue::enqueue()
{
    Customer c;
    c.id = ++count;

    cout << BRIGHT_CYAN << "\nMasukkan nama pelanggan : " << RESET;
    cin.ignore();
    getline(cin, c.nama);

    cout << BRIGHT_CYAN << "Masukkan jenis motor    : " << RESET;
    getline(cin, c.motor);

    // Generate plat kendaraan: BK + 4 angka acak
    int angka = rand() % 9000 + 1000; // 1000-9999
    c.plat = "BK " + to_string(angka);

    c.waktuPelayanan = rand() % 11 + 5; // antara 5–15 menit

    time_t now = time(0);
    if (!lastArrival)
        lastArrival = now;
    c.kedatangan = lastArrival + (rand() % 6 + 1) * 60;
    lastArrival = c.kedatangan;

    Node *newNode = new Node{c, nullptr};
    if (!rear)
    {
        front = rear = newNode;
    }
    else
    {
        rear->next = newNode;
        rear = newNode;
    }

    cout << BRIGHT_YELLOW << "Plat kendaraan          : " << c.plat << RESET << endl;
    cout << BRIGHT_GREEN << "\nPelanggan berhasil ditambahkan ke antrian!\n" << RESET;
    cout << endl;
    system("pause");
    system("cls");
}

void Queue::processQueue()
{
    if (!front)
    {
        cout << BRIGHT_RED << "Antrian kosong.\n" << RESET;
        system("pause");
        system("cls");
        return;
    }

    Node *current = front;

    while (current)
    {
        // Cari montir yang tersedia paling awal
        auto &montir = *min_element(montirs.begin(), montirs.end(),
                                    [](const Montir &a, const Montir &b)
                                    {
                                        return a.availableAt < b.availableAt;
                                    });

        current->data.waktuMulai = max(current->data.kedatangan, montir.availableAt);
        current->data.waktuSelesai = current->data.waktuMulai + (current->data.waktuPelayanan * 60);
        current->data.assignedMontir = montir.id;

        montir.availableAt = current->data.waktuSelesai;

        current = current->next;
    }

    printQueue();
    system("pause");
    system("cls");
}

void Queue::printQueue()
{
    double totalWait = 0;
    int totalCust = 0;
    Node *temp = front;
    if (!temp)
    {
        cout << BRIGHT_RED << "Antrian kosong.\n" << RESET;
        return;
    }

    auto potongString = [](const string &s, size_t maxLen)
    {
        return s.length() > maxLen ? s.substr(0, maxLen) : s;
    };

    // Header tabel
    cout << BRIGHT_BLUE << BOLD;
    cout << "\n+" << setfill('-') << setw(116) << "+" << endl;
    cout << "|" << setfill(' ') << setw(75) << right << "SERVICE MOTOR CFF - SIMULASI ANTRIAN"
         << setw(42) << "|\n";
    cout << "+" << setfill('-') << setw(116) << "+" << endl;
    cout << RESET;

    cout << BRIGHT_MAGENTA << BOLD;
    cout << setfill(' ') << left;
    cout << "| " << setw(3) << "ID"
         << "| " << setw(20) << "Nama"
         << "| " << setw(15) << "Motor"
         << "| " << setw(12) << "Plat"
         << "| " << setw(12) << "Kedatangan"
         << "| " << setw(10) << "Mulai"
         << "| " << setw(10) << "Selesai"
         << "| " << setw(8) << "Durasi"
         << "| " << setw(8) << "Montir"
         << "|\n";
    cout << RESET;

    cout << BRIGHT_BLUE << "+" << setfill('-') << setw(116) << "+" << RESET << endl;
    cout << setfill(' ') << left;

    int row = 0;
    while (temp)
    {
        Customer &c = temp->data;

        char datang[10], mulai[10], selesai[10];
        strftime(datang, sizeof(datang), "%H:%M", localtime(&c.kedatangan));
        strftime(mulai, sizeof(mulai), "%H:%M", localtime(&c.waktuMulai));
        strftime(selesai, sizeof(selesai), "%H:%M", localtime(&c.waktuSelesai));

        // Baris genap putih, ganjil kuning/orange
        if (row % 2 == 0)
            cout << BRIGHT_WHITE;
        else
            cout << BRIGHT_YELLOW;

        cout << "| " << setw(3) << c.id
             << "| " << setw(20) << potongString(c.nama, 19)
             << "| " << setw(15) << potongString(c.motor, 14)
             << "| " << setw(12) << c.plat
             << "| " << setw(12) << datang
             << "| " << setw(10) << mulai
             << "| " << setw(10) << selesai
             << "| " << setw(8) << (to_string(c.waktuPelayanan) + "m")
             << "| " << setw(8) << c.assignedMontir
             << "|" << RESET << endl;

        totalWait += c.waktuPelayanan;
        totalCust++;
        temp = temp->next;
        row++;
    }

    cout << BRIGHT_BLUE << "+" << setfill('-') << setw(116) << "+" << RESET << endl;

    double avgWait = (totalCust > 0 ? totalWait / totalCust : 0.0);
    if (avgWait >= 60)
    {
        int jam = static_cast<int>(avgWait) / 60;
        int menit = static_cast<int>(avgWait) % 60;
        cout << BRIGHT_CYAN << "Rata-rata waktu tunggu: " << jam << " jam " << menit << " menit" << RESET << endl;
    }
    else
    {
        cout << BRIGHT_CYAN << "Rata-rata waktu tunggu: " << fixed << setprecision(2) << avgWait << " menit" << RESET << endl;
    }

    cout << BRIGHT_MAGENTA << "Total customer: " << totalCust << RESET << endl;
}

void Queue::clearQueue()
{
    while (front)
    {
        Node *temp = front;
        front = front->next;
        delete temp;
    }
    rear = nullptr;
    count = 0;
    lastArrival = 0;
    for (auto &montir : montirs)
    {
        montir.availableAt = 0;
    }
    cout << BRIGHT_GREEN << "\nAntrian telah dikosongkan.\n" << RESET;
    system("pause");
    system("cls");
}

int main()
{
    system("color 0B");
    system("cls");
    srand(time(0));

    int jmlMontir;
    do {
        cout << BRIGHT_CYAN << "Masukkan jumlah montir: " << RESET;
        cin >> jmlMontir;
        if (cin.fail() || jmlMontir <= 0) {
            system("cls"); // Bersihkan layar
            cout << BRIGHT_RED << "Jumlah montir tidak valid. Masukkan angka positif.\n" << RESET;
            cin.clear();                // Reset error flag
            cin.ignore(10000, '\n');    // Buang sisa input sampai newline
            jmlMontir = 0;              // Pastikan tetap tidak valid agar loop lanjut
        }
    } while (jmlMontir <= 0);
    system("cls"); // Bersihkan layar setelah input valid

    Queue antrian(jmlMontir);
    int pilihan;

    do
    {
        cout << BRIGHT_BLUE << BOLD;
        cout << "\n+" << setfill('-') << setw(40) << "+" << endl;
        cout << "| " << setfill(' ') <<  setw(32) << right << "SISTEM ANTRIAN SERVICE MOTOR" << setw(8) << setfill(' ')<< "|\n";
        cout << "+" << setfill('-') << setw(40) << "+" << endl;
        cout << RESET;
        cout << BRIGHT_MAGENTA;
        cout << "| [1] Tambah pelanggan ke antrian       |\n";
        cout << "| [2] Proses antrian                    |\n";
        cout << "| [3] Lihat semua antrian               |\n";
        cout << "| [4] Kosongkan antrian                 |\n";
        cout << "| [5] Keluar                            |\n";
        cout << "+" << setfill('-') << setw(40) << "+" << endl;
        cout << RESET;
        cout << "Pilih menu: ";

        cin >> pilihan;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
            pilihan = 0;
        }

        switch (pilihan)
        {
        case 1:
            antrian.enqueue();
            break;
        case 2:
            antrian.processQueue();
            break;
        case 3:
            antrian.printQueue();
            system("pause");
            system("cls");
            break;
        case 4:
            antrian.clearQueue();
            break;
        case 5:
            cout << BRIGHT_GREEN << "Keluar dari program...\n" << RESET;
            system("color 07"); // Reset color
            system("cls");
            cout << BRIGHT_MAGENTA << "Terima kasih telah menggunakan sistem antrian service motor!\n" << RESET;
            system("pause");
            system("cls");
            break;
        default:
            cout << BRIGHT_RED << "Pilihan tidak valid!\n" << RESET;
            system("pause");
            system("cls");
        }

    } while (pilihan != 5);

    return 0;
}
