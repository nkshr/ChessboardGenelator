#include "chessboard_genelator.h"
#include "GLCapture.hpp"
#include <MatCom.hpp>

using namespace std;

Camera cam(0 , 0, 0);
MatCom server("server");

void setGluLookAt(GLfloat *eye, GLfloat *center, GLfloat *up)
{
	eye[0] = cam.eye.at<double>(0, 0);
	eye[1] = cam.eye.at<double>(1, 0);
	eye[2] = cam.eye.at<double>(2, 0);
	center[0] = cam.center.at<double>(0, 0);
	center[1] = cam.center.at<double>(1, 0);
	center[2] = cam.center.at<double>(2, 0);
	up[0] = cam.up.at<double>(0, 0);
	up[1] = cam.up.at<double>(1, 0);
	up[2] = cam.up.at<double>(2, 0);
}

void draw(void)
{
	const float side = 30;
	//glEnable(GL_CULL_FACE);
	/*glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(side * 7 - 1, 0, 0);
	glVertex3f(side * 7 - 1, side * 7 - 1, 0);
	glVertex3f(0, side * 7 - 1, 0);
	glEnd();*/
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			glBegin(GL_QUADS);
			if (i % 2 == 0)
			{
				if (j % 2 == 0)
					glColor4d(1, 1, 1, 0);
				else
					glColor4d(0, 0, 0, 0);
			}
			else
			{
				if (j % 2 == 0)
					glColor4d(0, 0, 0, 0);
				else
					glColor4d(1, 1, 1, 0);
			}
			glVertex3f(side * j, side * i, 0);
			glVertex3f(side * j + side - 1, side * i, 0);
			glVertex3f(side * j + side - 1, side * i + side - 1, 0);
			glVertex3f(side * j, side * i + side - 1, 0);
			glEnd();
		}
	}
	glColor4d(1, 0, 0, 1);
	line3D(0, 0, 0, 1000, 0, 0);
	glColor4d(0, 1, 0, 1);
	line3D(0, 0, 0, 0, 1000, 0);
	glColor4d(0, 0, 1, 1);
	line3D(0, 0, 0, 0, 0, 1000);
}

Camera::Camera(double x, double y, double z)
{
	rmat = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	eye = (Mat_<double>(3, 1) << 0, 0, 0);
	up = (Mat_<double>(3, 1) << 0, 1, 0);
	center = (Mat_<double>(3, 1) << 0, 0, -1);
	initial_eye = eye.clone();
	initial_up = up.clone();
	initial_center = center.clone();
	translate(x, y, z);
}

void Camera::roll(double angle)
{
	Mat cmat = (Mat_<double>(3, 3) << 
		0, - up.at<double>(2, 0), up.at<double>(1, 0),
		up.at<double>(2, 0), 0, - up.at<double>(0, 0),
		- up.at<double>(1, 0), up.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat rollingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = rollingMat * rmat;
}

void Camera::pitch(double angle)
{
	Mat temp = center - eye;
	Mat rAxis = temp.cross(up);
	Mat cmat = (Mat_<double>(3, 3) <<
		0, -rAxis.at<double>(2, 0), rAxis.at<double>(1, 0),
		rAxis.at<double>(2, 0), 0, -rAxis.at<double>(0, 0),
		-rAxis.at<double>(1, 0), rAxis.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat pitchingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = pitchingMat * rmat;
}

void Camera::yaw(double angle)
{
	Mat rAxis = center - eye;
	Mat cmat = (Mat_<double>(3, 3) <<
		0, -rAxis.at<double>(2, 0), rAxis.at<double>(1, 0),
		rAxis.at<double>(2, 0), 0, -rAxis.at<double>(0, 0),
		- rAxis.at<double>(1, 0), rAxis.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat yawingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = yawingMat * rmat;
}

void Camera::resetRmat()
{
	rmat = Mat::eye(3, 3, CV_64F);
}
void Camera::translate(double x, double y, double z)
{
	eye.at<double>(0, 0) += x;
	eye.at<double>(1, 0) += y;
	eye.at<double>(2, 0) += z;
	center.at<double>(0 ,0) += x;
	center.at<double>(1, 0) += y;
	center.at<double>(2, 0) += z;
}

void Camera::update()
{
	center = rmat * initial_center + eye;
	up = rmat * initial_up;
	//cam.resetRmat();
}

GLfloat eye[3], up[3], center[3];
GLfloat lightpos[] = { 200.0, 150.0, -500.0, 1.0 };

void line3D(GLfloat x1, GLfloat y1,
	GLfloat z1, GLfloat x2,
	GLfloat y2, GLfloat z2)
{
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();
}

Mat glImg2mat()
{
	Size size = Size(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	Mat capImg = cv::Mat(size, CV_8UC3);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, capImg.cols, capImg.rows, 0);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, capImg.data);
	cvtColor(capImg, capImg, CV_RGB2BGR);
	flip(capImg, capImg, 0);
	return capImg;
}

GLCapture glCapture;
char str[100];
Mat textImg;

int sock;
struct sockaddr_in addr;
struct sockaddr_in client;
int len;
int sock0;
char buf[64];
Mat rvec;
Mat temp2 = Mat::eye(3, 3, CV_64F);

void display(void)
{
	textImg = Mat::zeros(300, 300, CV_64FC3);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (double)WIDTH / (double)HEIGHT, 0.5, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setGluLookAt(eye, center, up);
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	/*cam.resetRmat();
	cam.pitch(1);
	cam.update();*/
	draw();
	glutSwapBuffers();
	glFlush();

	//print property
	char str[100];
	sprintf(str, "position %4.1f, %4.1f, %4.1f", 
		cam.eye.at<double>(0, 0), cam.eye.at<double>(1, 0), cam.eye.at<double>(2, 0));
	putText(textImg, str, cv::Point(5, 15), 
		FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 200), 1, CV_AA);
	sprintf(str, "rmmat %1.4f, %1.4f, %1.4f",
		cam.rmat.at<double>(0, 0), cam.rmat.at<double>(0, 1), cam.rmat.at<double>(0, 2));
	putText(textImg, str, cv::Point(5, 25),
		FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 200), 1, CV_AA);
	sprintf(str, "      %1.4f, %1.4f, %1.4f",
		cam.rmat.at<double>(1, 0), cam.rmat.at<double>(1, 1), cam.rmat.at<double>(1, 2));
	putText(textImg, str, cv::Point(5, 35),
		FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 200), 1, CV_AA);
	sprintf(str, "      %1.4f, %1.4f, %1.4f",
		cam.rmat.at<double>(2, 0), cam.rmat.at<double>(2, 1), cam.rmat.at<double>(2, 2));
	putText(textImg, str, cv::Point(5, 45),
		FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 200), 1, CV_AA);
	imshow("property", textImg);

	//glCapture.write();
	//imshow("disp", glImg2mat());
	server.sendMat(glImg2mat());
	Rodrigues(cam.rmat, rvec);
	sprintf(buf, "%f %f %f %f %f %f",
		cam.eye.at<double>(0, 0), cam.eye.at<double>(1, 0), cam.eye.at<double>(2, 0),
		rvec.at<double>(0, 0), rvec.at<double>(1, 0), rvec.at<double>(2, 0));
	cout << cam.rmat << endl;
	send(sock, buf, sizeof(buf), 0);
}

