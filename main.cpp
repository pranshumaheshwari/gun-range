#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

int windowHeight;
int windowWidth;

typedef struct vector3f {
    float x;
    float y;
    float z;
    vector3f(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z =z;
    }
    vector3f() {
        this->x = 0;
        this->y = 0;
        this->z =0;
    }
} vector3f;

typedef struct scoreBoardTarget {
    vector3f *translation;
    double radius;
    scoreBoardTarget(vector3f *translation, double radius) {
        this->translation = translation;
        this->radius = radius;
    }
} scoreBoardTarget;

typedef struct gameCamera {
    double eyeX;
    double eyeY;
    double eyeZ;
    double centerX;
    double centerY;
    double centerZ;
    double upX;
    double upY;
    double upZ;
    vector3f DefaultEye;
    vector3f DefaultCenter;
    vector3f DefaultUp;
    gameCamera(double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ) {
        this->eyeX = eyeX;
        this->eyeY = eyeY;
        this->eyeZ = eyeZ;
        this->centerX = centerX;
        this->centerY = centerY;
        this->centerZ = centerZ;
        this->upX = upX;
        this->upY = upY;
        this->upZ = upZ;
        vector3f DefaultEye(eyeX,eyeY,eyeZ);
        vector3f DefaultCenter(centerX,centerY,centerZ);
        vector3f DefaultUp(upX,upY,upZ);
        this->DefaultEye = DefaultEye;
        this->DefaultCenter = DefaultCenter;
        this->DefaultUp = DefaultUp;
    }
} gameCamera;


gameCamera gameCam(0, 0, 180, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
vector3f targetTranslation(0, 0, 5);
scoreBoardTarget target(&targetTranslation, 20);

void createWall () {
    glPushMatrix();
        glBegin(GL_POLYGON);
            glVertex3f(-70, -70, 0);
            glVertex3f(70, -70, 0.0f);
            glVertex3f(70,70, 0);
            glVertex3f(-70,70, 0);
        glEnd();
    glPopMatrix();
}

void createRoom () {
    // Side
    glPushMatrix();
        glNormal3f(0.6, 0.4, 0.7);
        glColor3f(0,0.6,1);
        glTranslated(0, 0, 0);
        createWall();
    glPopMatrix();
    
    glPushMatrix();
        glNormal3f(-1, 0, 0);
        glColor3f(0.8,0.4,0.4);
        glTranslated(70, 0, 70);
        glRotated(90, 0, 1, 0);
        createWall();
    glPopMatrix();
    
    glPushMatrix();
        glNormal3i(1, 0, 0);
        glColor3f(0.8,0.4,0.4);
        glTranslated(-70, 0, 70);
        glRotated(90, 0, 1, 0);
        createWall();
    glPopMatrix();
    
    // Ceiling
    glPushMatrix();
        glNormal3f(0, -1, 0);
        glColor3f(0.9,0.5,0.3);
        glTranslated(0, 70, 70);
        glRotated(90, 1, 0, 0);
        createWall();
    glPopMatrix();
    
    // Floor
    glPushMatrix();
        glNormal3f(0, 1, 0);
        glColor3f(0.9,0.5,0.3);
        glTranslated(0, -70, 70);
        glRotated(90, 1, 0, 0);
        createWall();
    glPopMatrix();
    
}

void createTarget (scoreBoardTarget* target) {
    GLUquadricObj * qobj;
    qobj = gluNewQuadric();
    glPushMatrix();
        glTranslated(target->translation->x, target->translation->y, target->translation->z);
        glPushMatrix();
            glTranslated(0, 0, 0);
            glPushMatrix();
                glColor3f(1, 0, 1);
                gluDisk(qobj, 0.001, 4, 32, 32);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1, 1, 0);
                gluDisk(qobj, 4, 13, 32, 32);
            glPopMatrix();
            glPushMatrix();
                glColor3f(0, 1, 1);
                gluDisk(qobj, 13, 20, 32, 32);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(1, 1, -1, 1, -1, 1);
    gluPerspective(80, 640 / 480, 0.001, 1000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(gameCam.eyeX, gameCam.eyeY, gameCam.eyeZ, gameCam.centerX, gameCam.centerY, gameCam.centerZ, gameCam.upX, gameCam.upY, gameCam.upZ);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
        setupCamera();
        createRoom();
        createTarget(&target);
    glPopMatrix();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    glutCreateWindow("Gun Range");
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutFullScreen();
    glutDisplayFunc(display);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutSetCursor(GLUT_CURSOR_NONE);
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    
    glutMainLoop();
    return 0;
}