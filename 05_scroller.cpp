#include <string>
#include "system.h"
#include "05_scroller.h"

// ---------------- Konfiguration ----------------
static std::string gMessage = R"(In the beginning, God created the world. Stop! In the beginning, God created the Demoscene and he saw that it was good. But then he found it a little lonely and he created the Demoscener. But what's the point of that? I'm Anon Monk and until recently I didn't even have the faintest idea that the Demoscene existed. But why am I here now anyway? It's because I met a nice gentleman who one day said come with me to Bremen for the Nordlicht and then I was at the Nordlicht. I didn't know what to expect but after about an hour I felt very comfortable and met great people. But why the demo? At the Nordlicht I met Titus Rabenauge and he said, "I've got a G3 I can send them to you and then you can do a demo." And that's why I did a demo. I learned a lot. From statements like "Where is my main?" to "You can't program when you're drunk" to "I need to calm down and read everything the AI writes," it was all there. Many thanks to Titus, without him I wouldn't have a G3 for the demo. Many thanks to Key Real, who made me a Demoscener and helped me a lot with the demo. Many thanks to Chat GPT, without whom I certainly would have thrown the G3 out the window several times. And thanks to Corvus, without whom I wouldn't have made this demo at all. Thanks for watching.)";

static float gSpeedPxPerSec = 80.0f;                // Pixel/Sekunde (rechts -> links)
static float gBaselineYPx = 60.0f;                // Baseline in Pixel (von unten)
static void* gFont = GLUT_BITMAP_9_BY_15;  // GLUT font

// ---------------- State ----------------
// gRightPx = position der rechten Textkante (logische Position)
static float gRightPx = 0.0f;
// Gesamtbreite des Textes (Pixel). -1 == noch nicht ermittelt (lazy).
static int   gTextWidth = -1;

// ---------------- Helpers ----------------
static inline void pushOrtho()
{
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, ScreenWidth, 0, ScreenHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
}
static inline void popOrtho()
{
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static int calcTextWidthPx(const std::string& s)
{
    int w = 0;
    const unsigned char* p = (const unsigned char*)s.c_str();
    while (*p) { w += glutBitmapWidth(gFont, *p++); }
    return w;
}

// ---------------- API ----------------
void resetScroller()
{
    // Setze rechte Kante so, dass die gesamte Zeile initial VOLLSTƒNDIG rechts auﬂerhalb liegt:
    // left0 = gRightPx - gTextWidth  -> for fully off-screen: left0 > ScreenWidth
    // => gRightPx = ScreenWidth + gTextWidth + margin. We don't know gTextWidth yet, so:
    // lazy: set gRightPx large; after first calc of gTextWidth, if it's too small adjust to ScreenWidth + gTextWidth.
    gRightPx = (float)ScreenWidth * 2.0f; // sicher weit rechts auﬂerhalb
    gTextWidth = -1;
}

void drawScroller()
{
    // lazy berechnen (glutInit muss vorher gelaufen sein)
    if (gTextWidth < 0) {
        gTextWidth = calcTextWidthPx(gMessage);
        // Falls gRightPx wurde initial groﬂ gew‰hlt, jetzt auf exakt ScreenWidth + gTextWidth setzen,
        // damit die ganze Zeile **vollst‰ndig** rechts auﬂerhalb startet.
        if (gRightPx > (float)ScreenWidth) {
            gRightPx = (float)ScreenWidth + (float)gTextWidth;
        }
        if (gTextWidth <= 0) return;
    }

    // Wenn die logische rechte Kante (gRightPx) ist die Referenz. Die sichtbare rechte Kante ist R = min(gRightPx, ScreenWidth).
    float R = gRightPx;
    if (R > (float)ScreenWidth) R = (float)ScreenWidth;
    if (R < 0.0f)               R = 0.0f;

    // Linke Kante der gesamten Textzeile (wenn vollst‰ndig sichtbar): left0 = gRightPx - gTextWidth
    float left0 = gRightPx - (float)gTextWidth;
    // Sichtfenster: wir wollen nur das Intervall [visL..visR] = [max(0,left0), R]
    float visL = left0;
    if (visL < 0.0f) visL = 0.0f;
    float visR = R;

    if (visL >= visR) return; // nichts sichtbar

    // Finde ersten (teilweise) sichtbaren Buchstaben i: kleinste i mit pos_i + cw_i > visL
    size_t i = 0;
    float pos = left0; // linke Kante des 0-ten Zeichens
    const unsigned char* s = (const unsigned char*)gMessage.c_str();
    while (s[i]) {
        int cw = glutBitmapWidth(gFont, s[i]);
        if (pos + (float)cw > visL) break;
        pos += (float)cw;
        ++i;
    }

    // pos ist die Pixel-Position (linke Kante) des Zeichens s[i].
    // Rasterposition: setze immer innerhalb des Bildes, auf >= visL (Ganzzahlig)
    float rasterX = pos;
    if (rasterX < visL) rasterX = visL;
    // round to int to avoid subpixel raster pos jitter
    int rasterXi = (int)(rasterX + 0.5f);
    int baselineI = (int)(gBaselineYPx + 0.5f);
    if (baselineI < 0) baselineI = 0;
    if (baselineI > ScreenHeight - 1) baselineI = ScreenHeight - 1;

    pushOrtho();
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.f, 1.f, 0.f);

    // setze g¸ltige Rasterposition (immer im Bild) - kein Invalid RasterPos
    glRasterPos2i(rasterXi, baselineI);

    // Wenn rasterXi > pos, wir m¸ssen links um (rasterXi - pos) verschieben: benutz glBitmap mit xmove negativ.
    // Statt glBitmap trick nutzen wir: wenn rasterXi > pos, wir zuerst glBitmap(0,0,0,0, delta,0,NULL) mit delta = pos - rasterXi
    int delta = (int)(pos)-rasterXi; // normalerweise <= 0
    if (delta != 0) {
        // verschiebung per glBitmap (keine Pixel, nur move)
        glBitmap(0, 0, 0, 0, (GLfloat)delta, 0.0f, NULL);
    }

    // Zeichne Zeichen bis visR (rechts)
    float pen = pos;
    for (; s[i]; ++i) {
        int cw = glutBitmapWidth(gFont, s[i]);
        if (pen >= visR) break; // rechts schon auﬂerhalb
        glutBitmapCharacter(gFont, s[i]); // GLUT verschiebt den Rasterpos intern um cw
        pen += (float)cw;
    }

    popOrtho();
}

void updateScroller(float dt)
{
    // Rechts->Links: gRightPx verringern
    gRightPx -= gSpeedPxPerSec * dt;

    // Reset: sobald die komplette Zeile links drauﬂen ist (rechte Kante <= 0),
    // setze so, dass die ganze Zeile wieder vollst‰ndig rechts auﬂerhalb ist:
    if (gTextWidth > 0) {
        if (gRightPx <= 0.0f) {
            gRightPx = (float)ScreenWidth + (float)gTextWidth; // vollst‰ndig rechts auﬂerhalb
        }
    }
}

// Optional-Setter
void setScrollerText(const char* text)
{
    if (!text) return;
    gMessage = text;
    gTextWidth = -1;
    // sicher neu rechts starten:
    gRightPx = (float)ScreenWidth + 1000.0f;
}
void setScrollerSpeed(float px_per_sec) { gSpeedPxPerSec = (px_per_sec < 0.f) ? -px_per_sec : px_per_sec; }
void setScrollerBaseline(float y_pixels) { gBaselineYPx = y_pixels; }
