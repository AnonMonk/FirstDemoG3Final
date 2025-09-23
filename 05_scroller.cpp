#if defined(_WIN32)
#define NOMINMAX
#endif

#include "05_scroller.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>

// -------------------- Konfig --------------------
static std::string gMessage =
"Test";

static float gCharDelay = 0.06f;   // Sekunden pro Buchstabe
static float gScrollSpeed = 0.25f;   // NDC pro Sekunde
static float gY = 0.0f;    // 0 = Mitte
static float gFontScale = 0.0007f; // Skalierung fuer GLUT_STROKE

// -------------------- State ---------------------
static int   gVisibleChars = 0;
static float gTimer = 0.0f;
static float gX = 1.2f;   // Start rechts ausserhalb (NDC)
static float gTextWidthNDC = 0.0f;

// Breitenhilfen (Stroke -> NDC)
static float strokeTextWidth(const std::string& s) {
    float w = 0.0f;
    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it) {
        unsigned char c = static_cast<unsigned char>(*it);
        w += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
    }
    return w;
}
static float strokeToNDC(float strokeWidth) {
    return strokeWidth * gFontScale / 700.0f * 2.0f;
}

void resetScroller() {
    gVisibleChars = 0;
    gTimer = 0.0f;
    gX = 1.2f;
    gTextWidthNDC = 0.0f;
}

void updateScroller(float dt) {
    if (dt < 0.0f) dt = 0.0f;

    // 1) Schreib-Effekt
    if (gVisibleChars < (int)gMessage.size()) {
        gTimer += dt;
        if (gTimer >= gCharDelay) {
            gVisibleChars++;
            if (gVisibleChars > (int)gMessage.size())
                gVisibleChars = (int)gMessage.size();
            gTimer = 0.0f;

            const std::string visible = gMessage.substr(0, gVisibleChars);
            gTextWidthNDC = strokeToNDC(strokeTextWidth(visible));
        }
        // return; // auskommentiert = gleichzeitig scrollen
    }

    // 2) Scroller (Loop)
    gX -= gScrollSpeed * dt;
    if (gX < -1.2f - gTextWidthNDC)
        gX = 1.2f;
}

void drawScroller() {
    glPushMatrix();
    glTranslatef(gX, gY, 0.0f);
    glScalef(gFontScale, gFontScale, 1.0f);

    // Gruen
    glColor3f(0.0f, 1.0f, 0.0f);

    const std::string visible = gMessage.substr(0, gVisibleChars);
    for (std::string::size_type i = 0; i < visible.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(visible[i]);
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }

    glPopMatrix();
}
