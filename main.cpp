#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

#define PI 3.14159265
#define toRad 0.01745329251

typedef struct vector {
    float x;
    float y;
    float z;
    vector(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z =z;
    }
    vector() {
        this->x = 0;
        this->y = 0;
        this->z =0;
    }
    void set(float newx, float newy, float newz) {
        x = newx;
        y = newy;
        z = newz;
    }
    void set(vector* vector) {
        x = vector->x;
        y = vector->y;
        z = vector->z;
    }
} vector;

typedef struct quadraple {
    float a;
    float x;
    float y;
    float z;
    quadraple(float a, float x, float y, float z) {
        this-> a = a;
        this->x = x;
        this->y = y;
        this->z =z;
    }
    void set(float a, float x, float y, float z) {
        this-> a = a;
        this->x = x;
        this->y = y;
        this->z =z;
    }
    void set(quadraple* quadraple) {
        this-> a = quadraple->a;
        this->x = quadraple->x;
        this->y = quadraple->y;
        this->z = quadraple->z;
    }
} quadraple;

typedef struct character {
    vector *translation;
    quadraple *rotation;
    quadraple *deepRotation;
    vector *firingInitialTranslation;
    quadraple *firingInitialRotation;
    float trajectoryXrotation;
    float trajectoryYrotation;
    float initialTrajectoryYrotation;
    float bezierTranslation;
    bool isFiring;
    bool hasFired;
    vector bezierTranslationPoints [4];

    character(vector *translation, quadraple *rotation, quadraple *deepRotation, vector *firingInitialTranslation, quadraple *firingInitialRotation) {
        this->translation = translation;
        this->rotation = rotation;
        this->deepRotation = deepRotation;
        this->firingInitialTranslation = firingInitialTranslation;
        this->firingInitialRotation = firingInitialRotation;
        this->trajectoryXrotation = 0;
        this->trajectoryYrotation = this->initialTrajectoryYrotation = 0;
        this->bezierTranslation = 0;
        this->isFiring = false;
        this->hasFired = false;
    }

    void setTranslation(vector toTranslate) {
        translation->x = toTranslate.x;
        translation->y = toTranslate.y;
        translation->z = toTranslate.z;
    }

    void setRotation(quadraple toRotate) {
        rotation->a = toRotate.a;
        rotation->x = toRotate.x;
        rotation->y = toRotate.y;
        rotation->z = toRotate.z;
    }

    void setRotation(float a, float x, float y, float z) {
        rotation->a = a;
        rotation->x = x;
        rotation->y = y;
        rotation->z = z;
    }

    void resetAttrs() {
        this->bezierTranslation = 0;
        this->isFiring = false;
        this->hasFired = false;
    }
} character;

typedef struct scoreBoardTarget {
    vector *translation;
    double radius;
    scoreBoardTarget(vector *translation, double radius) {
        this->translation = translation;
        this->radius = radius;
    }
} scoreBoardTarget;

typedef struct gameStatus {
    string gameMode;
    int currCharacter;
    bool gameOver;
    bool inGameControls;
    bool isReplayMode;
    bool replaying;
    int replayCam;
    int score;
    gameStatus(string gameMode, int currCharacter) {
        this->gameMode = gameMode;
        this->currCharacter = currCharacter;
        this->gameOver = false;
        this->inGameControls = true;
        this->isReplayMode = false;
        this->replayCam = 1;
        this->score = 0;
        this->replaying = false;
    }
    void toggleReplayCam() {
        ++replayCam%=2;
    }
    void reset() {
        inGameControls = true;
        gameOver = false;
        isReplayMode = false;
        replayCam = 1;
        this->replaying = false;
    }
} gameStatus;

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
    vector DefaultEye;
    vector DefaultCenter;
    vector DefaultUp;

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
        vector DefaultEye(eyeX,eyeY,eyeZ);
        vector DefaultCenter(centerX,centerY,centerZ);
        vector DefaultUp(upX,upY,upZ);
        this->DefaultEye = DefaultEye;
        this->DefaultCenter = DefaultCenter;
        this->DefaultUp = DefaultUp;
    }

    void setEye(double eyeX, double eyeY, double eyeZ) {
        this->eyeX = eyeX;
        this->eyeY = eyeY;
        this->eyeZ = eyeZ;
    }

    void setCenter(double centerX, double centerY, double centerZ) {
        this->centerX = centerX;
        this->centerY = centerY;
        this->centerZ = centerZ;
    }

    void set(double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ) {
        this->eyeX = eyeX;
        this->eyeY = eyeY;
        this->eyeZ = eyeZ;
        this->centerX = centerX;
        this->centerY = centerY;
        this->centerZ = centerZ;
        this->upX = upX;
        this->upY = upY;
        this->upZ = upZ;
    }

    void reset(){
        this->eyeX = DefaultEye.x;
        this->eyeY = DefaultEye.y;
        this->eyeZ = DefaultEye.z;
        this->centerX = DefaultCenter.x;
        this->centerY = DefaultCenter.y;
        this->centerZ = DefaultCenter.z;
        this->upX = DefaultUp.x;
        this->upY = DefaultUp.y;
        this->upZ = DefaultUp.z;
    }
} gameCamera;

