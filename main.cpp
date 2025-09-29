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
#include "06_qr.h"


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
    else if (phase == PHASE_QR)		 drawQR();

    //FPS
    showFPS();
	drawQR();

    glutSwapBuffers();
}


void timer(int) {

    music_task();

    updateTimeSystem();


    if (deltaTime < 4) phase = PHASE_INTRO_RISE;
    if ((deltaTime >= 4) && (deltaTime < 8)) phase = PHASE_CREDIT;
    if ((deltaTime >= 8) && (deltaTime < 12)) phase = PHASE_PICTURE;
    if ((deltaTime >= 12) && (deltaTime < 30)) phase = PHASE_BALL;
    if ((deltaTime >= 30) && (deltaTime < 50)) phase = PHASE_PARTICLES;
    if ((deltaTime >= 50) && (deltaTime < 175)) phase = PHASE_SCROLLER;
    if ((deltaTime >= 175) && (deltaTime < 180)) phase = PHASE_QR;
    if (deltaTime >= 180) demo_quit();





    if (phase == PHASE_INTRO_RISE)        updateIntroRise();
    else if (phase == PHASE_CREDIT)       updateIntroCredit();
    else if (phase == PHASE_PICTURE)      updatePicture();
    else if (phase == PHASE_BALL)         updateBall();
    else if (phase == PHASE_PARTICLES)    updateParticles();
    else if (phase == PHASE_SCROLLER)     updateScroller(0.016f);  
	else if (phase == PHASE_QR)			  updateQR();


    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {

    resetBall();


    init_system();
    resetScroller();

    //setScrollerText(R"(In the beginning, God created the world. Stop! In the beginning, God created the Demoscene and he saw that it was good. But then he found it a little lonely and he created the Demoscener. But what's the point of that? I'm Anon Monk and until recently I didn't even have the faintest idea that the Demoscene existed. But why am I here now anyway? It's because I met a nice gentleman who one day said come with me to Bremen for the Nordlicht and then I was at the Nordlicht. I didn't know what to expect but after about an hour I felt very comfortable and met great people. But why the demo? At the Nordlicht I met Titus Rabenauge and he said, "I've got a G3 I can send them to you and then you can do a demo." And that's why I did a demo. I learned a lot. From statements like "Where is my main?" to "You can't program when you're drunk" to "I need to calm down and read everything the AI writes," it was all there. Many thanks to Titus, without him I wouldn't have a G3 for the demo. Many thanks to Key Real, who made me a Demoscener and helped me a lot with the demo. Many thanks to Chat GPT, without whom I certainly would have thrown the G3 out the window several times. And thanks to Corvus, without whom I wouldn't have made this demo at all. Thanks for watching.)");


    loadImage();         // muss mach init sein, denn es benutzt OpenGL

	loadImageQR();
    
    music_start("./Stuff/music/music.wav");
    startTime = glutGet(GLUT_ELAPSED_TIME) * 0.001;
    glutMainLoop();

 
    return 0;

}
