#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: build_img <output.img> [payload.bin]\n";
        return 1;
    }
    string out = argv[1];
    string payload;
    if (argc >= 3) payload = argv[2];

    const size_t IMG_SIZE = 1440 * 1024; // 1.44MB floppy
    vector<unsigned char> img(IMG_SIZE, 0);

    // Minimal boot sector: infinite loop (jmp $) and signature
    // 0xEB 0xFE is jmp to current address (infinite)
    img[0] = 0xEB;
    img[1] = 0xFE;
    // Boot signature 0x55AA at offset 510
    img[510] = 0x55;
    img[511] = 0xAA;

    // If payload provided, read it and copy starting at sector 1 (offset 512)
    if (!payload.empty()) {
        ifstream p(payload, ios::binary);
        if (!p) {
            cerr << "Warning: cannot open payload: " << payload << "\n";
        } else {
            p.seekg(0, ios::end);
            size_t sz = (size_t)p.tellg();
            p.seekg(0, ios::beg);
            if (sz + 512 > IMG_SIZE) {
                cerr << "Error: payload too large for 1.44MB image\n";
                return 1;
            }
            p.read((char*)&img[512], sz);
            p.close();
            cout << "Embedded payload: " << payload << " (" << sz << " bytes)\n";
        }
    }

    ofstream outF(out, ios::binary);
    if (!outF) {
        cerr << "Error: cannot create image: " << out << "\n";
        return 1;
    }
    outF.write((char*)img.data(), img.size());
    outF.close();

    cout << "Created floppy image: " << out << " (" << IMG_SIZE << " bytes)\n";
    cout << "Boot sector: jmp $ (infinite loop). Test with: qemu-system-x86_64 -fda " << out << "\n";
    return 0;
}
