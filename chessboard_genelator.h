#pragma comment(lib, "glut32.lib")
#define WIDTH 640
#define HEIGHT 480

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>
#include <GL/glut.h>

using namespace cv;

class Camera
{
public:
	Mat rmat, tmat;
	Mat eye, up, center, initial_eye, initial_up, initial_center;
	Camera(double x, double y, double z);
	void roll(double angle);
	void pitch(double angle);
	void yaw(double angle);
	void translate(double x, double y, double z);
	void update();
	void resetRmat();
};

void display(void);
void display2(void);
void line3D(GLfloat x1, GLfloat y1,
	GLfloat z1, GLfloat x2,
	GLfloat y2, GLfloat z2);
void init();
void timer(int value);
void specialKeyboard(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void postProcess(void);