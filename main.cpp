#include <vector>
#include <fstream>

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/glut.h>
#include <gl/GLAUX.h>

#include "AstronomicalObject.h"

#pragma comment( lib, "glut32.lib"  )
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

void initialize();
void finalize();

// glut callback functions
void reshape(int w, int h);
void display();
void idle();
void finalize();
void timer(int timer_id);

void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

// menu
void menu_main(int item);
void menu_view(int item);
void menu_speed(int item);
void menu_realDistance(int item);
//
int win_width = 800;
int win_height = 800;
double win_aspect_ratio = ((double)win_width / (double)win_height);
int time_interval = 1;

//
const double PI = 3.141593;
const double DEGREE = (PI / 180.0);
double degree2radian(double degree) { return PI / 180.0 * degree; }
double cam_x, cam_y, cam_z;
double cam_dist = 1.0;
double cam_theta = 0.0;
double cam_phi = degree2radian(90.0);
double phi_upperBound = degree2radian(170.0);
double phi_lowerBound = degree2radian(10.0);
SolarSystem viewObject = SolarSystem::SUN;
void setupViewing(SolarSystem elementIndex);

//Texture (the order is important)
const unsigned int numTextures = SolarSystem::NUM_ELEMENTS + 1;// the last one is for font texture
AUX_RGBImageRec *texRec[numTextures];
AUX_RGBImageRec *texFont;
GLuint texID[numTextures];
void loadTexture();

// drawing objects
void drawSphere(SolarSystem elementIndex);
void drawScene();

// Font rasterization
void ChangeSize(int w, int h);
void BuildFont();
void glPrint(const char *fmt, ...);
void KillFont(GLvoid);
static int		base;
HDC				hDC;
HWND			hWnd;
void drawFontOn(SolarSystem elementIndex);

// Functions used to generate objects
struct Vector {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
void normalize(Vector &vector);

// setup material functions
void setupMaterial_silver();

// Functions for enable or disable lighting
void enableLighting(GLenum light);
void disableLighting(GLenum light);

bool is_light0_enabled = false;
bool is_light1_enabled = false;

// Objects in the Solar System
AstronomicalObject sun(SolarSystem::SUN, NULL);
AstronomicalObject mercury(SolarSystem::MERCURY, &sun);
AstronomicalObject venus(SolarSystem::VENUS, &sun);
AstronomicalObject earth(SolarSystem::EARTH, &sun);
AstronomicalObject mars(SolarSystem::MARS, &sun);
AstronomicalObject jupiter(SolarSystem::JUPITER, &sun);
AstronomicalObject saturn(SolarSystem::SATURN, &sun);
AstronomicalObject uranus(SolarSystem::URANUS, &sun);
AstronomicalObject neptune(SolarSystem::NEPTUNE, &sun);
AstronomicalObject moon(SolarSystem::MOON, &earth);

void setRealDistanceMode(bool realDistanceMode)
{
	sun.setRealDistanceMode(realDistanceMode);
	mercury.setRealDistanceMode(realDistanceMode);
	venus.setRealDistanceMode(realDistanceMode);
	earth.setRealDistanceMode(realDistanceMode);
	mars.setRealDistanceMode(realDistanceMode);
	jupiter.setRealDistanceMode(realDistanceMode);
	saturn.setRealDistanceMode(realDistanceMode);
	uranus.setRealDistanceMode(realDistanceMode);
	neptune.setRealDistanceMode(realDistanceMode);
	moon.setRealDistanceMode(realDistanceMode);
}
AstronomicalObject & getAstronomicalObject(SolarSystem elementIndex)
{
	switch (elementIndex)
	{
	case SolarSystem::SUN:
		return sun;
	case SolarSystem::MERCURY:
		return mercury;
	case SolarSystem::VENUS:
		return venus;
	case SolarSystem::EARTH:
		return earth;
	case SolarSystem::MARS:
		return mars;
	case SolarSystem::JUPITER:
		return jupiter;
	case SolarSystem::SATURN:
		return saturn;
	case SolarSystem::URANUS:
		return uranus;
	case SolarSystem::NEPTUNE:
		return neptune;
	case SolarSystem::MOON:
		return moon;
	}
}

//
void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(win_width, win_height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Solar System");
	
	// call-back initialization
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(time_interval, timer, 0);

	int imenu_view = glutCreateMenu(menu_view);
	glutAddMenuEntry("Sun", SUN);
	glutAddMenuEntry("Mercury", MERCURY);
	glutAddMenuEntry("Venus", VENUS);
	glutAddMenuEntry("Earth", EARTH);
	glutAddMenuEntry("Mars", MARS);
	glutAddMenuEntry("Jupiter", JUPITER);
	glutAddMenuEntry("Saturn", SATURN);
	glutAddMenuEntry("Uranus", URANUS);
	glutAddMenuEntry("Neptune", NEPTUNE);
	glutAddMenuEntry("Moon", MOON);

	int imenu_realDistance = glutCreateMenu(menu_realDistance);
	glutAddMenuEntry("Real Distance Mode", 0);
	glutAddMenuEntry("Close Mode", 1);

	glutCreateMenu(menu_main);
	glutAddSubMenu("View", imenu_view);
	glutAddSubMenu("Distance Mode", imenu_realDistance);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	initialize();
	glutMainLoop();
}

void initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	loadTexture();
	hWnd = GetActiveWindow();
	hDC = GetDC(hWnd);
	BuildFont();
}

void setupProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, win_aspect_ratio, 0.1, 1000000.0);
}

void setupViewing(SolarSystem elementIndex)
{
	AstronomicalObject& ao = getAstronomicalObject(elementIndex);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	cam_x = ao.getX() + (2 * ao.getRadius() + cam_dist) * sin(cam_phi) * sin(cam_theta + ao.getRadianRevolution());
	cam_y = ao.getY() + (2 * ao.getRadius() + cam_dist) * cos(cam_phi);
	cam_z = ao.getZ() + (2 * ao.getRadius() + cam_dist) * sin(cam_phi) * cos(cam_theta + ao.getRadianRevolution());

	gluLookAt(
		cam_x, cam_y, cam_z,
		ao.getX(), ao.getY(), ao.getZ(),
		0, 1, 0);
}

void setupLighting()
{
	// Light #0
	GLfloat ambient0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position0[4] = { 0.0, 1.0, 0.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);

	glEnable(GL_LIGHT0);
	is_light0_enabled = true;

	// Light #1
	GLfloat ambient1[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse1[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular1[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position1[4] = { 0.0, -1.0, 0.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);

	glEnable(GL_LIGHT1);
	is_light1_enabled = true;
}

void setupMaterial_silver()
{
	GLfloat ambient_m[4] = { 0.19225, 0.19225, 0.19225, 1.0 };
	GLfloat diffuse_m[4] = { 0.50754, 0.50754, 0.50754, 1.0 };
	GLfloat specular_m[4] = { 0.508273, 0.508273, 0.508273, 1.0 };
	GLfloat shininess_m = 0.4;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_m);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_m);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_m);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess_m);
}

void drawSphere(SolarSystem elementIndex)
{
	AstronomicalObject & ao = getAstronomicalObject(elementIndex);
	GLfloat theta = 0;
	GLfloat phi = 0;
	GLfloat radius = ao.getRadius();
	GLint slice = 36;
	GLint stack = 18;
	const GLfloat delta_theta = (360 / slice) * DEGREE; // the increment of theta(radian)
	const GLfloat delta_phi = (180 / stack) * DEGREE; // the increment of phi(radian)
	
	glBindTexture(GL_TEXTURE_2D, texID[numTextures-1]);
	drawFontOn(elementIndex);
	setupMaterial_silver();
	glBindTexture(GL_TEXTURE_2D, texID[elementIndex]);
	

	glPushMatrix();
	{
		if(&ao.getRevoluteObject() != NULL)
			glTranslatef(ao.getRevoluteObject().getX(), ao.getRevoluteObject().getY(), ao.getRevoluteObject().getZ());
		glRotatef(ao.getAngleRevolution(), 0, 1, 0); // Revolution
		glTranslatef(0, 0, ao.getDistanceRevolution()); // Revolution
		glRotatef(ao.getAngleAxialTilt(), 0, 0, 1); // axial tilt
		glRotatef(ao.getAngleRotation(), 0, 1, 0); // Rotation
		for (theta = 0; theta < 2 * PI; theta += delta_theta)
			//for (int i = 0; i != slice; ++i, theta += delta_theta)
		{

			glBegin(GL_TRIANGLE_STRIP);
			{
				glNormal3f(0, 1, 0);
				glTexCoord2f(
					1.0 - theta / (2 * PI),
					1.0
					);
				glVertex3f(0, radius, 0);
				for (phi = delta_phi; phi < PI; phi += delta_phi)
					//for (int j = 0, phi = delta_phi; j != (stack-2); ++j, phi += delta_phi)
				{
					glNormal3f(
						sin(phi)*cos(theta),
						cos(phi),
						sin(phi)*sin(theta)
						);
					glTexCoord2f(
						1.0 - theta / (2 * PI),
						1.0 - phi / PI
						);
					glVertex3f(
						radius*sin(phi)*cos(theta),
						radius*cos(phi),
						radius*sin(phi)*sin(theta)
						);
					glNormal3f(
						sin(phi)*cos(theta + delta_theta),
						cos(phi),
						sin(phi)*sin(theta + delta_theta)
						);
					glTexCoord2f(
						1.0 - (theta + delta_theta) / (2 * PI),
						1.0 - (phi) / PI
						);
					glVertex3f(
						radius*sin(phi)*cos(theta + delta_theta),
						radius*cos(phi),
						radius*sin(phi)*sin(theta + delta_theta)
						);
				}
				glNormal3f(0, -1, 0);
				glTexCoord2f(
					1.0 - theta / (2 * PI),
					0.0
					);
				glVertex3f(0, -radius, 0);
			}
			glEnd();
		}
	}
	glPopMatrix();
}


