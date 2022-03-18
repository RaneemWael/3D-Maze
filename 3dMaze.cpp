//Includes
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <string>
#include <iostream>
#include <ctime>
#include <cmath>
#include <random>
#include <windows.h>
#include <mmsystem.h>
using namespace std;
//-----------------

//Defines
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
//-----------------


//Classes
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 0.5f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.5f, float centerY = -1.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void reset() {
		eye = Vector3f(0.5, 1.0, 1.0);
		center = Vector3f(0.5, -1.0, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void top() {
		eye = Vector3f(0.5, 1.5, 0.5);
		center = Vector3f(0.5, -6.0, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void front() {
		eye = Vector3f(0.5, 0.5, 1.5);
		center = Vector3f(0.5, 0.0, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void side() {
		eye = Vector3f(1.5, 0.5, 0.5);
		center = Vector3f(0.5, 0.0, 0.5);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void first(double charaX, double charaZ) {
		eye = Vector3f(0.7, 0.2, 0.88);
		center = Vector3f(0.7, 0.0, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);

		moveX(charaX);
		moveZ(charaZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};
//-------------------------------------------------------------------

//Methods Signatures
void displayText(int x, int y, float r, float g, float b, char* string);
void TimeNow(int value);
void Keyboard(unsigned char key, int x, int y);
void drawCharacter();
void drawGoal();
void drawLanes();
void drawBridges();
void drawCoins();
void drawPowerUps();
void drawRocks();
void Anim();
void Display();
void Timer(int value);
void drawFence();
void myInit();
void drawGround();
void drawGrass();
void drawTrees();
void drawGoal();
void setupCamera();
void setupLights();
void Special(int key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
float randf(float a, float b);
void load();
//-----------------

//Global Variables	
GLuint tex;
GLuint texWood;
GLuint texClouds;
GLuint texMetal;
GLuint texRocks;
GLuint texDarkSky;
GLTexture tex_ground;
Model_3DS model_tree;
int rep = 1;
int timeLeft = 300;
int windowX = 640;
int windowY = 480;
int score = 0;
int level = 1;
int coinRotation = 0;
Camera camera;
bool firstCam = false;
//both
double bridgesX[6 - 1];
double bordersZ[6 * 2];
const int coins = 4;
int coinsLane[coins];
double coinsX[coins];
double power1X;
int power1Lane;
double power2X;
int power2Lane;
double power3X;
int power3Lane;
int currentLane = 5;
bool inBridge = false;
bool p1Used = false;
bool p2Used = false;
bool p3Used = false;
bool up = false;
bool goalTaken = false;
double charaX = 0.0;
double charaZ = 0.0;
double charaDimX[2];
double charaDimZ[2];
//-----------------

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

float randf(float a, float b) {
	float r = ((b - a) * ((float)rand() / RAND_MAX)) + a;
	return r;
}

void displayText(int x, int y, float r, float g, float b, char* string) {
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void TimeNow(int value) {
	timeLeft--;
	glutPostRedisplay();

	//recall the Timer function after 1 seconds (1,000 milliseconds)
	glutTimerFunc(1 * 1000, TimeNow, 0);
}

void Timer(int value) {

	int lanes;
	if (level == 1)
		lanes = 5;
	else
		lanes = 6;

	srand((unsigned)time(0));
	float test = randf(0.1, 0.9);

	//set bridges' start point to a random number between 0.1 and 0.9
	for (int x = 1; x < lanes; x++) {
		double randX = randf(0.1, 0.9);
		bridgesX[x - 1] = randX;
	}

	//set power1Lane and power2Lane with random values between 1 and max number of lanes
	power1Lane = (rand() % (lanes - 1)) + 1;
	power2Lane = (rand() % (lanes - 1)) + 1;
	power3Lane = (rand() % (lanes - 1)) + 1;

	//set power1X and power2X with random values between 0.1 and 0.9
	double randPower1X;
	double randPower2X;
	double randPower3X;
	double rejects[coins + 3];
	int rejectsSpace = 0;

	for (int i = 0; i < coins; i++) {
		if (power1Lane == coinsLane[i])
			rejects[rejectsSpace++] = coinsX[i];
	}

	randPower1X = randf(0.1, 0.9);
	while ((find(begin(rejects), end(rejects), randPower1X)) != end(rejects))
		randPower1X = randf(0.1, 0.9);
	rejects[rejectsSpace++] = randPower1X;
	power1X = randPower1X;

	randPower2X = randf(0.1, 0.9);
	while ((find(begin(rejects), end(rejects), randPower2X)) != end(rejects))
		randPower2X = randf(0.1, 0.9);
	power2X = randPower2X;

	randPower3X = randf(0.1, 0.9);
	while ((find(begin(rejects), end(rejects), randPower3X)) != end(rejects))
		randPower3X = randf(0.1, 0.9);
	power3X = randPower3X;
	//---------------------------------------------------------------------

	if (inBridge) {
		if (up) {
			charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
			charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
			charaDimZ[0] = charaDimZ[1] - 0.03;
			currentLane--;
		}
		else {
			charaZ += (bordersZ[(currentLane) * 2 - 1] - charaDimZ[0]);
			charaDimZ[0] = bordersZ[(currentLane) * 2 - 1];
			charaDimZ[1] = charaDimZ[0] + 0.03;
		}
		inBridge = false;
	}

	//ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();

	//recall the Timer function after 10 or 20 seconds (20,000 milliseconds)
	if (level == 1)
		glutTimerFunc(20 * 1000, Timer, 0);
	else
		glutTimerFunc(10 * 1000, Timer, 0);
}

//animation key for each object
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	float a = 1.0;

	switch (key) {
		//camera keys
	case 'w': {
		camera.moveY(d);
		firstCam = false;
		setupCamera();
	}
			break;
	case 's': {
		camera.moveY(-d);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'a': {
		camera.moveX(d);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'd': {
		camera.moveX(-d);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'q': {
		camera.moveZ(d);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'e': {
		camera.moveZ(-d);
		firstCam = false;
		setupCamera();
	}
			break;
			//camera rotations
	case 'i': {
		camera.rotateX(a);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'k': {
		camera.rotateX(-a);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'j': {
		camera.rotateY(a);
		firstCam = false;
		setupCamera();
	}
			break;
	case 'l': {
		camera.rotateY(-a);
		firstCam = false;
		setupCamera();
	}
			break;
			//side
	case 'f': {
		camera.front();
		firstCam = false;
		setupCamera();
	}
			break;
			//top
	case 't': {
		camera.top();
		firstCam = false;
		setupCamera();
	}
			break;
			//front
	case 'g': {
		camera.side();
		firstCam = false;
		setupCamera();
	}
			break;
			//first person
	case 'p': {
		camera.first(-charaX, -charaZ);
		firstCam = true;
		setupCamera();
	}
			break;
	}

	//glutPostRedisplay();
}

void Special(int key, int x, int y) {

	int lanes;
	if (level == 1)
		lanes = 5;
	else
		lanes = 6;

	switch (key) {
	case GLUT_KEY_UP:

		up = true;

		if (!(charaDimZ[0] - 0.01 <= 0.02)) {

			//in bridge
			if (inBridge) {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaZ -= 0.01;
				charaDimZ[0] -= 0.01;
				charaDimZ[1] -= 0.01;

				if (firstCam)
					camera.moveZ(0.01);


				if (charaDimZ[1] <= bordersZ[(currentLane - 1) * 2 - 2]) {
					currentLane--;
					inBridge = false;
				}
			}
			//not in bridge
			else if (charaDimZ[0] - 0.01 <= bordersZ[currentLane * 2 - 1]) {
				if (currentLane != 1) {
					if (charaDimX[0] >= bridgesX[currentLane - 2] - 0.075 && charaDimX[1] <= bridgesX[currentLane - 2] + 0.075) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
						charaZ -= 0.01;
						charaDimZ[0] -= 0.01;
						charaDimZ[1] -= 0.01;
						inBridge = true;

						if (firstCam)
							camera.moveZ(0.01);
					}

					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
					return;
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaZ -= 0.01;
				charaDimZ[0] -= 0.01;
				charaDimZ[1] -= 0.01;

				if (firstCam)
					camera.moveZ(0.01);
			}

			//check if goal taken
			if ((charaDimZ[0] <= 0.14 && charaDimX[1] >= 0.19 && charaDimX[1] <= 0.2) || (charaDimZ[0] <= 0.14 && charaDimX[0] >= 0.2 && charaDimX[0] <= 0.21) || (charaDimZ[0] <= 0.14 && charaDimX[0] < 0.2 && charaDimX[1] > 0.2)) {
				if (level == 1) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);

					level = 2;
					lanes = 6;
					currentLane = 6;
					p1Used = false;
					p2Used = false;
					p3Used = false;

					//intialize character dimensions
					charaX = 0.0;
					charaZ = 0.0;
					charaDimX[0] = 0.665;
					charaDimX[1] = 0.735;
					charaDimZ[0] = 0.885;
					charaDimZ[1] = 0.915;

					//intialize coinsLane array with random values between 1 and max number of lanes
					for (int x = 1; x <= coins; x++) {
						int randX = (rand() % (6 - 1)) + 1;
						coinsLane[x - 1] = randX;
					}

					//intialize coinsX array with random values between 0.1 and 0.9
					for (int x = 1; x <= coins; x++) {
						double randCoinX;
						bool final = false; 

						if (coinsLane[x - 1] == (6 - 1)) {

							//to prevent overlapping of coins
							double rejects[coins + 3];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}

							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}
						else {

							//to prevent overlapping of coins
							double rejects[coins + 3];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}
							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}

						coinsX[x - 1] = randCoinX;
					}
					//-----------------------------------------------------------------------------------------

					//intiallize bordersZ
					double lane = 1.0;
					for (int i = 0; i < lanes * 2; i += 2) {
						if (lane == 6.0) {
							bordersZ[i] = 0.99;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						else if (lane == 1.0) {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = 0.02;
						}
						else {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						lane++;
					}
					//-----------------------------------------------------------------
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					goalTaken = true;
					camera.reset();
				}
			}

			//to catch coins
			if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.015) {
				for (int j = 0; j < coins; j++) {
					if (coinsLane[j] == currentLane - 1) {
						if (charaDimX[1] >= (coinsX[j] - 0.015) && charaDimX[0] <= (coinsX[j] + 0.015)) {
							double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.015;
							if (charaDimZ[0] <= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
								coinsX[j] = 0;
								score += 10;
								break;
							}
						}
					}
				}
			}
			//to catch p1
			if (!p1Used) {
				if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
					if (power1Lane == currentLane - 1) {
						if (charaDimX[1] >= (power1X - 0.01) && charaDimX[0] <= (power1X + 0.01)) {
							double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
							if (charaDimZ[0] <= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p1Used = true;
								score *= 2;
							}
						}
					}
				}
			}
			//to catch p2
			if (!p2Used) {
				if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
					if (power2Lane == currentLane - 1) {
						if (charaDimX[1] >= (power2X - 0.01) && charaDimX[0] <= (power2X + 0.01)) {
							double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
							if (charaDimZ[0] <= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p2Used = true;
								timeLeft += 20;
							}
						}
					}
				}
			}
			//to catch p3
			if (!p3Used) {
				if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
					if (power3Lane == currentLane - 1) {
						if (charaDimX[1] >= (power3X - 0.01) && charaDimX[0] <= (power3X + 0.01)) {
							double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
							if (charaDimZ[0] <= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p3Used = true;
								charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
								charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
								charaDimZ[0] = charaDimZ[1] - 0.03;
								currentLane--;
							}
						}
					}
				}
			}
		}

		else {
			//PlaySound(NULL, 0, 0);
			sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
		}

		break;
	case GLUT_KEY_DOWN:

		up = false;

		if (!(charaDimZ[1] + 0.01 >= 0.99)) {

			//in bridge to change currentLane
			if (inBridge) {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaZ += 0.01;
				charaDimZ[0] += 0.01;
				charaDimZ[1] += 0.01;

				if (firstCam)
					camera.moveZ(-0.01);

				if (charaDimZ[0] >= bordersZ[(currentLane + 1) * 2 - 1]) {
					currentLane++;
					inBridge = false;
				}
			}
			//not in bridge
			else if (charaDimZ[1] + 0.01 >= bordersZ[currentLane * 2 - 2]) {
				if ((level == 1 && currentLane != 5) || (level == 2 && currentLane != 6)) {
					if (charaDimX[0] >= bridgesX[currentLane - 1] - 0.075 && charaDimX[1] <= bridgesX[currentLane - 1] + 0.075) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
						charaZ += 0.01;
						charaDimZ[0] += 0.01;
						charaDimZ[1] += 0.01;
						inBridge = true;

						if (firstCam)
							camera.moveZ(-0.01);
					}

					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
					return;
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaZ += 0.01;
				charaDimZ[0] += 0.01;
				charaDimZ[1] += 0.01;

				if (firstCam)
					camera.moveZ(-0.01);
			}

			//catch goal
			if ((charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[1] >= 0.2 - 0.01 && charaDimX[1] <= 0.2) || (charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[0] >= 0.2 && charaDimX[0] <= 0.2 + 0.01) || (charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[0] < 0.2 && charaDimX[1] > 0.2)) {
				if (level == 1) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					level = 2;
					lanes = 6;
					currentLane = 6;
					p1Used = false;
					p2Used = false;

					//intialize character dimensions
					charaX = 0.0;
					charaZ = 0.0;
					charaDimX[0] = 0.665;
					charaDimX[1] = 0.735;
					charaDimZ[0] = 0.885;
					charaDimZ[1] = 0.915;

					//intialize coinsLane array with random values between 1 and max number of lanes
					for (int x = 1; x <= coins; x++) {
						int randX = (rand() % (6 - 1)) + 1;
						coinsLane[x - 1] = randX;
					}

					//intialize coinsX array with random values between 0.1 and 0.9
					for (int x = 1; x <= coins; x++) {
						double randCoinX;
						bool final = false;

						if (coinsLane[x - 1] == (6 - 1)) {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}

							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}
						else {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}
							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}

						coinsX[x - 1] = randCoinX;
					}
					//-----------------------------------------------------------------------------------------

					//intiallize bordersZ
					double lane = 1.0;
					for (int i = 0; i < lanes * 2; i += 2) {
						if (lane == 6.0) {
							bordersZ[i] = 0.99;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						else if (lane == 1.0) {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = 0.02;
						}
						else {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						lane++;
					}
					//-----------------------------------------------------------------
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					goalTaken = true;
					camera.reset();
				}
			}

			//to catch coins
			if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.015) {
				for (int j = 0; j < coins; j++) {
					if (coinsLane[j] == currentLane - 1) {
						if (charaDimX[1] >= (coinsX[j] - 0.015) && charaDimX[0] <= (coinsX[j] + 0.015)) {
							int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.015;
							if (charaDimZ[1] >= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
								coinsX[j] = 0;
								score += 10;
							}
						}
					}
				}
			}

			//to catch p1
			if (!p1Used) {
				if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
					if (power1Lane == currentLane - 1) {
						if (charaDimX[1] >= (power1X - 0.01) && charaDimX[0] <= (power1X + 0.01)) {
							int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
							if (charaDimZ[1] >= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p1Used = true;
								score *= 2;
							}
						}
					}
				}
			}
			//to catch p2
			if (!p2Used) {
				if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
					if (power2Lane == currentLane - 1) {
						if (charaDimX[1] >= (power2X - 0.01) && charaDimX[0] <= (power2X + 0.01)) {
							int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
							if (charaDimZ[1] >= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p2Used = true;
								timeLeft += 20;
							}
						}
					}
				}
			}
			//to catch p3
			if (!p3Used) {
				if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
					if (power3Lane == currentLane - 1) {
						if (charaDimX[1] >= (power3X - 0.01) && charaDimX[0] <= (power3X + 0.01)) {
							int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
							if (charaDimZ[1] >= posY) {
								//PlaySound(NULL, 0, 0);
								sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
								p3Used = true;
								charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
								charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
								charaDimZ[0] = charaDimZ[1] - 0.03;
								currentLane--;
							}
						}
					}
				}
			}
		}

		else {
			//PlaySound(NULL, 0, 0);
			sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
		}
		break;
	case GLUT_KEY_LEFT:

		if ((currentLane != 1 && !(charaDimX[0] - 0.01 <= 0.02)) || (currentLane == 1 && !(charaDimX[0] - 0.01 <= 0.115))) {

			//in bridge
			if (inBridge) {
				if (charaDimX[0] - 0.01 <= bridgesX[currentLane - 2] - 0.075) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
					return;
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\moVe.wav"), SND_ASYNC);
					charaX -= 0.01;
					charaDimX[0] -= 0.01;
					charaDimX[1] -= 0.01;

					if (firstCam)
						camera.moveX(0.01);
				}
			}
			//not in bridge
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaX -= 0.01;
				charaDimX[0] -= 0.01;
				charaDimX[1] -= 0.01;

				if (firstCam)
					camera.moveX(0.01);
			}

			//goal taken
			if ((charaDimX[0] <= (0.2 + 0.01) && charaDimZ[0] >= 0.1 && charaDimZ[0] <= (0.1 + 0.04)) || (charaDimX[0] <= (0.2 + 0.01) && charaDimZ[1] <= 0.1 && charaDimZ[1] >= (0.1 - 0.04)) || (charaDimX[0] <= (0.2 + 0.01) && charaDimZ[0] > (0.1) && charaDimZ[1] < (0.1))) {
				if (level == 1) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					level = 2;
					lanes = 6;
					currentLane = 6;
					p1Used = false;
					p2Used = false;

					//intialize character dimensions
					charaX = 0.0;
					charaZ = 0.0;
					charaDimX[0] = 0.665;
					charaDimX[1] = 0.735;
					charaDimZ[0] = 0.885;
					charaDimZ[1] = 0.915;

					//intialize coinsLane array with random values between 1 and max number of lanes
					for (int x = 1; x <= coins; x++) {
						int randX = (rand() % (6 - 1)) + 1;
						coinsLane[x - 1] = randX;
					}

					//intialize coinsX array with random values between 0.1 and 0.9
					for (int x = 1; x <= coins; x++) {
						double randCoinX;
						bool final = false;

						if (coinsLane[x - 1] == (6 - 1)) {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}

							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}
						else {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}
							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}

						coinsX[x - 1] = randCoinX;
					}
					//-----------------------------------------------------------------------------------------

					//intiallize bordersZ
					double lane = 1.0;
					for (int i = 0; i < lanes * 2; i += 2) {
						if (lane == 6.0) {
							bordersZ[i] = 0.99;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						else if (lane == 1.0) {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = 0.02;
						}
						else {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						lane++;
					}
					//-----------------------------------------------------------------
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					goalTaken = true;
					camera.reset();
				}
			}

			//to catch coins
			for (int j = 0; j < coins; j++) {
				if (coinsLane[j] == currentLane - 1) {
					if ((charaDimX[0]) <= (coinsX[j] + 0.015) && (charaDimX[1]) >= (coinsX[j] - 0.015)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
						coinsX[j] = 0.0;
						score += 10;
					}
				}
			}

			//to catch P1
			if (!p1Used) {
				if (power1Lane == currentLane - 1) {
					if ((charaDimX[0]) <= (power1X + 0.01) && (charaDimX[1]) >= (power1X - 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p1Used = true;
						score *= 2;
					}
				}
			}
			//to catch P2
			if (!p2Used) {
				if (power2Lane == currentLane - 1) {
					if ((charaDimX[0]) <= (power2X + 0.01) && (charaDimX[1]) >= (power2X - 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p2Used = true;
						timeLeft += 20;
					}
				}
			}
			//to catch P3
			if (!p3Used) {
				if (power3Lane == currentLane - 1) {
					if ((charaDimX[0]) <= (power3X + 0.01) && (charaDimX[1]) >= (power3X - 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p3Used = true;
						charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
						charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
						charaDimZ[0] = charaDimZ[1] - 0.03;
						currentLane--;
					}
				}
			}
		}
		else {
			//PlaySound(NULL, 0, 0);
			sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
		}
		break;
	case GLUT_KEY_RIGHT:

		if ((currentLane != 1 && !(charaDimX[1] + 0.01 >= 1.0)) || (currentLane == 1 && !(charaDimX[1] + 0.01 >= 0.885))) {

			//in bridge
			if (inBridge) {
				if (charaDimX[1] + 0.01 >= bridgesX[currentLane - 2] + 0.075) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
					return;
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaX += 0.01;
					charaDimX[0] += 0.01;
					charaDimX[1] += 0.01;

					if (firstCam)
						camera.moveX(-0.01);
				}
			}
			//not in bridge
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
				charaX += 0.01;
				charaDimX[0] += 0.01;
				charaDimX[1] += 0.01;

				if (firstCam)
					camera.moveX(-0.01);
			}

			//catch goal
			if ((charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[0] >= 0.1 && charaDimZ[0] <= 0.1 + 0.04) || (charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[1] <= 0.1 && charaDimZ[1] >= 0.1 - 0.04) || (charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[0] > 0.1 && charaDimZ[1] < 0.1)) {
				if (level == 1) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					level = 2;
					lanes = 6;
					currentLane = 6;
					p1Used = false;
					p2Used = false;

					//intialize character dimensions
					charaX = 0.0;
					charaZ = 0.0;
					charaDimX[0] = 0.665;
					charaDimX[1] = 0.735;
					charaDimZ[0] = 0.885;
					charaDimZ[1] = 0.915;

					//intialize coinsLane array with random values between 1 and max number of lanes
					for (int x = 1; x <= coins; x++) {
						int randX = (rand() % (6 - 1)) + 1;
						coinsLane[x - 1] = randX;
					}

					//intialize coinsX array with random values between 0.1 and 0.9
					for (int x = 1; x <= coins; x++) {
						double randCoinX;
						bool final = false;

						if (coinsLane[x - 1] == (6 - 1)) {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}

							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}
						else {

							//to prevent overlapping of coins
							double rejects[coins + 2];
							int rejectsSpace = 0;
							for (int i = 0; i < coins; i++) {
								if (x - 1 != i) {
									if (coinsLane[i] == coinsLane[x])
										rejects[rejectsSpace++] = coinsX[i];
								}
							}
							while (!final) {
								randCoinX = randf(0.1, 0.9);
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
										break;
								}
								for (int i = 0; i <= 15; i++) {
									if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
										break;
								}
								final = true;
							}
						}

						coinsX[x - 1] = randCoinX;
					}
					//-----------------------------------------------------------------------------------------

					//intiallize bordersZ
					double lane = 1.0;
					for (int i = 0; i < lanes * 2; i += 2) {
						if (lane == 6.0) {
							bordersZ[i] = 0.99;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						else if (lane == 1.0) {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = 0.02;
						}
						else {
							bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
							bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
						}
						lane++;
					}
					//-----------------------------------------------------------------
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
					goalTaken = true;
					camera.reset();
				}
			}

			//to catch coins
			for (int j = 0; j < coins; j++) {
				if (coinsLane[j] == currentLane - 1) {
					if ((charaDimX[1]) >= (coinsX[j] - 0.015) && (charaDimX[0]) <= (coinsX[j] + 0.015)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
						coinsX[j] = 0;
						score += 10;
					}
				}
			}

			//to catch P1
			if (!p1Used) {
				if (power1Lane == currentLane - 1) {
					if ((charaDimX[1]) >= (power1X - 0.01) && (charaDimX[0]) <= (power1X + 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p1Used = true;
						score *= 2;
					}
				}
			}
			//to catch P2
			if (!p2Used) {
				if (power2Lane == currentLane - 1) {
					if ((charaDimX[1]) >= (power2X - 0.01) && (charaDimX[0]) <= (power2X + 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p2Used = true;
						timeLeft += 20;
					}
				}
			}
			//to catch P3
			if (!p3Used) {
				if (power3Lane == currentLane - 1) {
					if ((charaDimX[1]) >= (power3X - 0.01) && (charaDimX[0]) <= (power3X + 0.01)) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
						p3Used = true;
						charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
						charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
						charaDimZ[0] = charaDimZ[1] - 0.03;
						currentLane--;
					}
				}
			}
		}
		else {
			//PlaySound(NULL, 0, 0);
			sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
		}
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	setupCamera();

	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		int lanes;
		if (level == 1)
			lanes = 5;
		else
			lanes = 6;

		if (y < 240 && x>240 && x < 400) {

			up = true;

			if (!(charaDimZ[0] - 0.01 <= 0.02)) {

				//in bridge
				if (inBridge) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaZ -= 0.01;
					charaDimZ[0] -= 0.01;
					charaDimZ[1] -= 0.01;

					if (firstCam)
						camera.moveZ(0.01);

					if (charaDimZ[1] <= bordersZ[(currentLane - 1) * 2 - 2]) {
						currentLane--;
						inBridge = false;
					}
				}
				//not in bridge
				else if (charaDimZ[0] - 0.01 <= bordersZ[currentLane * 2 - 1]) {
					if (currentLane != 1) {
						if (charaDimX[0] >= bridgesX[currentLane - 2] - 0.075 && charaDimX[1] <= bridgesX[currentLane - 2] + 0.075) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
							charaZ -= 0.01;
							charaDimZ[0] -= 0.01;
							charaDimZ[1] -= 0.01;
							inBridge = true;

							if (firstCam)
								camera.moveZ(0.01);
						}

						else {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
							return;
						}
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaZ -= 0.01;
					charaDimZ[0] -= 0.01;
					charaDimZ[1] -= 0.01;

					if (firstCam)
						camera.moveZ(0.01);
				}

				//check if goal taken
				if ((charaDimZ[0] <= 0.14 && charaDimX[1] >= 0.19 && charaDimX[1] <= 0.2) || (charaDimZ[0] <= 0.14 && charaDimX[0] >= 0.2 && charaDimX[0] <= 0.21) || (charaDimZ[0] <= 0.14 && charaDimX[0] < 0.2 && charaDimX[1] > 0.2)) {
					if (level == 1) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						level = 2;
						lanes = 6;
						currentLane = 6;
						p1Used = false;
						p2Used = false;
						p3Used = false;

						//intialize character dimensions
						charaX = 0.0;
						charaZ = 0.0;
						charaDimX[0] = 0.665;
						charaDimX[1] = 0.735;
						charaDimZ[0] = 0.885;
						charaDimZ[1] = 0.915;

						//intialize coinsLane array with random values between 1 and max number of lanes
						for (int x = 1; x <= coins; x++) {
							int randX = (rand() % (6 - 1)) + 1;
							coinsLane[x - 1] = randX;
						}

						//intialize coinsX array with random values between 0.1 and 0.9
						for (int x = 1; x <= coins; x++) {
							double randCoinX;
							bool final = false;

							if (coinsLane[x - 1] == (6 - 1)) {

								//to prevent overlapping of coins
								double rejects[coins + 3];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}

								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}
							else {

								//to prevent overlapping of coins
								double rejects[coins + 3];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}
								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}

							coinsX[x - 1] = randCoinX;
						}
						//-----------------------------------------------------------------------------------------

						//intiallize bordersZ
						double lane = 1.0;
						for (int i = 0; i < lanes * 2; i += 2) {
							if (lane == 6.0) {
								bordersZ[i] = 0.99;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							else if (lane == 1.0) {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = 0.02;
							}
							else {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							lane++;
						}
						//-----------------------------------------------------------------
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						goalTaken = true;
						camera.reset();
					}
				}

				//to catch coins
				if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.015) {
					for (int j = 0; j < coins; j++) {
						if (coinsLane[j] == currentLane - 1) {
							if (charaDimX[1] >= (coinsX[j] - 0.015) && charaDimX[0] <= (coinsX[j] + 0.015)) {
								double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.015;
								if (charaDimZ[0] <= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
									coinsX[j] = 0;
									score += 10;
									break;
								}
							}
						}
					}
				}
				//to catch p1
				if (!p1Used) {
					if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
						if (power1Lane == currentLane - 1) {
							if (charaDimX[1] >= (power1X - 0.01) && charaDimX[0] <= (power1X + 0.01)) {
								double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
								if (charaDimZ[0] <= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p1Used = true;
									score *= 2;
								}
							}
						}
					}
				}
				//to catch p2
				if (!p2Used) {
					if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
						if (power2Lane == currentLane - 1) {
							if (charaDimX[1] >= (power2X - 0.01) && charaDimX[0] <= (power2X + 0.01)) {
								double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
								if (charaDimZ[0] <= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p2Used = true;
									timeLeft += 20;
								}
							}
						}
					}
				}
				//to catch p3
				if (!p3Used) {
					if (charaDimZ[0] <= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01) {
						if (power3Lane == currentLane - 1) {
							if (charaDimX[1] >= (power3X - 0.01) && charaDimX[0] <= (power3X + 0.01)) {
								double posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2.0) + 0.01;
								if (charaDimZ[0] <= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p3Used = true;
									charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
									charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
									charaDimZ[0] = charaDimZ[1] - 0.03;
									currentLane--;
								}
							}
						}
					}
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
			}
		}

		else if (y > 240 && x > 240 && x < 400) {

			up = false;

			if (!(charaDimZ[1] + 0.01 >= 0.99)) {

				//in bridge to change currentLane
				if (inBridge) {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaZ += 0.01;
					charaDimZ[0] += 0.01;
					charaDimZ[1] += 0.01;

					if (firstCam)
						camera.moveZ(-0.01);

					if (charaDimZ[0] >= bordersZ[(currentLane + 1) * 2 - 1]) {
						currentLane++;
						inBridge = false;
					}
				}
				//not in bridge
				else if (charaDimZ[1] + 0.01 >= bordersZ[currentLane * 2 - 2]) {
					if ((level == 1 && currentLane != 5) || (level == 2 && currentLane != 6)) {
						if (charaDimX[0] >= bridgesX[currentLane - 1] - 0.075 && charaDimX[1] <= bridgesX[currentLane - 1] + 0.075) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
							charaZ += 0.01;
							charaDimZ[0] += 0.01;
							charaDimZ[1] += 0.01;
							inBridge = true;

							if (firstCam)
								camera.moveZ(-0.01);
						}
						else {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
							return;
						}
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
				}
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaZ += 0.01;
					charaDimZ[0] += 0.01;
					charaDimZ[1] += 0.01;

					if (firstCam)
						camera.moveZ(-0.01);
				}

				//catch goal
				if ((charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[1] >= 0.2 - 0.01 && charaDimX[1] <= 0.2) || (charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[0] >= 0.2 && charaDimX[0] <= 0.2 + 0.01) || (charaDimZ[1] >= 0.1 - 0.04 && charaDimZ[1] <= 0.1 + 0.04 && charaDimX[0] < 0.2 && charaDimX[1] > 0.2)) {
					if (level == 1) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						level = 2;
						lanes = 6;
						currentLane = 6;
						p1Used = false;
						p2Used = false;

						//intialize character dimensions
						charaX = 0.0;
						charaZ = 0.0;
						charaDimX[0] = 0.665;
						charaDimX[1] = 0.735;
						charaDimZ[0] = 0.885;
						charaDimZ[1] = 0.915;

						//intialize coinsLane array with random values between 1 and max number of lanes
						for (int x = 1; x <= coins; x++) {
							int randX = (rand() % (6 - 1)) + 1;
							coinsLane[x - 1] = randX;
						}

						//intialize coinsX array with random values between 0.1 and 0.9
						for (int x = 1; x <= coins; x++) {
							double randCoinX;
							bool final = false;

							if (coinsLane[x - 1] == (6 - 1)) {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}

								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}
							else {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}
								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}

							coinsX[x - 1] = randCoinX;
						}
						//-----------------------------------------------------------------------------------------

						//intiallize bordersZ
						double lane = 1.0;
						for (int i = 0; i < lanes * 2; i += 2) {
							if (lane == 6.0) {
								bordersZ[i] = 0.99;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							else if (lane == 1.0) {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = 0.02;
							}
							else {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							lane++;
						}
						//-----------------------------------------------------------------
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						goalTaken = true;
						camera.reset();
					}
				}

				//to catch coins
				if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.015) {
					for (int j = 0; j < coins; j++) {
						if (coinsLane[j] == currentLane - 1) {
							if (charaDimX[1] >= (coinsX[j] - 0.015) && charaDimX[0] <= (coinsX[j] + 0.015)) {
								int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.015;
								if (charaDimZ[1] >= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
									coinsX[j] = 0;
									score += 10;
								}
							}
						}
					}
				}

				//to catch p1
				if (!p1Used) {
					if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
						if (power1Lane == currentLane - 1) {
							if (charaDimX[1] >= (power1X - 0.01) && charaDimX[0] <= (power1X + 0.01)) {
								int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
								if (charaDimZ[1] >= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p1Used = true;
									score *= 2;
								}
							}
						}
					}
				}
				//to catch p2
				if (!p2Used) {
					if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
						if (power2Lane == currentLane - 1) {
							if (charaDimX[1] >= (power2X - 0.01) && charaDimX[0] <= (power2X + 0.01)) {
								int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
								if (charaDimZ[1] >= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p2Used = true;
									timeLeft += 20;
								}
							}
						}
					}
				}
				//to catch p3
				if (!p3Used) {
					if (charaDimZ[1] >= (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01) {
						if (power3Lane == currentLane - 1) {
							if (charaDimX[1] >= (power3X - 0.01) && charaDimX[0] <= (power3X + 0.01)) {
								int posY = (0.97 / lanes) * currentLane + 0.015 + (0.97 / lanes / 2) - 0.01;
								if (charaDimZ[1] >= posY) {
									//PlaySound(NULL, 0, 0);
									sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
									p3Used = true;
									charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
									charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
									charaDimZ[0] = charaDimZ[1] - 0.03;
									currentLane--;
								}
							}
						}
					}
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
			}
		}

		else if (x < 340 && y>180 && y < 300) {

			if ((currentLane != 1 && !(charaDimX[0] - 0.01 <= 0.02)) || (currentLane == 1 && !(charaDimX[0] - 0.01 <= 0.115))) {

				//in bridge
				if (inBridge) {
					if (charaDimX[0] - 0.01 <= bridgesX[currentLane - 2] - 0.075) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
						charaX -= 0.01;
						charaDimX[0] -= 0.01;
						charaDimX[1] -= 0.01;

						if (firstCam)
							camera.moveX(0.01);
					}
				}
				//not in bridge
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaX -= 0.01;
					charaDimX[0] -= 0.01;
					charaDimX[1] -= 0.01;

					if (firstCam)
						camera.moveX(0.01);
				}

				//goal taken
				if ((charaDimX[0] <= (0.2 + 0.01) && charaDimZ[0] >= 0.1 && charaDimZ[0] <= (0.1 + 0.04)) || (charaDimX[0] <= (0.2 + 0.01) && charaDimZ[1] <= 0.1 && charaDimZ[1] >= (0.1 - 0.04)) || (charaDimX[0] <= (0.2 + 0.01) && charaDimZ[0] > (0.1) && charaDimZ[1] < (0.1))) {
					if (level == 1) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						level = 2;
						lanes = 6;
						currentLane = 6;
						p1Used = false;
						p2Used = false;

						//intialize character dimensions
						charaX = 0.0;
						charaZ = 0.0;
						charaDimX[0] = 0.665;
						charaDimX[1] = 0.735;
						charaDimZ[0] = 0.885;
						charaDimZ[1] = 0.915;

						//intialize coinsLane array with random values between 1 and max number of lanes
						for (int x = 1; x <= coins; x++) {
							int randX = (rand() % (6 - 1)) + 1;
							coinsLane[x - 1] = randX;
						}

						//intialize coinsX array with random values between 0.1 and 0.9
						for (int x = 1; x <= coins; x++) {
							double randCoinX;
							bool final = false;

							if (coinsLane[x - 1] == (6 - 1)) {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}

								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}
							else {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}
								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}

							coinsX[x - 1] = randCoinX;
						}
						//-----------------------------------------------------------------------------------------

						//intiallize bordersZ
						double lane = 1.0;
						for (int i = 0; i < lanes * 2; i += 2) {
							if (lane == 6.0) {
								bordersZ[i] = 0.99;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							else if (lane == 1.0) {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = 0.02;
							}
							else {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							lane++;
						}
						//-----------------------------------------------------------------
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						goalTaken = true;
						camera.reset();
					}
				}

				//to catch coins
				for (int j = 0; j < coins; j++) {
					if (coinsLane[j] == currentLane - 1) {
						if ((charaDimX[0]) <= (coinsX[j] + 0.015) && (charaDimX[1]) >= (coinsX[j] - 0.015)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
							coinsX[j] = 0.0;
							score += 10;
						}
					}
				}

				//to catch P1
				if (!p1Used) {
					if (power1Lane == currentLane - 1) {
						if ((charaDimX[0]) <= (power1X + 0.01) && (charaDimX[1]) >= (power1X - 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p1Used = true;
							score *= 2;
						}
					}
				}
				//to catch P2
				if (!p2Used) {
					if (power2Lane == currentLane - 1) {
						if ((charaDimX[0]) <= (power2X + 0.01) && (charaDimX[1]) >= (power2X - 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p2Used = true;
							timeLeft += 20;
						}
					}
				}
				//to catch P3
				if (!p3Used) {
					if (power3Lane == currentLane - 1) {
						if ((charaDimX[0]) <= (power3X + 0.01) && (charaDimX[1]) >= (power3X - 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p3Used = true;
							charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
							charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
							charaDimZ[0] = charaDimZ[1] - 0.03;
							currentLane--;
						}
					}
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
			}
		}

		else if (x > 340 && y > 180 && y < 300) {

			if ((currentLane != 1 && !(charaDimX[1] + 0.01 >= 1.0)) || (currentLane == 1 && !(charaDimX[1] + 0.01 >= 0.885))) {

				//in bridge
				if (inBridge) {
					if (charaDimX[1] + 0.01 >= bridgesX[currentLane - 2] + 0.075) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
						return;
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
						charaX += 0.01;
						charaDimX[0] += 0.01;
						charaDimX[1] += 0.01;

						if (firstCam)
							camera.moveX(-0.01);
					}
				}
				//not in bridge
				else {
					//PlaySound(NULL, 0, 0);
					sndPlaySound(TEXT("sounds\\move.wav"), SND_ASYNC);
					charaX += 0.01;
					charaDimX[0] += 0.01;
					charaDimX[1] += 0.01;

					if (firstCam)
						camera.moveX(-0.01);
				}

				//catch goal
				if ((charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[0] >= 0.1 && charaDimZ[0] <= 0.1 + 0.04) || (charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[1] <= 0.1 && charaDimZ[1] >= 0.1 - 0.04) || (charaDimX[1] >= 0.2 - 0.01 && charaDimX[0] <= 0.2 + 0.01 && charaDimZ[0] > 0.1 && charaDimZ[1] < 0.1)) {
					if (level == 1) {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						level = 2;
						lanes = 6;
						currentLane = 6;
						p1Used = false;
						p2Used = false;

						//intialize character dimensions
						charaX = 0.0;
						charaZ = 0.0;
						charaDimX[0] = 0.665;
						charaDimX[1] = 0.735;
						charaDimZ[0] = 0.885;
						charaDimZ[1] = 0.915;

						//intialize coinsLane array with random values between 1 and max number of lanes
						for (int x = 1; x <= coins; x++) {
							int randX = (rand() % (6 - 1)) + 1;
							coinsLane[x - 1] = randX;
						}

						//intialize coinsX array with random values between 0.1 and 0.9
						for (int x = 1; x <= coins; x++) {
							double randCoinX;
							bool final = false;

							if (coinsLane[x - 1] == (6 - 1)) {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}

								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}
							else {

								//to prevent overlapping of coins
								double rejects[coins + 2];
								int rejectsSpace = 0;
								for (int i = 0; i < coins; i++) {
									if (x - 1 != i) {
										if (coinsLane[i] == coinsLane[x])
											rejects[rejectsSpace++] = coinsX[i];
									}
								}
								while (!final) {
									randCoinX = randf(0.1, 0.9);
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
											break;
									}
									for (int i = 0; i <= 15; i++) {
										if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
											break;
									}
									final = true;
								}
							}

							coinsX[x - 1] = randCoinX;
						}
						//-----------------------------------------------------------------------------------------

						//intiallize bordersZ
						double lane = 1.0;
						for (int i = 0; i < lanes * 2; i += 2) {
							if (lane == 6.0) {
								bordersZ[i] = 0.99;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							else if (lane == 1.0) {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = 0.02;
							}
							else {
								bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
								bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
							}
							lane++;
						}
						//-----------------------------------------------------------------
					}
					else {
						//PlaySound(NULL, 0, 0);
						sndPlaySound(TEXT("sounds\\goal.wav"), SND_ASYNC);
						goalTaken = true;
						camera.reset();
					}
				}

				//to catch coins
				for (int j = 0; j < coins; j++) {
					if (coinsLane[j] == currentLane - 1) {
						if ((charaDimX[1]) >= (coinsX[j] - 0.015) && (charaDimX[0]) <= (coinsX[j] + 0.015)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\coin.wav"), SND_ASYNC);
							coinsX[j] = 0;
							score += 10;
						}
					}
				}

				//to catch P1
				if (!p1Used) {
					if (power1Lane == currentLane - 1) {
						if ((charaDimX[1]) >= (power1X - 0.01) && (charaDimX[0]) <= (power1X + 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p1Used = true;
							score *= 2;
						}
					}
				}
				//to catch P2
				if (!p2Used) {
					if (power2Lane == currentLane - 1) {
						if ((charaDimX[1]) >= (power2X - 0.01) && (charaDimX[0]) <= (power2X + 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p2Used = true;
							timeLeft += 20;
						}
					}
				}
				//to catch P3
				if (!p3Used) {
					if (power3Lane == currentLane - 1) {
						if ((charaDimX[1]) >= (power3X - 0.01) && (charaDimX[0]) <= (power3X + 0.01)) {
							//PlaySound(NULL, 0, 0);
							sndPlaySound(TEXT("sounds\\powerup.wav"), SND_ASYNC);
							p3Used = true;
							charaZ -= (charaDimZ[1] - bordersZ[(currentLane - 1) * 2 - 2]);
							charaDimZ[1] = bordersZ[(currentLane - 1) * 2 - 2];
							charaDimZ[0] = charaDimZ[1] - 0.03;
							currentLane--;
						}
					}
				}
			}
			else {
				//PlaySound(NULL, 0, 0);
				sndPlaySound(TEXT("sounds\\solid.wav"), SND_ASYNC);
			}
		}
	}

	setupCamera();


	glutPostRedisplay();//redisplay to update the screen with the new paraeters
}