void replay();

static int windowHeight = GLUT_SCREEN_HEIGHT;
static int windowWidth = GLUT_SCREEN_WIDTH;

gameStatus gameStat("basic", 0);
gameCamera gameCam(0, 0, 180, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

vector mainCharacterTranslation(0,0,120);
vector mainCharacterInitialTranslation(0,0,0);
quadraple mainCharacterRotation(0,0,0,0);
quadraple mainCharacterInitialRotation(0,0,0,0);
quadraple mainCharacterDeepRotation(0,0,0,0);
character mainCharacter(&mainCharacterTranslation, &mainCharacterRotation, &mainCharacterDeepRotation,&mainCharacterInitialTranslation ,&mainCharacterInitialRotation);

vector targetTranslation(0,0,5);
scoreBoardTarget target(&targetTranslation, 20);

void createBullet (character* thisCharacter) {
    glPushMatrix();
        glRotated(thisCharacter->rotation->a ,thisCharacter->rotation->x, thisCharacter->rotation->y, thisCharacter->rotation->z);
        glRotated(thisCharacter->trajectoryXrotation ,1, 0,0);
        glTranslated(thisCharacter->translation->x, thisCharacter->translation->y, thisCharacter->translation->z);
        glRotated(thisCharacter->trajectoryYrotation ,0, 1,0);
        glPushMatrix();
            glRotated(thisCharacter->deepRotation->a ,thisCharacter->deepRotation->x, thisCharacter->deepRotation->y, thisCharacter->deepRotation->z);
            glPushMatrix();
                glColor3f(0.5, 0.5, 0.5);
                GLUquadricObj * qobj4;
                qobj4 = gluNewQuadric();
                gluCylinder(qobj4, 0.2, 2, 3, 100,100);
            glPopMatrix();
        
            glPushMatrix();
                glColor3f(0.5, 0.5, 0.5);
                glTranslated(0, 0, 3);
                GLUquadricObj * qobj3;
                qobj3 = gluNewQuadric();
                gluCylinder(qobj3, 2, 2, 5, 100,100);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.5, 0.5, 0.5);
                glTranslated(0, 0, 5+3);
                GLUquadricObj * qobj2;
                qobj2 = gluNewQuadric();
                gluCylinder(qobj2, 2, 3, 4, 100,100);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.5, 0.5, 0.5);
                glTranslated(0, 0, 5+4+3);
                GLUquadricObj * qobj;
                qobj = gluNewQuadric();
                gluCylinder(qobj, 3, 3, 7, 100,100);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.5, 0.5, 0.5);
                glTranslated(0, 0, 7+5+4+3);
                gluDisk(qobj, 0.001, 3, 32, 32);
            glPopMatrix();
            
            glPushMatrix();
                glTranslated(0, 0, 7+5+4+3);
                glColor3f(0.4, 0.4, 0.4);
                glutSolidTorus(0.2,3.2, 32, 32);
            glPopMatrix();
            
            glPushMatrix();
                glTranslated(0, 0, 7+5+4+3+0.01);
                glColor3f(0.4, 0.4, 0.4);
                glutSolidTorus(0.2,1.8, 32, 32);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

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

