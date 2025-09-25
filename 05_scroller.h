#pragma once

// Initialisiert den Scroller-State (Position etc.)
void resetScroller();

// Zeichnet den Scroller-Text (GLUT-Bitmap-Font)
void drawScroller();

// Aktualisiert die Scrollposition (dt in Sekunden)
void updateScroller(float dt);

// Optionale Setter
void setScrollerText(const char* text);
void setScrollerSpeed(float px_per_sec);
void setScrollerBaseline(float y_pixels);