void drawScene()
{
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	{
		drawSphere(SUN); 
		drawSphere(MERCURY); 
		drawSphere(VENUS);
		drawSphere(EARTH);
		drawSphere(MOON);
		drawSphere(MARS);
		drawSphere(JUPITER);
		drawSphere(SATURN);
		drawSphere(URANUS);
		drawSphere(NEPTUNE);
		drawSphere(MOON);
		
	}
	glPopMatrix();
}

void loadTexture()
{
	texRec[SUN] = auxDIBImageLoad("texture_sun.bmp");
	texRec[MERCURY] = auxDIBImageLoad("texture_mercury.bmp");
	texRec[VENUS] = auxDIBImageLoad("texture_venus.bmp");
	texRec[EARTH] = auxDIBImageLoad("texture_earth.bmp");
	texRec[MARS] = auxDIBImageLoad("texture_mars.bmp");
	texRec[JUPITER] = auxDIBImageLoad("texture_jupiter.bmp");
	texRec[SATURN] = auxDIBImageLoad("texture_saturn.bmp");
	texRec[URANUS] = auxDIBImageLoad("texture_uranus.bmp");
	texRec[NEPTUNE] = auxDIBImageLoad("texture_neptune.bmp");
	texRec[MOON] = auxDIBImageLoad("texture_moon.bmp");
	texRec[numTextures - 1] = auxDIBImageLoad("texture_font.bmp");

	glGenTextures(numTextures, &texID[0]); // Load 2 textures
	for (register int i = 0; i < numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texID[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec[i]->sizeX, texRec[i]->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, texRec[i]->data);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete[] texRec[i]->data;
		delete texRec[i];
	}
}

void display()
{
	glClearColor(0.1, 0.1, 0.1, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupProjection();
	setupViewing(viewObject);
	setupLighting();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	drawScene();

	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	win_width = w;
	win_height = h;
	win_aspect_ratio = (double)w / (double)h;

	glViewport(0, 0, win_width, win_height);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'D':
		cam_dist += 1;
		//if (cam_dist > 10.0)	cam_dist = 10.0;
		break;
	case 'd':
		cam_dist -= 1;
		//if (cam_dist < 0.0)	cam_dist = 0.0;
		break;
	case 9:	// 'tab' | GL_LIGHT0
		if (is_light0_enabled)	disableLighting(GL_LIGHT0);
		else					enableLighting(GL_LIGHT0);
		break;
	case '1': // GL_LIGHT1
		if (is_light1_enabled)	disableLighting(GL_LIGHT1);
		else					enableLighting(GL_LIGHT1);
		break;
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		cam_theta += PI / 180.0;
		break;

	case GLUT_KEY_LEFT:
		cam_theta -= PI / 180.0;
		break;

	case GLUT_KEY_DOWN:
		cam_phi += PI / 180.0;
		
		if (cam_phi > phi_upperBound)	cam_phi = phi_upperBound;
		break;
	case GLUT_KEY_UP:
		cam_phi -= PI / 180.0;
		if (cam_phi < phi_lowerBound)	cam_phi = phi_lowerBound;
		break;
	}
	glutPostRedisplay();
}