void drawGame(scoreBoardTarget* target ,character* character) {
    createRoom();
    createTarget(target);
    createBullet(character);
}

void changeCharacterTrajectoryAimLogic(int direction) {
    bool canIncreaseY = true;
    bool canDecreaseY = true;
    if(mainCharacter.rotation->a >= 24)
        canIncreaseY = false;
    if(mainCharacter.rotation->a <= -24)
        canDecreaseY = false;
    if(direction == 1 && canDecreaseY)
        mainCharacter.setRotation(mainCharacter.rotation->a-2, 0,1,0);
    if(direction == 2 && canIncreaseY)
        mainCharacter.setRotation(mainCharacter.rotation->a+2, 0,1,0);
    if(direction == 3 && mainCharacter.trajectoryXrotation>=-10){
        mainCharacter.trajectoryXrotation-=2;
    }
    if(direction == 4 && mainCharacter.trajectoryXrotation<=10){
        mainCharacter.trajectoryXrotation+=2;
    }
}

void replayFiringCamLogic() {
    if (mainCharacter.isFiring) {
        switch (gameStat.replayCam) {
            case 0:
                gameCam.setCenter(mainCharacter.translation->x, mainCharacter.translation->y, mainCharacter.translation->z);
                break;
            case 1:
                gameCam.setCenter(mainCharacter.translation->x, mainCharacter.translation->y, mainCharacter.translation->z-40);
                gameCam.setEye(mainCharacter.translation->x, mainCharacter.translation->y, mainCharacter.translation->z-10);
                break;
        }
    } else {
        gameCam.reset();
    }
}

bool hitTarget(character* character) {
    return character->translation->z == target.translation->z && character->translation->x > target.translation->x - target.radius && character->translation->x < target.translation->x + target.radius;
}

void calculateScore(character* character){
    if(hitTarget(character) && !gameStat.isReplayMode) {
        cout << "score: " << ++gameStat.score << "\n";
    }
}

bool hitWall(character* character) {
    return (character->translation->z<=3 || character->translation->x<-60 || character->translation->x>60 || character->translation->y<-60 || character->translation->y>60);
}

void characterHit() {
    gameStat.inGameControls = false;
    gameStat.isReplayMode = true;
    for (long i=0; i<999999; i++);
    if(!gameStat.replaying){
        replay();
    }
}

void fireBullet(character* bulletCharacter) {
    bulletCharacter->isFiring = true;
    bulletCharacter->deepRotation->a +=2;
    bulletCharacter->deepRotation->z= 1;
    bulletCharacter->translation->z -= 1*0.5;
    if (bulletCharacter->rotation->a != 0) {
        if(bulletCharacter->rotation->a < 0) {
            float slope = tan ((180+bulletCharacter->rotation->a) * toRad);
            bulletCharacter->translation->x += slope*0.01;
        } else {
            float slope = tan ((180-bulletCharacter->rotation->a) * toRad);
            bulletCharacter->translation->x -= slope*0.01;
        }
    }
    if (bulletCharacter->trajectoryYrotation != 0) {
        if(bulletCharacter->trajectoryYrotation < 0) {
            float slope = tan ((180+bulletCharacter->trajectoryYrotation) * toRad);
            bulletCharacter->translation->x -= slope*0.5;
        } else {
            float slope = tan ((180-bulletCharacter->trajectoryYrotation) * toRad);
            bulletCharacter->translation->x += slope*0.5;
        }
    }
    if(hitTarget(bulletCharacter) || hitWall(bulletCharacter)) {
        calculateScore(bulletCharacter);
        bulletCharacter->isFiring = false;
        characterHit();
    }
}

void fireCharacter() {
    if(!mainCharacter.isFiring){
        mainCharacter.firingInitialTranslation->set(mainCharacter.translation);
        mainCharacter.firingInitialRotation->set(mainCharacter.rotation);
        mainCharacter.initialTrajectoryYrotation = mainCharacter.trajectoryYrotation;
    }
    fireBullet(&mainCharacter);
    if(gameStat.isReplayMode && gameStat.replaying){
        replayFiringCamLogic();
    }
}

