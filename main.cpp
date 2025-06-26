#include <bits/stdc++.h>
#include <termios.h>
#include <unistd.h>
using namespace std;
using namespace chrono;

struct KTPData {
    string id, nama, email, gender;
};

const int MAX_KEYS = 4;

struct BPlusTreeNode {
    bool isLeaf;
    vector<string> keys;
    vector<BPlusTreeNode*> children;
    vector<KTPData> values;
    BPlusTreeNode* next;
    BPlusTreeNode(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

struct BPlusTree {
    BPlusTreeNode* root;
    BPlusTree() { root = new BPlusTreeNode(true); }

    void insert(string key, KTPData data) {
        BPlusTreeNode* r = root;
        if (r->keys.size() == MAX_KEYS) {
            auto s = new BPlusTreeNode(false);
            s->children.push_back(r);
            splitChild(s, 0);
            root = s;
        }
        insertNonFull(root, key, data);
    }

    void insertNonFull(BPlusTreeNode* node, string key, KTPData data) {
        if (node->isLeaf) {
            auto it = lower_bound(node->keys.begin(), node->keys.end(), key, [](const string& a, const string& b) {
                return stoi(a) < stoi(b);
            });
            int idx = it - node->keys.begin();
            node->keys.insert(it, key);
            node->values.insert(node->values.begin() + idx, data);
        } else {
            int i = upper_bound(node->keys.begin(), node->keys.end(), key, [](const string& a, const string& b) {
                return stoi(a) < stoi(b);
            }) - node->keys.begin();
            if (node->children[i]->keys.size() == MAX_KEYS) {
                splitChild(node, i);
                if (stoi(key) > stoi(node->keys[i])) ++i;
            }
            insertNonFull(node->children[i], key, data);
        }
    }

    void splitChild(BPlusTreeNode* parent, int idx) {
        BPlusTreeNode* child = parent->children[idx];
        BPlusTreeNode* sibling = new BPlusTreeNode(child->isLeaf);
        int mid = MAX_KEYS / 2;

        if (child->isLeaf) {
            sibling->keys.assign(child->keys.begin() + mid, child->keys.end());
            sibling->values.assign(child->values.begin() + mid, child->values.end());
            child->keys.resize(mid);
            child->values.resize(mid);
            sibling->next = child->next;
            child->next = sibling;
            parent->keys.insert(parent->keys.begin() + idx, sibling->keys[0]);
            parent->children.insert(parent->children.begin() + idx + 1, sibling);
        } else {
            parent->keys.insert(parent->keys.begin() + idx, child->keys[mid]);
            sibling->keys.assign(child->keys.begin() + mid + 1, child->keys.end());
            sibling->children.assign(child->children.begin() + mid + 1, child->children.end());
            child->keys.resize(mid);
            child->children.resize(mid + 1);
            parent->children.insert(parent->children.begin() + idx + 1, sibling);
        }
    }

    bool search(string key, KTPData& result) {
        return searchInternal(root, key, result);
    }

    bool searchInternal(BPlusTreeNode* node, string key, KTPData& result) {
        if (node->isLeaf) {
            for (int i = 0; i < node->keys.size(); ++i)
                if (node->keys[i] == key) {
                    result = node->values[i];
                    return true;
                }
            return false;
        } else {
            int i = upper_bound(node->keys.begin(), node->keys.end(), key, [](const string& a, const string& b) {
                return stoi(a) < stoi(b);
            }) - node->keys.begin();
            return searchInternal(node->children[i], key, result);
        }
    }

    void update(string key, KTPData data) {
        BPlusTreeNode* node = findLeaf(root, key);
        if (!node) return;
        for (int i = 0; i < node->keys.size(); ++i)
            if (node->keys[i] == key)
                node->values[i] = data;
    }

    void remove(string key) {
        BPlusTreeNode* node = findLeaf(root, key);
        if (!node) return;
        for (int i = 0; i < node->keys.size(); ++i)
            if (node->keys[i] == key) {
                node->keys.erase(node->keys.begin() + i);
                node->values.erase(node->values.begin() + i);
                return;
            }
    }

    BPlusTreeNode* findLeaf(BPlusTreeNode* node, string key) {
        while (!node->isLeaf) {
            int i = upper_bound(node->keys.begin(), node->keys.end(), key, [](const string& a, const string& b) {
                return stoi(a) < stoi(b);
            }) - node->keys.begin();
            node = node->children[i];
        }
        return node;
    }
};

vector<KTPData> loadCSV(const string& filename) {
    vector<KTPData> list;
    ifstream file(filename);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string id, fname, lname, email, gender, ip;
        getline(ss, id, ','); getline(ss, fname, ','); getline(ss, lname, ',');
        getline(ss, email, ','); getline(ss, gender, ','); getline(ss, ip, ',');
        list.push_back({id, fname + " " + lname, email, gender});
    }
    return list;
}

void printData(const KTPData& d) {
    cout << "ID     : " << d.id << "\nNama   : " << d.nama
         << "\nEmail  : " << d.email << "\nGender : " << d.gender << endl;
}

int getChoiceWithArrow(const vector<string>& options) {
    int pos = 0;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (true) {
        system("clear");
        cout << "=== PILIH OPSI ===\n";
        for (int i = 0; i < options.size(); ++i)
            cout << (i == pos ? "> " : "  ") << options[i] << endl;
        char c = getchar();
        if (c == '\033') {
            getchar();
            switch(getchar()) {
                case 'A': pos = (pos - 1 + options.size()) % options.size(); break;
                case 'B': pos = (pos + 1) % options.size(); break;
            }
        } else if (c == '\n') break;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return pos;
}

int main() {
    vector<string> datasetFiles = {"100.csv", "500.csv", "1000.csv"};
    vector<KTPData> dataList;
    unordered_map<string, KTPData> hashmap;
    BPlusTree bptree;

    while (true) {
        vector<string> datasets = {"100 Data", "500 Data", "1000 Data", "Keluar"};
        int dataChoice = getChoiceWithArrow(datasets);
        if (dataChoice == 3) break;

        dataList = loadCSV(datasetFiles[dataChoice]);
        hashmap.clear();
        bptree = BPlusTree();

        vector<string> structures = {"HashMap", "B+ Tree", "Kembali"};
        int structChoice = getChoiceWithArrow(structures);
        if (structChoice == 2) continue;

        auto start = high_resolution_clock::now();
        if (structChoice == 0)
            for (auto& d : dataList) hashmap[d.id] = d;
        else
            for (auto& d : dataList) bptree.insert(d.id, d);
        auto end = high_resolution_clock::now();
        cout << "Waktu insert: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
        cout << "Tekan ENTER untuk lanjut...";cin.ignore(); cin.get();

        vector<string> ops = {"Create", "Read", "Update", "Delete", "Kembali"};
        while (true) {
            int op = getChoiceWithArrow(ops);
            if (op == 4) break;
            string id;
            if (structChoice == 0) {
                if (op == 0) {
                    KTPData d;
                    cout << "ID     : "; cin >> d.id;
                    cout << "Nama   : "; cin.ignore(); getline(cin, d.nama);
                    cout << "Email  : "; getline(cin, d.email);
                    cout << "Gender : "; getline(cin, d.gender);
                    if (hashmap.count(d.id)) {
                        cout << "ID sudah ada. Gunakan update untuk mengubah.\n";
                    } else {
                        auto start = high_resolution_clock::now();
                        hashmap[d.id] = d;
                        auto end = high_resolution_clock::now();
                        cout << "Waktu insert: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                    }
                } else if (op == 1) {
                    cout << "Masukkan ID: "; cin >> id;
                    auto start = high_resolution_clock::now();
                    if (hashmap.count(id)) printData(hashmap[id]);
                    else cout << "Tidak ditemukan\n";
                    auto end = high_resolution_clock::now();
                    cout << "Waktu baca: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                } else if (op == 2) {
                    cout << "ID: "; cin >> id;
                    KTPData d;
                    cout << "Nama   : "; cin.ignore(); getline(cin, d.nama);
                    cout << "Email  : "; getline(cin, d.email);
                    cout << "Gender : "; getline(cin, d.gender);
                    auto start = high_resolution_clock::now();
                    hashmap[id] = {id, d.nama, d.email, d.gender};
                    auto end = high_resolution_clock::now();
                    cout << "Waktu update: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                } else if (op == 3) {
                    cout << "ID: "; cin >> id;
                    if (!hashmap.count(id)) {
                        cout << "ID tidak ada\n";
                    } else {
                        auto start = high_resolution_clock::now();
                        hashmap.erase(id);
                        auto end = high_resolution_clock::now();
                        cout << "Waktu hapus: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                    }
                }
            } else {
                if (op == 0) {
                    KTPData d;
                    cout << "ID     : "; cin >> d.id;
                    cout << "Nama   : "; cin.ignore(); getline(cin, d.nama);
                    cout << "Email  : "; getline(cin, d.email);
                    cout << "Gender : "; getline(cin, d.gender);
                    KTPData temp;
                    if (bptree.search(d.id, temp)) {
                        cout << "ID sudah ada. Gunakan update untuk mengubah.\n";
                    } else {
                        auto start = high_resolution_clock::now();
                        bptree.insert(d.id, d);
                        auto end = high_resolution_clock::now();
                        cout << "Waktu insert: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                    }
                } else if (op == 1) {
                    cout << "ID: "; cin >> id;
                    auto start = high_resolution_clock::now();
                    KTPData r;
                    if (bptree.search(id, r)) printData(r);
                    else cout << "Tidak ditemukan\n";
                    auto end = high_resolution_clock::now();
                    cout << "Waktu baca: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                } else if (op == 2) {
                    KTPData d;
                    cout << "ID: "; cin >> id;
                    cout << "Nama   : "; cin.ignore(); getline(cin, d.nama);
                    cout << "Email  : "; getline(cin, d.email);
                    cout << "Gender : "; getline(cin, d.gender);
                    auto start = high_resolution_clock::now();
                    bptree.update(id, {id, d.nama, d.email, d.gender});
                    auto end = high_resolution_clock::now();
                    cout << "Waktu update: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                } else if (op == 3) {
                    KTPData temp;
                    cout << "ID: "; cin >> id;
                    if (!bptree.search(id, temp)) {
                        cout << "ID tidak ada\n";
                    } else {
                        auto start = high_resolution_clock::now();
                        bptree.remove(id);
                        auto end = high_resolution_clock::now();
                        cout << "Waktu hapus: " << duration_cast<microseconds>(end - start).count() << " microsecond\n";
                    }
                }
            }
             cout << "Tekan ENTER untuk lanjut..."; cin.ignore(); cin.get();
        }
    }
    return 0;
}
