
#define NOMINMAX


#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Value_Slider.H>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Fl/Fl_Double_Window.h>
#include <Fl/Fl_Light_Button.h>


#include "stdio.h"
#include "math.h"
#include "3DUtils.h"

#include "Viewer.h"
#include <vector>

#include "core.h"
#include "Object.h"
#include <MoverConnection.h>
#include <Plane.h>

class MyGlWindow : public Fl_Gl_Window {
public:
	MyGlWindow(int x, int y, int w, int h);


	Fl_Slider* time;
	Fl_Light_Button* ui;

	int run;
	int selected;

	//3D position when mouse is pushing
	cyclone::Vector3 p1;
	//3D position when mouse is releasing
	cyclone::Vector3 p2;
	//times
	clock_t t1, t2; //time: t1=clock() and t2=clock()

	void update();
	void drawStuff();
	void doPick();
	void test();
	void step();

private:
	void draw();					// standard FlTk
	int handle(int);				// standard FlTk
	float fieldOfView;
	Viewer *m_viewer;
	//std::vector<Mover> movers;
	//std::vector<Plane> planes;
	MoverConnection moversConnection;

	void setProjection(int clearProjection = 1);
	void getMouseNDC(float& x, float& y);
	void setupLight(float x, float y, float z);
	void putText(char* string, int x, int y, float r, float g, float b);
};