//animation for each object (start, stop) rotate, scale, translate back and forth
void Anim() {

	coinRotation++;
	for (int i = 0; i < 60000; i++);

	glutPostRedisplay();
}

void setupLights() {

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lightspecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	GLfloat lightIntensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void main(int argc, char** argv) {

	srand((unsigned)time(0));
	float test = randf(0.1, 0.9);

	glutInit(&argc, argv);

	int lanes;
	if (level == 1)
		lanes = 5;
	else
		lanes = 6;

	//intialize character dimensions
	charaDimX[0] = 0.665;
	charaDimX[1] = 0.735;
	charaDimZ[0] = 0.885;
	charaDimZ[1] = 0.915;

	//intialize coinsLane array with random values between 1 and max number of lanes
	for (int x = 1; x <= coins; x++) {
		int randX = (rand() % (lanes - 1)) + 1;
		coinsLane[x - 1] = randX;
	}

	//intialize coinsX array with random values between 0.1 and 0.9
	for (int x = 1; x <= coins; x++) {
		double randCoinX;
		bool final = false;

		if (coinsLane[x - 1] == (lanes - 1)) {

			//to prevent overlapping of coins
			double rejects[coins + 2];
			int rejectsSpace = 0;
			for (int i = 0; i < coins; i++) {
				if (x - 1 != i) {
					if (coinsLane[i] == coinsLane[x])
						rejects[rejectsSpace++] = coinsX[i];
				}
			}

			while (!final) {
				randCoinX = randf(0.1, 0.9);
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
						break;
				}
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
						break;
				}
				final = true;
			}
		}
		else {

			//to prevent overlapping of coins
			double rejects[coins + 2];
			int rejectsSpace = 0;
			for (int i = 0; i < coins; i++) {
				if (x - 1 != i) {
					if (coinsLane[i] == coinsLane[x])
						rejects[rejectsSpace++] = coinsX[i];
				}
			}
			while (!final) {
				randCoinX = randf(0.1, 0.9);
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX - i)) != end(rejects))
						break;
				}
				for (int i = 0; i <= 15; i++) {
					if ((find(begin(rejects), end(rejects), randCoinX + i)) != end(rejects))
						break;
				}
				final = true;
			}
		}

		coinsX[x - 1] = randCoinX;
	}
	//-----------------------------------------------------------------------------------------

	//intiallize bordersZ
	double lane = 1.0;
	for (int i = 0; i < lanes * 2; i += 2) {
		if (lane == 5.0) {
			bordersZ[i] = 0.99;
			bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
		}
		else if (lane == 1.0) {
			bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
			bordersZ[i + 1] = 0.02;
		}
		else {
			bordersZ[i] = (0.97 / lanes) * lane + 0.02 - 0.015;
			bordersZ[i + 1] = (0.97 / lanes) * (lane - 1.0) + 0.02 + 0.015;
		}
		lane++;
	}

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300, 50);

	glutCreateWindow("3D Maze");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutMouseFunc(mouseFunc);

	myInit();
	load();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);

	glutIdleFunc(Anim);
	glutTimerFunc(0, Timer, 0);
	glutTimerFunc(0, TimeNow, 0);

	glutMainLoop();
}

