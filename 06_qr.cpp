#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "main.h"
#include "helpers.h"
#include "06_qr.h"

#include <cstdio>    // FILE, fopen
#include <vector>    // Fallback BGR->RGB

// ===== Nur auf Apple: Endian-Helper (PPC/Tiger braucht Little->Host) =====
#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define LE16(x) OSSwapLittleToHostInt16((uint16_t)(x))
#define LE32(x) OSSwapLittleToHostInt32((uint32_t)(x))
#else
#define LE16(x) (x)
#define LE32(x) (x)
#endif
// ========================================================================

// Portable fopen (Windows/Linux/macOS) – identisch zu 03_picture
static FILE* fopen_portable(const char* filename, const char* mode) {
    FILE* f = 0;
#if defined(_MSC_VER) || defined(_WIN32)
    fopen_s(&f, filename, mode);
#else
    f = std::fopen(filename, mode);   // WICHTIG: kein neues "FILE* f" davor!
#endif
    return f;
}

// --------- QR Bild-Daten/State (eigene Namen, keine Kollision mit Picture) ---------
static unsigned char* qrData = nullptr;
static int qrWidth = 0, qrHeight = 0;
static GLuint qrTextureID = 0;   // eigene OpenGL-Textur-ID

#pragma pack(push,1)   // make sure struct is packed
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} MY_BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} MY_BITMAPINFOHEADER;
#pragma pack(pop)

// BMP-Lader – identisch vom Aufbau wie in 03_picture
static unsigned char* loadBMP_QR(const char* filename, int* width, int* height)
{
    FILE* f = fopen_portable(filename, "rb");
    if (!f) { perror("fopen"); return NULL; }

    MY_BITMAPFILEHEADER fileHdr;
    if (fread(&fileHdr, sizeof(fileHdr), 1, f) != 1) {
        fprintf(stderr, "loadBMP_QR: short read on file header '%s'\n", filename);
        fclose(f);
        return NULL;
    }

    // Magic 'BM' (mit Endian-Swap nur auf Apple)
    if (LE16(fileHdr.bfType) != 0x4D42) { // 'BM'
        fprintf(stderr, "Not a BMP file: '%s'\n", filename);
        fclose(f);
        return NULL;
    }

    MY_BITMAPINFOHEADER infoHdr;
    if (fread(&infoHdr, sizeof(infoHdr), 1, f) != 1) {
        fprintf(stderr, "loadBMP_QR: short read on info header '%s'\n", filename);
        fclose(f);
        return NULL;
    }

    // Nur auf Apple: Felder little->host wandeln; sonst unverändert
#if defined(__APPLE__)
    const uint16_t biBitCount = LE16(infoHdr.biBitCount);
    const uint32_t biCompression = LE32(infoHdr.biCompression);
    int w = (int)LE32(infoHdr.biWidth);
    int h = (int)LE32(infoHdr.biHeight);
#else
    const uint16_t biBitCount = infoHdr.biBitCount;
    const uint32_t biCompression = infoHdr.biCompression;
    int w = (int)infoHdr.biWidth;
    int h = (int)infoHdr.biHeight;
#endif

    if (biBitCount != 24 || biCompression != 0) {
        fprintf(stderr, "Only uncompressed 24-bit BMP supported\n");
        fclose(f);
        return NULL;
    }

#if defined(__APPLE__)
    if (h > 0) // Hinweis analog zu 03_picture
        fprintf(stderr, "BMP ist top-down gespeichert (height negativ)\n");
#endif
    if (h < 0) h = -h;

    *width = w;
    *height = h;

    // Each row is padded to 4-byte boundary
    size_t rowSize = ((*width * 3 + 3) / 4) * 4;
    size_t dataSize = rowSize * (size_t)h;

    unsigned char* data = (unsigned char*)malloc(dataSize);
    if (!data) { perror("malloc"); fclose(f); return NULL; }

    // Datenoffset (mit Endian-Swap nur auf Apple)
#if defined(__APPLE__)
    fseek(f, (long)LE32(fileHdr.bfOffBits), SEEK_SET);
#else
    fseek(f, (long)fileHdr.bfOffBits, SEEK_SET);
#endif

    if (fread(data, 1, dataSize, f) != dataSize) {
        fprintf(stderr, "loadBMP_QR: short read on pixel data '%s'\n", filename);
        fclose(f);
        free(data);
        return NULL;
    }
    fclose(f);

    return data; // BGR order, bottom-to-top rows
}

// ---------------------- Öffentliche API (wie 03_picture, aber QR) ----------------------

void loadImageQR() {

    qrData = loadBMP_QR("./Stuff/images/image2.bmp", &qrWidth, &qrHeight);
    if (!qrData) {
        fprintf(stderr, "loadImageQR: failed to load BMP\n");
        return; // keine Textur anlegen, verhindert weiße Fläche
    }

    if (qrTextureID == 0) {
        glGenTextures(1, &qrTextureID);   // GLuint!
    }
    glBindTexture(GL_TEXTURE_2D, qrTextureID);

    // 24-bit BMP hat 3-Byte-Alignment, BMP-Zeilen sind auf 4 gepaddet
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Filter/Wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Texturfarbe unverändert übernehmen (keine Modulation mit glColor/Licht)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

#ifdef GL_BGR
    // BMP liefert BGR
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, qrWidth, qrHeight, 0,
        GL_BGR, GL_UNSIGNED_BYTE, qrData);
#else
    // Fallback: BGR -> RGB umkopieren, falls GL_BGR nicht verfügbar
    std::vector<unsigned char> rgb(qrWidth * qrHeight * 3);
    const unsigned char* src = qrData;
    for (int i = 0; i < qrWidth * qrHeight; ++i) {
        rgb[i * 3 + 0] = src[i * 3 + 2]; // R
        rgb[i * 3 + 1] = src[i * 3 + 1]; // G
        rgb[i * 3 + 2] = src[i * 3 + 0]; // B
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, qrWidth, qrHeight, 0,
        GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
#endif

    free(qrData);
    qrData = nullptr;
}

void drawQR() {

    if (qrTextureID == 0) return;

    glDisable(GL_LIGHTING);          // verhindert Modulation durch Licht
    glColor3f(1.0f, 1.0f, 1.0f);     // Textur nicht einfärben

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, qrTextureID);

    // Fullscreen-Quad in NDC
#if defined(__APPLE__)
    const float t0 = 1.0f, t1 = 0.0f;  // Normal
#else
    const float t0 = 0.0f, t1 = 1.0f;  // Normal
#endif

    glBegin(GL_QUADS);
    glTexCoord2f(0, t0); glVertex2f(-1, -1);
    glTexCoord2f(1, t0); glVertex2f(1, -1);
    glTexCoord2f(1, t1); glVertex2f(1, 1);
    glTexCoord2f(0, t1); glVertex2f(-1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void updateQR() {
    // keine Animation nötig
}
