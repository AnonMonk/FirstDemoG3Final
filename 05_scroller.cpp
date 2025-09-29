#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "system.h"
#include "05_scroller.h"

// -------------------------------------------------
// Konfiguration
// -------------------------------------------------
static const char* gMessage = "In the beginning, God created the world. Stop! In the beginning, God created the Demoscene and he saw that it was good. But then he found it a little lonely and he created the Demoscener. But what's the point of that? I'm Anon Monk and until recently I didn't even have the faintest idea that the Demoscene existed. But why am I here now anyway? It's because I met a nice gentleman who one day said come with me to Bremen for the Nordlicht and then I was at the Nordlicht. I didn't know what to expect but after about an hour I felt very comfortable and met great people. But why the demo? At the Nordlicht I met Titus Rabenauge and he said, I've got a G3 I can send them to you and then you can do a demo. And that's why I did a demo. I learned a lot. From statements like Where is my main? to You can't program when you're drunk to I need to calm down and read everything the AI writes, it was all there. Many thanks to Titus, without him I wouldn't have a G3 for the demo. Many thanks to Key Real, who made me a Demoscener and helped me a lot with the demo. Many thanks to Chat GPT, without whom I certainly would have thrown the G3 out the window several times. And thanks to Corvus, without whom I wouldn't have made this demo at all. Thanks for watching.     ";

static float gScrollX = 0.0f;
static float gSpeedPxPerSec = 112.0f;      // 12% schneller für ca. 15 Sekunden Ersparnis
static float gBaselineY = 240.0f;          // Mittig bei 480px Höhe

// -------------------------------------------------
// Ortho 2D Setup
// -------------------------------------------------
static void pushOrtho()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, ScreenWidth, 0, ScreenHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void popOrtho()
{
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// -------------------------------------------------
// Scroller Funktionen
// -------------------------------------------------
void initScroller()
{
    gScrollX = static_cast<float>(ScreenWidth);
    printf("Scroller initialisiert - großer Text, mittig, von rechts nach links\n");
}

void resetScroller()
{
    gScrollX = static_cast<float>(ScreenWidth);
}

void updateScroller(float deltaTime)
{
    gScrollX -= gSpeedPxPerSec * deltaTime;

    // Reset nach 15000px
    const float RESET_DISTANCE = 15000.0f;

    if (gScrollX < -RESET_DISTANCE)
    {
        gScrollX = static_cast<float>(ScreenWidth);
    }
}

void drawScroller()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 1.0f, 0.0f); // Grün

    pushOrtho();

    float currentX = gScrollX;

    for (const char* c = gMessage; *c != '\0'; ++c)
    {
        // Nur zeichnen wenn sichtbar
        if (currentX > -50.0f && currentX < ScreenWidth + 50.0f)
        {
            glRasterPos2f(currentX, gBaselineY);
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); // Großer Font
        }

        // Nächste Position
        int charWidth = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        if (charWidth <= 0) charWidth = 18; // Fallback
        currentX += static_cast<float>(charWidth);

        // Früh abbrechen wenn zu weit rechts
        if (currentX > ScreenWidth + 100.0f) break;
    }

    popOrtho();
}

void cleanupScroller()
{
    // Nichts zu bereinigen
}