void myInit () {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	setupCamera();
	setupLights();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	if (goalTaken) {
		//display you won + score
		char* p0s1[20];
		sprintf((char*)p0s1, "YOU WON!! Score: %d", score);
		displayText(0.0, 0.0, 1.0, 1.0, 1.0, (char*)p0s1);
	}

	else if (timeLeft <= 0 && !goalTaken) {
		//display you lost + score
		char* p0s1[20];
		sprintf((char*)p0s1, "Times up, you lost :( Score: %d", score);
		displayText(0.0, 0.0, 1.0, 1.0, 1.0, (char*)p0s1);
	}

	//garden level
	else if (level == 1) {
		//ground
		drawGround();
		//grass
		drawGrass();
		//fence
		glPushMatrix();
		glTranslated(0.01, 0.0, 0.0);
		glRotated(90, 0, 0, 1.0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(1.01, 0.0, 0.0);
		glRotated(90, 0, 0, 1.0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, 0.0, 0.01);
		glRotated(90, 0, 0, 1.0);
		glRotated(90, 1.0, 0, 0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, 0.0, 1.0);
		glRotated(90, 0, 0, 1.0);
		glRotated(90, 1.0, 0, 0);
		drawFence();
		glPopMatrix();
		//trees
		drawTrees();
		//lanes
		drawLanes();
		//bridges
		drawBridges();
		//coins
		drawCoins();
		//power ups
		drawPowerUps();
		//character
		glPushMatrix();
		glTranslatef(charaX, 0, charaZ);
		drawCharacter();
		glPopMatrix();
		//goal
		drawGoal();
		//background
		glPushMatrix();
		glColor3f(1, 1, 1);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 50, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();
	}

	//dungeon level
	else if (level == 2) {
		//ground
		drawGround();
		//rocks
		drawRocks();
		//fence
		glPushMatrix();
		glTranslated(0.01, 0.0, 0.0);
		glRotated(90, 0, 0, 1.0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(1.01, 0.0, 0.0);
		glRotated(90, 0, 0, 1.0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, 0.0, 0.01);
		glRotated(90, 0, 0, 1.0);
		glRotated(90, 1.0, 0, 0);
		drawFence();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, 0.0, 1.0);
		glRotated(90, 0, 0, 1.0);
		glRotated(90, 1.0, 0, 0);
		drawFence();
		glPopMatrix();
		//lanes
		drawLanes();
		//bridges
		drawBridges();
		//coins
		drawCoins();
		//power ups
		drawPowerUps();
		//character
		glPushMatrix();
		glTranslatef(charaX, 0, charaZ);
		drawCharacter();
		glPopMatrix();
		//goal
		drawGoal();
		//background
		glPushMatrix();
		glColor3f(1, 1, 1);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 50, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, texMetal);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();
	}

	glutSwapBuffers();

	glFlush();
}