void timer(int timer_id)
{
	sun.increaseRotation();
	sun.increaseRevolution();
	mercury.increaseRotation();
	mercury.increaseRevolution();
	venus.increaseRotation();
	venus.increaseRevolution();
	earth.increaseRotation();
	earth.increaseRevolution();
	mars.increaseRotation();
	mars.increaseRevolution();
	jupiter.increaseRotation();
	jupiter.increaseRevolution();
	saturn.increaseRotation();
	saturn.increaseRevolution();
	uranus.increaseRotation();
	uranus.increaseRevolution();
	neptune.increaseRotation();
	neptune.increaseRevolution();
	moon.increaseRotation();
	moon.increaseRevolution();

	glutPostRedisplay();
	glutTimerFunc(time_interval, timer, 0);
}

void menu_main(int item)
{

}

void menu_view(int item)
{
	switch (item)
	{
	case SolarSystem::SUN:
		viewObject = SUN;
		break;
	case SolarSystem::MERCURY:
		viewObject = MERCURY;
		break;
	case SolarSystem::VENUS:
		viewObject = VENUS;
		break;
	case SolarSystem::EARTH:
		viewObject = EARTH;
		break;
	case SolarSystem::MARS:
		viewObject = MARS;
		break;
	case SolarSystem::JUPITER:
		viewObject = JUPITER;
		break;
	case SolarSystem::SATURN:
		viewObject = SATURN;
		break;
	case SolarSystem::URANUS:
		viewObject = URANUS;
		break;
	case SolarSystem::NEPTUNE:
		viewObject = NEPTUNE;
		break;
	case SolarSystem::MOON:
		viewObject = MOON;
		break;
	}
	glutPostRedisplay();
}

void menu_realDistance(int item)
{
	switch (item)
	{
	case 0:// Real Distance Mode
		setRealDistanceMode(true);
		break;
	case 1: // Close Distance Mode
		setRealDistanceMode(false);
		break;
	}
}



void enableLighting(GLenum light)
{
	switch (light) {
	case GL_LIGHT0:
		glEnable(GL_LIGHT0);
		is_light0_enabled = true;
		break;
	case GL_LIGHT1:
		glEnable(GL_LIGHT1);
		is_light1_enabled = true;
		break;
	}
}

void disableLighting(GLenum light)
{
	switch (light) {
	case GL_LIGHT0:
		glDisable(GL_LIGHT0);
		is_light0_enabled = false;
		break;
	case GL_LIGHT1:
		glDisable(GL_LIGHT1);
		is_light1_enabled = false;
		break;

	}
}

void BuildFont()
{
	HFONT font;
	HFONT oldfont;

	base = glGenLists(96);

	font = CreateFont(
		-20,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"Courier New"
		);
	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

void glPrint(const char *fmt, ...)
{
	char text[256];
	va_list ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	{
		vsprintf_s(text, fmt, ap);
	}
	va_end(ap);

	glPushAttrib(GL_LIST_BIT);
	{
		glListBase(base - 32);
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	}
	glPopAttrib();
}

void KillFont(GLvoid)
{
	glDeleteLists(base, 96);
}

void normalize(Vector &vector)
{
	GLfloat norm = sqrtf(
		vector.x * vector.x + 
		vector.y * vector.y +
		vector.z * vector.z
		);
	vector.x = vector.x / norm;
	vector.y = vector.y / norm;
	vector.z = vector.z / norm;
}

void drawFontOn(SolarSystem elementIndex)
{
	Vector fontVector;
	AstronomicalObject &ao = getAstronomicalObject(elementIndex);
	fontVector.x = ao.getX() - cam_x;
	fontVector.y = ao.getY() - cam_y;
	fontVector.z = ao.getZ() - cam_z;
	normalize(fontVector);
	glColor3f(1, 1, 1);
	glRasterPos3f(
		fontVector.x + cam_x,
		fontVector.y + cam_y,
		fontVector.z + cam_z
		);
	switch (elementIndex)
	{
	case SUN:
		glPrint("Sun");
		break;
	case MERCURY:
		glPrint("Mercury");
		break;
	case VENUS:
		glPrint("Venus");
		break;
	case EARTH:
		glPrint("Earth");
		break;
	case MARS:
		glPrint("Mars");
		break;
	case JUPITER:
		glPrint("Jupiter");
		break;
	case SATURN:
		glPrint("Saturn");
		break;
	case URANUS:
		glPrint("Uranus");
		break;
	case NEPTUNE:
		glPrint("Neptune");
		break;
	case MOON:
		glPrint("Moon");
		break;
	}
}