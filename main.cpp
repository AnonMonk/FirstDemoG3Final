#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "main.h"
#include "system.h"
#include "helpers.h"
#include "01_text.h"
#include "02_particles.h"
#include "03_picture.h"
#include "04_ball.h"
#include "05_scroller.h"


Phase phase;


void display() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    if (phase == PHASE_INTRO_RISE)  drawIntroRise();
    else if (phase == PHASE_CREDIT)      drawIntroCredit();
    else if (phase == PHASE_PICTURE)     drawPicture();
    else if (phase == PHASE_BALL)        drawBall();
    else if (phase == PHASE_PARTICLES)   drawParticles();
    else if (phase == PHASE_SCROLLER)    drawScroller();

    showFPS();

    glutSwapBuffers();
}


void timer(int) {

    music_task();

    updateTimeSystem();


    if (deltaTime < 4) phase = PHASE_INTRO_RISE;
    if ((deltaTime >= 4) && (deltaTime < 8)) phase = PHASE_CREDIT;
    if ((deltaTime >= 8) && (deltaTime < 12)) phase = PHASE_PICTURE;
    if ((deltaTime >= 12) && (deltaTime < 32)) phase = PHASE_BALL;
    if ((deltaTime >= 32) && (deltaTime < 52)) phase = PHASE_PARTICLES;
	if ((deltaTime >= 52) && (deltaTime < 62)) phase = PHASE_SCROLLER;
    if (deltaTime >= 62) demo_quit();





    if (phase == PHASE_INTRO_RISE)   updateIntroRise();
    else if (phase == PHASE_CREDIT)       updateIntroCredit();
    else if (phase == PHASE_PICTURE)      updatePicture();
    else if (phase == PHASE_BALL)         updateBall();
    else if (phase == PHASE_PARTICLES)    updateParticles();
    else if (phase == PHASE_SCROLLER)     updateScroller(0.016f);  // ~16 ms pro Frame


    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {

    resetBall();
    resetScroller();


    init_system();

    loadImage();         // muss mach init sein, denn es benutzt OpenGL
    
    music_start("./Stuff/music/music.wav");
    startTime = glutGet(GLUT_ELAPSED_TIME) * 0.001;
    glutMainLoop();

 
    return 0;

}