void load() {
	model_tree.Load("Models/tree/Tree1.3ds");
	tex_ground.Load("textures\\ground.bmp");
	loadBMP(&tex, "textures\\blu-sky-3.bmp", true);
	loadBMP(&texMetal, "textures/metal.bmp", true);
	loadBMP(&texRocks, "textures/rocks.bmp", true);
}

//6 primitives
void drawFence() {

	if (level == 1) {

		glColor3f(1.0, 1.0, 1.0);

		//horizontal
		glPushMatrix();
		glTranslated(0.05, 0.5 * 0.02, 0.5);
		glScaled(0.02, 0.02, 1.0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.5 * 0.02, 0.5);
		glScaled(0.02, 0.02, 1.0);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.15, 0.5 * 0.02, 0.5);
		glScaled(0.02, 0.02, 1.0);
		glutSolidCube(1);
		glPopMatrix();

		//vertical
		glPushMatrix();
		glTranslated(0.1, 0.01, 0.0);
		glScaled(0.2, 0.02, 0.02);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.01, 0.25);
		glScaled(0.2, 0.02, 0.02);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.01, 0.5);
		glScaled(0.2, 0.02, 0.02);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.01, 0.75);
		glScaled(0.2, 0.02, 0.02);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.01, 1);
		glScaled(0.2, 0.02, 0.02);
		glutSolidCube(1);
		glPopMatrix();

	}
	else {
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);

		glColor3f(0.0, 0.0, 0.0);

		//horizontal
		glPushMatrix();
		glTranslated(0.05, 0.5 * 0.02, 0.0);
		gluCylinder(qobj, 0.01, 0.01, 1.0, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.5 * 0.02, 0.0);
		gluCylinder(qobj, 0.01, 0.01, 1.0, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.15, 0.5 * 0.02, 0.0);
		gluCylinder(qobj, 0.01, 0.01, 1.0, 8, 8);
		glPopMatrix();

		//vertical
		glPushMatrix();
		glTranslated(0.0, 0.01, 0.0);
		glRotated(90, 0, 1, 0);
		gluCylinder(qobj, 0.01, 0.01, 0.2, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.0, 0.01, 0.25);
		glRotated(90, 0, 1, 0);
		gluCylinder(qobj, 0.01, 0.01, 0.2, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.0, 0.01, 0.5);
		glRotated(90, 0, 1, 0);
		gluCylinder(qobj, 0.01, 0.01, 0.2, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.0, 0.01, 0.75);
		glRotated(90, 0, 1, 0);
		gluCylinder(qobj, 0.01, 0.01, 0.2, 8, 8);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.0, 0.01, 1);
		glRotated(90, 0, 1, 0);
		gluCylinder(qobj, 0.01, 0.01, 0.2, 8, 8);
		glPopMatrix();
	}
}

