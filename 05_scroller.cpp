#include <string>
#include "system.h"
#include "05_scroller.h"

// ---------------- Konfiguration ----------------
// Dein kompletter Text (kannst du auch zur Laufzeit via setScrollerText ‰ndern)
static std::string gMessage = R"(In the beginning, God created the world. Stop! In the beginning, God created the Demoscene and he saw that it was good. But then he found it a little lonely and he created the Demoscener. But what's the point of that? I'm Anon Monk and until recently I didn't even have the faintest idea that the Demoscene existed. But why am I here now anyway? It's because I met a nice gentleman who one day said come with me to Bremen for the Nordlicht and then I was at the Nordlicht. I didn't know what to expect but after about an hour I felt very comfortable and met great people. But why the demo? At the Nordlicht I met Titus Rabenauge and he said, "I've got a G3 I can send them to you and then you can do a demo." And that's why I did a demo. I learned a lot. From statements like "Where is my main?" to "You can't program when you're drunk" to "I need to calm down and read everything the AI writes," it was all there. Many thanks to Titus, without him I wouldn't have a G3 for the demo. Many thanks to Key Real, who made me a Demoscener and helped me a lot with the demo. Many thanks to Chat GPT, without whom I certainly would have thrown the G3 out the window several times. And thanks to Corvus, without whom I wouldn't have made this demo at all. Thanks for watching.)";

static float gSpeedPxPerSec = 80.0f;               // Pixel pro Sekunde (links laufend)
static float gBaselineYPx = 60.0f;               // y-Basislinie in Pixel (von unten)
static void* gFont = GLUT_BITMAP_9_BY_15;  // GLUT-Bitmap-Font

// ---------------- State ----------------
static float gXpx = 0.0f;   // aktuelle X-Position in Pixel
static int   gTextWidth = -1;     // Breite in Pixel; -1 = noch nicht berechnet (lazy)

// ---------------- Helpers ----------------
static inline float ndcX(float px) { return (px / (float)ScreenWidth) * 2.0f - 1.0f; }
static inline float ndcY(float py) { return (py / (float)ScreenHeight) * 2.0f - 1.0f; }

static int bitmapWidthPx(const std::string& s, void* font) {
    int w = 0;
    const unsigned char* p = (const unsigned char*)s.c_str();
    while (*p) { w += glutBitmapWidth(font, *p++); }
    return w;
}

// ---------------- API ----------------
void resetScroller() {
    gXpx = (float)ScreenWidth;  // Start rechts auﬂerhalb
    gTextWidth = -1;            // Breite wird beim ersten draw berechnet
}

void drawScroller() {
    // Textbreite erst jetzt (nach glutInit) berechnen
    if (gTextWidth < 0) {
        gTextWidth = bitmapWidthPx(gMessage, gFont);
    }

    glColor3f(0.f, 1.f, 0.f);

    // 1) Sichtbaren Start suchen (Zeichen ¸berspringen, die komplett links drauﬂen sind)
    float penX = gXpx;                         // Stiftposition in Pixel
    size_t startIdx = 0;
    const unsigned char* s = (const unsigned char*)gMessage.c_str();

    while (s[startIdx]) {
        int cw = glutBitmapWidth(gFont, s[startIdx]);
        if (penX + (float)cw > 0.0f) break;    // erstes (teilweise) sichtbares Zeichen
        penX += (float)cw;
        ++startIdx;
    }

    // 2) Rasterposition sicher im sichtbaren Bereich setzen (mind. x = 0 px)
    float safeXpx = (penX < 0.0f) ? 0.0f : penX;
    glRasterPos2f(ndcX(safeXpx), ndcY(gBaselineYPx));

    // 3) Zeichnen bis rechter Bildschirmrand
    float drawnX = safeXpx;
    for (size_t i = startIdx; s[i]; ++i) {
        int cw = glutBitmapWidth(gFont, s[i]);
        if (drawnX > (float)ScreenWidth) break; // rechts komplett raus -> fertig
        glutBitmapCharacter(gFont, s[i]);
        drawnX += (float)cw;
    }
}

void updateScroller(float dt) {
    // Falls Breite noch nicht bekannt, konservativ mit ScreenWidth arbeiten
    const int width = (gTextWidth >= 0) ? gTextWidth : ScreenWidth;

    gXpx -= gSpeedPxPerSec * dt;
    // Ñ<=ì verhindert einen 1-px-Edgecase beim Zur¸cksetzen
    if (gXpx <= -(float)width) {
        gXpx = (float)ScreenWidth;
    }
}

// ---------- optionale Setter ----------
void setScrollerText(const char* text) {
    if (!text) return;
    gMessage = text;
    gTextWidth = -1; // Beim n‰chsten draw neu berechnen
}
void setScrollerSpeed(float px_per_sec) { gSpeedPxPerSec = px_per_sec; }
void setScrollerBaseline(float y_pixels) { gBaselineYPx = y_pixels; }
