#define NOMINMAX
#include "05_scroller.h"
#include <GL/freeglut.h>
#include <string>
#include <algorithm>

// -------------------- Konfig --------------------
static std::string gMessage =
    "Test";

static float gCharDelay   = 0.06f;   // Sekunden pro Buchstabe
static float gScrollSpeed = 0.25f;   // NDC pro Sekunde
static float gY           = 0.0f;  // NDC (−1 .. +1)
static float gFontScale   = 0.0007f; // Skalierung für GLUT_STROKE

// -------------------- State ---------------------
static int   gVisibleChars = 0;
static float gTimer        = 0.0f;
static float gX            = 1.2f;   // Start rechts außerhalb (NDC)
static float gTextWidthNDC = 0.0f;

// Breitenhilfen (Stroke → NDC)
static float strokeTextWidth(const std::string& s) {
    float w = 0.0f;
    for (unsigned char c : s) w += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
    return w;
}
static float strokeToNDC(float strokeWidth) {
    return strokeWidth * gFontScale / 700.0f * 2.0f; // empirisch angenehm
}

void resetScroller() {
    gVisibleChars = 0;
    gTimer        = 0.0f;
    gX            = 1.2f;
    gTextWidthNDC = 0.0f;
}

void updateScroller(float dt) {
    dt = std::max(0.0f, dt);

    // 1) Schreib-Effekt
    if (gVisibleChars < (int)gMessage.size()) {
        gTimer += dt;
        if (gTimer >= gCharDelay) {
            gVisibleChars = std::min((int)gMessage.size(), gVisibleChars + 1);
            gTimer = 0.0f;

            const std::string visible = gMessage.substr(0, gVisibleChars);
            gTextWidthNDC = strokeToNDC(strokeTextWidth(visible));
        }
    }

    // 2) Scroller (Loop)
    gX -= gScrollSpeed * dt;
    if (gX < -1.2f - gTextWidthNDC) gX = 1.2f;
}

void drawScroller() {
    glPushMatrix();
    glTranslatef(gX, gY, 0.0f);
    glScalef(gFontScale, gFontScale, 1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);

    const std::string visible = gMessage.substr(0, gVisibleChars);
    for (unsigned char c : visible) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);

    glPopMatrix();
}