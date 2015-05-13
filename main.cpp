#include <iostream>
#include <opencv2/opencv.hpp>

#include "chessboard_genelator.h"

using namespace std;

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	int mainWindow = glutCreateWindow("generated chessboard");
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, NULL);
	glutSpecialFunc(specialKeyboard);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	postProcess();
	return 0;
}