#pragma once


void display();
void update();


void timer(int);


enum Phase {
    PHASE_INTRO_RISE,
    PHASE_CREDIT,
    PHASE_PARTICLES,
    PHASE_PICTURE,
    PHASE_BALL,
    PHASE_SCROLLER
};

extern Phase phase;