//1 primitive
void drawGround() {
	glPushMatrix();
	glTranslated(0.5, 0.0, 0.5);
	glScaled(1.0, 0.02, 1.0);
	glColor3f(0.3, 0.0, 0.0);
	glutSolidCube(1);
	glPopMatrix();
}


void drawGrass() {
	glDisable(GL_LIGHTING);
	glColor3f(0.6, 0.6, 0.6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(0.0, 0.04, 0.0);
	glTexCoord2f(5, 0);
	glVertex3f(1, 0.04, 0.0);
	glTexCoord2f(5, 5);
	glVertex3f(1, 0.04, 1);
	glTexCoord2f(0, 5);
	glVertex3f(0, 0.04, 1);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glColor3f(1, 1, 1);
}


void drawRocks() {
	glDisable(GL_LIGHTING);
	glColor3f(0.6, 0.6, 0.6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texRocks);
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(0.0, 0.04, 0.0);
	glTexCoord2f(5, 0);
	glVertex3f(1, 0.04, 0.0);
	glTexCoord2f(5, 5);
	glVertex3f(1, 0.04, 1);
	glTexCoord2f(0, 5);
	glVertex3f(0, 0.04, 1);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glColor3f(1, 1, 1);
}


void drawLanes() {

	int lanes;

	if (level == 1) {
		lanes = 5;
		glColor3f(1.0, 1.0, 1.0);

		for (int x = 1; x < lanes; x++) {
			double pos = (0.97 / lanes) * x + 0.02;
			glPushMatrix();
			glTranslated(0.5, 0.04, pos);
			glScaled(0.97, 0.1, 0.03);
			glutSolidCube(1.0);
			glPopMatrix();
		}
	}
	else {
		lanes = 6;
		glColor3f(0.0, 0.0, 0.0);

		for (int x = 1; x < lanes; x++) {
			double pos = (0.97 / lanes) * x + 0.02;
			glPushMatrix();
			glTranslated(0.5, 0.04, pos);
			glScaled(0.97, 0.1, 0.03);
			glutSolidCube(1.0);
			glPopMatrix();
		}
	}
}

void drawBridges() {

	double lanes;

	if (level == 1) {
		lanes = 5.0;
		glColor3f(0.0, 0.0, 0.0);

		for (int x = 1; x < lanes; x++) {
			double pos = (0.97 / lanes) * x + 0.02;
			glPushMatrix();
			glTranslated(bridgesX[x - 1], 0.04, pos);
			glScaled(0.15, 0.11, 0.04);
			glutSolidCube(1.0);
			glPopMatrix();
		}
	}
	else {
		lanes = 6.0;
		glColor3f(1.0, 1.0, 1.0);

		for (int x = 1; x < lanes; x++) {
			double pos = (0.97 / lanes) * x + 0.02;
			glPushMatrix();
			glTranslated(bridgesX[x - 1], 0.04, pos);
			glScaled(0.15, 0.11, 0.04);
			glutSolidCube(1.0);
			glPopMatrix();
		}
	}
}

void drawTrees() {
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);

	glPushMatrix();
	glTranslated(0.1, 0.04, 0.1);
	glScaled(0.05, 0.03, 0.05);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.9, 0.04, 0.1);
	glScaled(0.05, 0.03, 0.05);
	model_tree.Draw();
	glPopMatrix();
}