void replay() {
    if(!gameStat.inGameControls) {
        mainCharacter.resetAttrs();
        mainCharacter.translation->set(mainCharacter.firingInitialTranslation);
        mainCharacter.rotation->set(mainCharacter.firingInitialRotation);
        mainCharacter.deepRotation->set(0,0,0,0);
        gameStat.replaying = true;
        mainCharacter.trajectoryYrotation = mainCharacter.initialTrajectoryYrotation;
        fireCharacter();
    }
}

void initGame() {
    gameCam.reset();
    gameStat.reset();
    mainCharacter.resetAttrs();
    mainCharacter.trajectoryYrotation = 0;
    mainCharacter.translation->set(0,0,140);
    mainCharacter.rotation->set(0,0,0,0);
    mainCharacter.deepRotation->set(0,0,0,0);
    mainCharacter.trajectoryXrotation = 0;
    target.translation->set(0,0,5);
}

void endGame() {
    gameStat.gameOver = true;
    exit (0);
}

void anim() {
    if(mainCharacter.isFiring) {
        fireCharacter();
    }
    glutPostRedisplay();
}

void passM(int x,int y) {
    float mappedX = (x - (windowWidth/2));
    if(mappedX>-windowWidth/4.2 && mappedX < windowWidth/4.2)
        gameCam.eyeX = mappedX*0.2;
    if(!mainCharacter.isFiring && !gameStat.isReplayMode) {
        if (mappedX*0.4<80 && mappedX*0.4 >-80) {
            mainCharacter.trajectoryYrotation =  mappedX*0.4;
        }
    }
}

void keyUp(unsigned char k, int x,int y) {
    if(gameStat.inGameControls) {
        switch (k) {
            case 'w':
                target.translation->z++;
                break;
            case 's':
                if(target.translation->z>=2) {
                    target.translation->z--;
                }
                break;
            case 'd':
                if(target.translation->x!=(45)) {
                    target.translation->x++;
                }
                break;
            case 'a':
                if(target.translation->x!=(-45)) {
                    target.translation->x--;
                }
                break;
            case 32:
                if(!mainCharacter.hasFired)
                    fireCharacter();
                break;
            case 49:
            case 'j':
                changeCharacterTrajectoryAimLogic(1);
                break;
            case 50 :
            case 'l':
                changeCharacterTrajectoryAimLogic(2);
                break;
            case 'i':
                changeCharacterTrajectoryAimLogic(3);
                break;
            case 'k':
                 changeCharacterTrajectoryAimLogic(4);
                break;
        }
    }

    switch (k) {
        case 27:
            endGame();
            break;
        case 'n':
            initGame();
            break;
        case 'r':
            replay();
            break;
        case 9:
            gameStat.toggleReplayCam();
            break;
    }

    glutPostRedisplay();
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

void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 100 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    
    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.5f };
    GLfloat lightPosition[] = { 0.0f, 60.0f, 70.0f, 0 };
    GLfloat lightDirection[] = { 0.0, -1.0, -1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glMaterialfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
    glMaterialfv(GL_LIGHT0, GL_SHININESS, shininess);
    
    GLfloat l1Diffuse[] = { 0.5f, 0.5f, 0.5f, 0.3f };
    GLfloat l1Ambient[] = { 0.2f, 0.2f, 0.2f, 0.2f };
    GLfloat l1Position[] = { 0.0f, 0.0f, 140.0f, 0 };
    GLfloat l1Direction[] = { 0.0, 0.0, -1.0 };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
    glLightfv(GL_LIGHT1,GL_AMBIENT, l1Ambient);
    glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
        if(!gameStat.gameOver) {
            setupCamera();
            setupLights();
            drawGame(&target, &mainCharacter);
        }
    
    glPopMatrix();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Gun Range");
    glutInitWindowSize(windowWidth, windowHeight);
    glutFullScreen();
    glutDisplayFunc(Display);
    glutIdleFunc(anim);
    glutPassiveMotionFunc(passM);
    glutKeyboardUpFunc(keyUp);
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

    initGame();
    glutMainLoop();
    return 0;
}