void init()
{
	server.init(12345, "192.168.100.100");
	//glCapture.setWriteFile("output.avi");
	glClearColor(0.3, 0.3, 0.3, 0);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);*/
	namedWindow("property");
	//namedWindow("disp");

	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
	listen(sock0, 5);

	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr *)&client, &len);
}

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}

void specialKeyboard(int key, int x, int y)
{
	//cam.resetRmat();
	//cout << cam.rmat << endl;
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			cam.yaw(-1);
			break;
		}
		cam.roll(1);
		break;
	case GLUT_KEY_RIGHT:
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			cam.yaw(1);
			break;
		}
		cam.roll(-1);
		break;
	case GLUT_KEY_UP:
		cam.pitch(1);
		break;
	case GLUT_KEY_DOWN:
		cam.pitch(-1);
		break;
	default:
		break;
	}
	cam.update();
	//display();
}

void keyboard(unsigned char key, int x, int y)
{
	double temp[3] = { cam.center.at<double>(0, 0) - cam.eye.at<double>(0, 0),
		cam.center.at<double>(1, 0) - cam.eye.at<double>(1, 0),
		cam.center.at<double>(2, 0) - cam.eye.at<double>(2, 0) };
	const int speed = 3;
	switch (key)
	{
	case '[':
		cam.translate(speed * temp[0], speed * temp[1],speed * temp[2]);
		break;
	case ']':
		cam.translate(- speed * temp[0], - speed * temp[1], - speed * temp[2]);
		break;
	case 'w':
		cam.translate(speed * cam.up.at<double>(0, 0),
			speed * cam.up.at<double>(1, 0),
			speed * cam.up.at<double>(2, 0));
		break;
	case 's':
		cam.translate(- speed * cam.up.at<double>(0, 0),
			- speed * cam.up.at<double>(1, 0),
			- speed * cam.up.at<double>(2, 0));
		break;
	case 'd':
		cam.translate(speed * (temp[1] * cam.up.at<double>(2, 0) - temp[2] * cam.up.at<double>(1, 0)),
			speed * (temp[2] * cam.up.at<double>(0, 0) - temp[0] * cam.up.at<double>(2, 0)),
			speed * (temp[0] * cam.up.at<double>(1, 0) - temp[1] * cam.up.at<double>(0, 0)));
		break;
	case 'a':
		cam.translate(speed * (- temp[1] * cam.up.at<double>(2, 0) + temp[2] * cam.up.at<double>(1, 0)),
			speed * (- temp[2] * cam.up.at<double>(0, 0) + temp[0] * cam.up.at<double>(2, 0)),
			speed * (- temp[0] * cam.up.at<double>(1, 0) + temp[1] * cam.up.at<double>(0, 0)));
		break;
	}
}

void postProcess(void)
{
	closesocket(sock);
	closesocket(sock0);
	WSACleanup();
}