//4 primitives (head and body)
void drawCharacter() {
	//head
	glPushMatrix();
	glTranslated(0.7, 0.2, 0.9);
	glColor3f(0.9, 0.8, 0.6);
	glutSolidSphere(0.02, 10, 10);
	glPopMatrix();

	//body
	glPushMatrix();
	glTranslated(0.7, 0.14, 0.9);
	glScaled(0.07, 0.1, 0.03);
	glColor3f(0.1, 0, 0.4);
	glutSolidCube(1.0);
	glPopMatrix();

	//legs
	glColor3f(0.0, 0.0, 0.0);

	glPushMatrix();
	glTranslated(0.685, 0.045, 0.9);
	glScaled(0.01, 0.09, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.715, 0.045, 0.9);
	glScaled(0.01, 0.09, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawCoins() {

	double lanes;
	if (level == 1)
		lanes = 5.0;
	else
		lanes = 6.0;

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);

	glColor3f(0.9, 0.9, 0.2);

	for (int x = 1; x <= coins; x++) {
		if (coinsX[x - 1] != 0) {
			double pos = (0.97 / lanes) * coinsLane[x - 1] + 0.015 + (0.97 / lanes / 2.0);
			glPushMatrix();
			glTranslated(coinsX[x - 1], 0.07, pos);
			glRotatef(coinRotation, 0, 1, 0);
			glTranslated(0, 0, 0);
			gluCylinder(qobj, 0.03, 0.03, 0.01, 8.0, 8.0);
			glPopMatrix();
		}
	}
}

void drawPowerUps() {

	double lanes;
	if (level == 1)
		lanes = 5.0;
	else
		lanes = 6.0;

	//doublescore
	if (!p1Used) {
		glColor3f(0.7, 0.4, 0.7);
		glPushMatrix();
		double pos = (0.97 / lanes) * power1Lane + 0.015 + (0.97 / lanes / 2.0);
		glTranslated(power1X, 0.07, pos);
		glutSolidSphere(0.02, 10, 10);
		glPopMatrix();
	}
	//timeboost
	if (!p2Used) {
		glColor3f(0.6, 0.0, 0.3);
		glPushMatrix();
		double pos = (0.97 / lanes) * power2Lane + 0.015 + (0.97 / lanes / 2.0);
		glTranslated(power2X, 0.07, pos);
		glutSolidSphere(0.02, 10, 10);
		glPopMatrix();
	}
	//skiplane
	if (!p3Used) {
		glColor3f(0.4, 0.0, 0.0);
		glPushMatrix();
		double pos = (0.97 / lanes) * power3Lane + 0.015 + (0.97 / lanes / 2.0);
		glTranslated(power3X, 0.07, pos);
		glutSolidSphere(0.02, 10, 10);
		glPopMatrix();
	}
}

//1 primitive
void drawGoal() {

	glPushMatrix();
	glTranslated(0.2, 0.04, 0.1);
	glutSolidCone(0.02, 0.04, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.2, 0.04, 0.1);
	glRotated(180, 1, 0, 0);
	glutSolidCone(0.02, 0.04, 8, 8);
	glPopMatrix();

}