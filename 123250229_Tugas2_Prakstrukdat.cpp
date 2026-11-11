#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
using namespace std;

string keKecil(string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

string bacaTeks(const string &prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

struct NodeAntrian {
    string pelanggan;
    NodeAntrian *next;
};

class Queue {
private:
    NodeAntrian *depan;
    NodeAntrian *belakang;

public:
    Queue() : depan(NULL), belakang(NULL) {}
    ~Queue() {
        while (!kosong()) dequeue();
    }
    Queue(const Queue &) = delete;
    Queue &operator=(const Queue &) = delete;

    bool kosong() const { return depan == NULL; }

    void enqueue(const string &pelanggan) {
        NodeAntrian *baru = new NodeAntrian{pelanggan, NULL};
        if (kosong()) depan = belakang = baru;
        else {
            belakang->next = baru;
            belakang = baru;
        }
    }

    string dequeue() {
        if (kosong()) return "";
        NodeAntrian *hapus = depan;
        string nama = hapus->pelanggan;
        depan = depan->next;
        if (depan == NULL) belakang = NULL;
        delete hapus;
        return nama;
    }

    void kembalikanKeDepan(const string &pelanggan) {
        NodeAntrian *baru = new NodeAntrian{pelanggan, depan};
        depan = baru;
        if (belakang == NULL) belakang = baru;
    }

    void tampil(const string &namaMenu) const {
        if (kosong()) {
            cout << "Antrian kosong" << endl;
            return;
        }
        for (NodeAntrian *p = depan; p != NULL; p = p->next)
            cout << p->pelanggan << " menunggu menu " << namaMenu << endl;
    }
};

enum JenisAksi { AKSI_PESAN, AKSI_UPDATE };

struct Aksi {
    JenisAksi jenis;
    string namaMenu;
    string pelanggan; 
};

struct NodeStack {
    Aksi data;
    NodeStack *next;
};

class Stack {
private:
    NodeStack *top;

public:
    Stack() : top(NULL) {}
    ~Stack() {
        Aksi tmp;
        while (!kosong()) pop(tmp);
    }
    Stack(const Stack &) = delete;
    Stack &operator=(const Stack &) = delete;

    bool kosong() const { return top == NULL; }

    void push(const Aksi &a) {
        NodeStack *baru = new NodeStack{a, top};
        top = baru;
    }

    bool pop(Aksi &a) {
        if (kosong()) return false;
        NodeStack *hapus = top;
        a = hapus->data;
        top = top->next;
        delete hapus;
        return true;
    }
};

struct Menu {
    string nama;
    string kategori;
    int harga;
    bool tersedia;
    Queue antrian; 

    Menu(const string &n, const string &k, int h)
        : nama(n), kategori(k), harga(h), tersedia(true) {}
};

struct NodePTB {
    Menu data;
    NodePTB *kiri;
    NodePTB *kanan;

    NodePTB(const string &n, const string &k, int h)
        : data(n, k, h), kiri(NULL), kanan(NULL) {}
};

class PTB {
private:
    NodePTB *akar;

    NodePTB *sisip(NodePTB *node, NodePTB *baru, bool &berhasil) {
        if (node == NULL) {
            berhasil = true;
            return baru;
        }
        string a = keKecil(baru->data.nama);
        string b = keKecil(node->data.nama);
        if (a < b) node->kiri = sisip(node->kiri, baru, berhasil);
        else if (a > b) node->kanan = sisip(node->kanan, baru, berhasil);
        else berhasil = false; // duplikat
        return node;
    }

    NodePTB *lepasTerkecil(NodePTB *node, NodePTB *&terkecil) {
        if (node->kiri == NULL) {
            terkecil = node;
            return node->kanan;
        }
        node->kiri = lepasTerkecil(node->kiri, terkecil);
        return node;
    }

    NodePTB *hapus(NodePTB *node, const string &kunci, bool &berhasil) {
        if (node == NULL) {
            berhasil = false;
            return NULL;
        }
        string k = keKecil(kunci);
        string n = keKecil(node->data.nama);
        if (k < n) {
            node->kiri = hapus(node->kiri, kunci, berhasil);
        } else if (k > n) {
            node->kanan = hapus(node->kanan, kunci, berhasil);
        } else {
            berhasil = true;
            NodePTB *pengganti = NULL;
            if (node->kiri == NULL) pengganti = node->kanan;
            else if (node->kanan == NULL) pengganti = node->kiri;
            else {
                NodePTB *penerus = NULL;
                NodePTB *kananBaru = lepasTerkecil(node->kanan, penerus);
                penerus->kiri = node->kiri;
                penerus->kanan = kananBaru;
                pengganti = penerus;
            }
            delete node; 
            return pengganti;
        }
        return node;
    }

    void inorder(NodePTB *node, int &no) const {
        if (node == NULL) return;
        inorder(node->kiri, no);
        cout << left << setw(4) << no++
             << setw(25) << node->data.nama
             << setw(18) << node->data.kategori
             << setw(12) << node->data.harga
             << (node->data.tersedia ? "Tersedia" : "Habis") << endl;
        inorder(node->kanan, no);
    }

    void hapusSemua(NodePTB *node) {
        if (node == NULL) return;
        hapusSemua(node->kiri);
        hapusSemua(node->kanan);
        delete node;
    }

public:
    PTB() : akar(NULL) {}
    ~PTB() { hapusSemua(akar); }
    PTB(const PTB &) = delete;
    PTB &operator=(const PTB &) = delete;

    bool kosong() const { return akar == NULL; }

    bool tambah(const string &nama, const string &kategori, int harga) {
        NodePTB *baru = new NodePTB(nama, kategori, harga);
        bool berhasil = false;
        akar = sisip(akar, baru, berhasil);
        if (!berhasil) delete baru;
        return berhasil;
    }

    Menu *cari(const string &nama) const {
        string k = keKecil(nama);
        NodePTB *p = akar;
        while (p != NULL) {
            string n = keKecil(p->data.nama);
            if (k == n) return &p->data;
            p = (k < n) ? p->kiri : p->kanan;
        }
        return NULL;
    }

    bool hapusMenu(const string &nama) {
        bool berhasil = false;
        akar = hapus(akar, nama, berhasil);
        return berhasil;
    }

    void tampilSemua() const {
        if (kosong()) {
            cout << "Belum ada menu yang diinputkan" << endl;
            return;
        }
        cout << left << setw(4) << "No" << setw(25) << "Nama Menu"
             << setw(18) << "Kategori" << setw(12) << "Harga" << "Status" << endl;
        cout << string(66, '-') << endl;
        int no = 1;
        inorder(akar, no);
    }
};

void tambahMenu(PTB &menu) {
    string nama = bacaTeks("Nama menu     : ");
    string kategori = bacaTeks("Kategori      : ");
    string hargaStr = bacaTeks("Harga         : ");

    if (nama.empty() || kategori.empty()) {
        cout << "Nama menu dan kategori tidak boleh kosong" << endl;
        return;
    }
    int harga;
    try {
        size_t pos;
        harga = stoi(hargaStr, &pos);
        if (pos != hargaStr.size() || harga < 0) throw 1;
    } catch (...) {
        cout << "Harga tidak valid" << endl;
        return;
    }

    if (menu.tambah(nama, kategori, harga)) 
        cout << "Menu berhasil ditambahkan" << endl;
    else
        cout << "Menu sudah ada di daftar" << endl;
}

void catatPesanan(PTB &menu, Stack &riwayat) {
    string pelanggan = bacaTeks("Nama pelanggan : ");
    string namaMenu = bacaTeks("Nama menu      : ");

    Menu *m = menu.cari(namaMenu);
    if (m == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (m->tersedia) {
        cout << "Pesanan " << pelanggan << " berhasil dibuat" << endl;
        riwayat.push(Aksi{AKSI_PESAN, m->nama, pelanggan});
    } else {
        cout << "Menu sedang habis, " << pelanggan << " masuk ke dalam waiting list" << endl;
        m->antrian.enqueue(pelanggan);
    }
}

void menuTersediaKembali(PTB &menu, Stack &riwayat) {
    string namaMenu = bacaTeks("Nama menu : ");
    Menu *m = menu.cari(namaMenu);
    if (m == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (m->tersedia) {
        cout << "Menu " << m->nama << " masih berstatus Tersedia" << endl;
        return;
    }

    m->tersedia = true;
    cout << "Menu tersedia kembali" << endl;

    string dilayani = "";
    if (!m->antrian.kosong()) {
        dilayani = m->antrian.dequeue();
        cout << "Pesanan langsung diberikan kepada " << dilayani << " dari waiting list" << endl;
    }
    riwayat.push(Aksi{AKSI_UPDATE, m->nama, dilayani});
}

// 5. Hapus menu
void hapusMenu(PTB &menu) {
    string namaMenu = bacaTeks("Nama menu yang dihapus : ");
    if (menu.hapusMenu(namaMenu)) cout << "Menu berhasil dihapus" << endl;
    else cout << "Menu tidak ditemukan" << endl;
}

void undo(PTB &menu, Stack &riwayat) {
    Aksi a;
    if (!riwayat.pop(a)) {
        cout << "Tidak ada aksi untuk di-undo" << endl;
        return;
    }

    if (a.jenis == AKSI_PESAN) {
        cout << "Undo: Pesanan menu " << a.namaMenu << " dibatalkan" << endl;
    } else {
        Menu *m = menu.cari(a.namaMenu);
        if (m == NULL) {
            cout << "Undo gagal: menu " << a.namaMenu << " sudah dihapus dari daftar" << endl;
            return;
        }
        m->tersedia = false;
        // Pelanggan yang tadi langsung dilayani dikembalikan ke posisi awal di waiting list
        if (!a.pelanggan.empty()) m->antrian.kembalikanKeDepan(a.pelanggan);
        cout << "Undo: Menu " << a.namaMenu << " kembali habis" << endl;
    }
}

void tampilAntrian(PTB &menu) {
    string namaMenu = bacaTeks("Nama menu : ");
    Menu *m = menu.cari(namaMenu);
    if (m == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    m->antrian.tampil(m->nama);
}

void tandaiHabis(PTB &menu) {
    string namaMenu = bacaTeks("Nama menu : ");
    Menu *m = menu.cari(namaMenu);
    if (m == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (!m->tersedia) {
        cout << "Menu " << m->nama << " sudah berstatus Habis" << endl;
        return;
    }
    m->tersedia = false;
    cout << "Menu " << m->nama << " ditandai Habis" << endl;
}

int main() {
    PTB menu;
    Stack riwayat;
    string pilihan;

    do {
        cout << "\n===== SISTEM PESANAN RESTORAN =====" << endl;
        cout << "1. Tambah menu" << endl;
        cout << "2. Lihat daftar menu" << endl;
        cout << "3. Catat pesanan pelanggan" << endl;
        cout << "4. Catat menu tersedia kembali" << endl;
        cout << "5. Hapus menu" << endl;
        cout << "6. Undo pesanan / pembaruan menu terakhir" << endl;
        cout << "7. Tampilkan antrian pelanggan" << endl;
        cout << "8. (Tambahan) Tandai menu habis" << endl;
        cout << "0. Keluar" << endl;
        pilihan = bacaTeks("Pilih menu : ");
        cout << endl;

        if (pilihan == "1") tambahMenu(menu);
        else if (pilihan == "2") menu.tampilSemua();
        else if (pilihan == "3") catatPesanan(menu, riwayat);
        else if (pilihan == "4") menuTersediaKembali(menu, riwayat);
        else if (pilihan == "5") hapusMenu(menu);
        else if (pilihan == "6") undo(menu, riwayat);
        else if (pilihan == "7") tampilAntrian(menu);
        else if (pilihan == "8") tandaiHabis(menu);
        else if (pilihan == "0") cout << "Terima kasih" << endl;
        else cout << "Pilihan tidak valid" << endl;
    } while (pilihan != "0");

    return 0;
}