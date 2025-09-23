#pragma once
#include "system.h"   // wie bei deinen anderen Effekten (holt GL/GLUT rein)

// Öffentliche API des Effekts
void resetScroller();
void updateScroller(float dt);
void drawScroller();
