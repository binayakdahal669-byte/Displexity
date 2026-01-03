#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

// Minimal bootloader with embedded x86 code to print "Displexity"
// Machine code (raw bytes) for real mode code
int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: bootgen <output.bin>\n";
        return 1;
    }
    string out = argv[1];

    // x86 real mode bootloader machine code
    // This prints "Displexity" using BIOS int 0x10
    vector<unsigned char> bootcode = {
        0xEB, 0x00,                 // jmp short start (skip nop)
        0x90,                       // nop
        // start:
        0xB8, 0x00, 0x00,           // mov ax, 0x0000 (setup data segment)
        0x8E, 0xD8,                 // mov ds, ax
        0x8E, 0xC0,                 // mov es, ax
        0xBE, 0x0E, 0x7C,           // mov si, 0x7C0E (address of message)
        
        // print_loop:
        0xAC,                       // lodsb (load byte from [si], increment si)
        0x84, 0xC0,                 // test al, al (check if null terminator)
        0x74, 0x08,                 // jz done (if zero, jump to done)
        
        0xB4, 0x0E,                 // mov ah, 0x0E (BIOS print char function)
        0xBB, 0x00, 0x00,           // mov bx, 0x0000 (page 0, color)
        0xCD, 0x10,                 // int 0x10 (BIOS video interrupt)
        0xEB, 0xF3,                 // jmp print_loop
        
        // done:
        0xFA,                       // cli (disable interrupts)
        0xF4,                       // hlt (halt)
    };

    // Pad bootcode to start of message area
    size_t msgStart = 0x0E;
    while (bootcode.size() < msgStart) {
        bootcode.push_back(0x90); // nop padding
    }

    // Add message "Displexity\0"
    string msg = "Displexity";
    for (char c : msg) {
        bootcode.push_back((unsigned char)c);
    }
    bootcode.push_back(0x00); // null terminator

    // Pad rest of sector with zeros
    while (bootcode.size() < 510) {
        bootcode.push_back(0x00);
    }

    // Boot signature
    bootcode.push_back(0x55);
    bootcode.push_back(0xAA);

    if (bootcode.size() != 512) {
        cerr << "Error: bootcode is not exactly 512 bytes (got " << bootcode.size() << ")\n";
        return 1;
    }

    ofstream outF(out, ios::binary);
    if (!outF) {
        cerr << "Error: cannot create bootloader: " << out << "\n";
        return 1;
    }
    outF.write((char*)bootcode.data(), bootcode.size());
    outF.close();

    cout << "Created bootloader: " << out << " (512 bytes)\n";
    cout << "Prints: Displexity\n";
    cout << "Test with: qemu-system-x86_64 -fda " << out << " -nographic\n";
    return 0;
}
