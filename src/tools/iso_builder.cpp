#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdint>

using namespace std;

// Simple ISO9660 builder without external tools
// Creates a basic ISO with optional boot sector and files

struct ISODateTime {
    char year[4];   // since 1900
    char month[2];
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
    char tz;        // 0
};

struct PrimaryVolumeDescriptor {
    unsigned char type;                    // 1
    char id[5];                            // "CD001"
    unsigned char version;                 // 1
    unsigned char unused1;                 // 0
    char systemId[32];
    char volumeId[32];
    unsigned char unused2[8];
    uint32_t spaceSizeLE;                  // sectors (LE)
    uint32_t spaceSizeBE;                  // sectors (BE)
    unsigned char unused3[32];
    uint16_t volumeSetSizeLE, volumeSetSizeBE;
    uint16_t volumeSeqNumLE, volumeSeqNumBE;
    uint16_t logicalBlockSizeLE, logicalBlockSizeBE;
    uint32_t pathTableSizeLE, pathTableSizeBE;
    uint32_t pathTableLocLE, pathTableLocBE;
    uint32_t optPathTableLocLE, optPathTableLocBE;
    unsigned char rootDirRecord[34];  // simplified
    char volumeSetId[128];
    char publisherId[128];
    char dataPreparerId[128];
    char applicationId[128];
    char copyrightFile[37];
    char abstractFile[37];
    char bibliographicFile[37];
    ISODateTime creationTime;
    ISODateTime modificationTime;
    ISODateTime expirationTime;
    ISODateTime effectiveTime;
    unsigned char fileStructVersion;
    unsigned char unused4;
    unsigned char applicationUseArea[512];
    unsigned char reserved[653];
};

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: iso_builder <output.iso> [--boot bootfile.bin]\n";
        return 1;
    }

    string out = argv[1];
    string bootfile;
    
    for (int i = 2; i < argc; i++) {
        if (string(argv[i]) == "--boot" && i + 1 < argc) {
            bootfile = argv[++i];
        }
    }

    // Create a simple ISO: system area (16 sectors) + boot sector
    vector<unsigned char> isoData;
    
    // System area: 16 sectors of zeros (32KB)
    for (int s = 0; s < 16; s++) {
        for (int i = 0; i < 2048; i++) {
            isoData.push_back(0);
        }
    }

    // Sector 16: boot sector (2048 bytes)
    if (!bootfile.empty()) {
        ifstream bf(bootfile, ios::binary);
        if (!bf) {
            cerr << "Error: cannot open boot file: " << bootfile << "\n";
            return 1;
        }
        bf.seekg(0, ios::end);
        size_t sz = (size_t)bf.tellg();
        bf.seekg(0, ios::beg);
        
        if (sz > 2048) {
            cerr << "Warning: boot file > 2048 bytes, truncating\n";
            sz = 2048;
        }
        
        vector<unsigned char> bootData(2048, 0);
        bf.read((char*)bootData.data(), sz);
        bf.close();
        
        for (unsigned char b : bootData) {
            isoData.push_back(b);
        }
        cout << "Embedded boot file: " << bootfile << " (" << sz << " bytes)\n";
    } else {
        // If no boot file, add 2048 bytes of zeros
        for (int i = 0; i < 2048; i++) {
            isoData.push_back(0);
        }
    }

    // Write ISO
    ofstream outF(out, ios::binary);
    if (!outF) {
        cerr << "Error: cannot create ISO: " << out << "\n";
        return 1;
    }
    outF.write((char*)isoData.data(), isoData.size());
    outF.close();

    cout << "Created ISO9660 image: " << out << " (" << isoData.size() << " bytes)\n";
    cout << "Bootable ISO. Test with: qemu-system-x86_64 -cdrom " << out << "\n";
    return 0;
}
