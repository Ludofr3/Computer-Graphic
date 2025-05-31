#include "MyGlWindow.h"
#include <iostream>
#include "drawUtils.h"
#include "timing.h"
#include <Fireworks.h>
#include "Contact.h"

//static double DEFAULT_VIEW_POINT[3] = { 150, 150, 150 };
static double DEFAULT_VIEW_POINT[3] = { 30, 30, 30 };
static double DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static double DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

MyGlWindow::MyGlWindow(int x, int y, int w, int h) :
	Fl_Gl_Window(x, y, w, h), fluid(1000.0, 0.0, 50.0) {
	particleCount = 1;
	cyclone::ParticleGravity* gravity = new cyclone::ParticleGravity(cyclone::Vector3::GRAVITY);

	world = new cyclone::ParticleWorld(particleCount * 10);
	moversConnection = MoverConnection();

	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

	fieldOfView = 45;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

	TimingData::init();
	run = 0;
	selected = -1;
}


void MyGlWindow::setupLight(float x, float y, float z)
{

	// set up the lighting
	GLfloat lightPosition[] = { 500, 900.0, 500, 1.0 };
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };

	GLfloat violetLight[] = { 0.5f, 0.1f, .5f, 1.0 };
	GLfloat whiteLight[] = { 1, 1, 1, 1.0 };
	GLfloat whiteLight2[] = { .3, .3, .3, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };


	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	lightPosition[0] = x;
	lightPosition[1] = y;
	lightPosition[2] = z;

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);
	// 
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void setupObjects(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0x0, 0x0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilMask(0x1);		// only deal with the 1st bit
}


void MyGlWindow::drawStuff()
{
	glColor4f(1, 1, 0, 0.5); //color
	polygonf(4, 20., 0., -25., 20., 0., 25., -20., 30., 25., -20., 30., -25.); //a polygon with 4 vertices
}


void MyGlWindow::drawFluidSurface() {
	glPushMatrix();
	glTranslatef(0.0f, fluid.level, 0.0f);

	// Activer la transparence
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Couleur bleu eau avec transparence
	glColor4f(0.0f, 0.3f, 0.8f, 0.5f);

	// Dessiner le plan liquide carré de 50x50
	glBegin(GL_QUADS);
	glVertex3f(-fluid.size / 2, 0, -fluid.size / 2);
	glVertex3f(-fluid.size / 2, 0, fluid.size / 2);
	glVertex3f(fluid.size / 2, 0, fluid.size / 2);
	glVertex3f(fluid.size / 2, 0, -fluid.size / 2);
	glEnd();

	// Désactiver la transparence
	glDisable(GL_BLEND);
	glPopMatrix();
}

// Corrected MyGlWindow::draw to fix floor not displaying when drawing axes
void MyGlWindow::draw() {
	glViewport(0, 0, w(), h());
	glClearColor(0.2f, 0.2f, 0.2f, 0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	/* //Draw the ground plane
	setProjection();
	setupFloor();
	glPushMatrix();
	drawFloor(200, 20);
	glPopMatrix();*/

	drawFluidSurface();

	// Désactiver la transparence
	glDisable(GL_BLEND);
	glPopMatrix();

	// Setup lighting for objects
	setupLight(m_viewer->getViewPoint().x,
		m_viewer->getViewPoint().y,
		m_viewer->getViewPoint().z);

	// ----------------------------------------------------
	// Draw world axes at origin
	// Save previous attributes (lighting, blending, line width)
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glLineWidth(3.0f);

	glBegin(GL_LINES);
	// Y axis (red)
	glColor3f(1, 0, 0);
	glVertex3f(0, 0.1f, 0);
	glVertex3f(0, 100.0f, 0);
	// X axis (green)
	glColor3f(0, 1, 0);
	glVertex3f(0, 0.1f, 0);
	glVertex3f(100.0f, 0.1f, 0);
	// Z axis (blue)
	glColor3f(0, 0, 1);
	glVertex3f(0, 0.1f, 0);
	glVertex3f(0, 0.1f, 100.0f);
	glEnd();

	glPopAttrib(); // restores lighting, blending, line width, color
	// ----------------------------------------------------

	// Draw shadows
	setupShadows();
	moversConnection.draw(1);
	unsetupShadows();

	// Draw movers (cubes and local axes)
	glEnable(GL_LIGHTING);
	glLineWidth(3.0f);
	moversConnection.draw(0);
	glLineWidth(1.0f);

	// Draw HUD text
	glDisable(GL_LIGHTING);
	putText("Ludovic de Chavagnac - 7701625", 0, 0, 1, 1, 1);

	// Restore projection for overlay or future draws
	glViewport(0, 0, w(), h());
	setProjection();
	glEnable(GL_COLOR_MATERIAL);
}

void MyGlWindow::test()
{
	if (!moversConnection.movers.empty()) {
		Mover* box = moversConnection.movers[0];
		box->body->addForceAtBodyPoint(cyclone::Vector3(5000, 30000, 1000), cyclone::Vector3(0.5, 1, 0.5));
		run = 1;
		ui->value(1);
	}
}

void MyGlWindow::update()
{

	TimingData::get().update();

	if (!run) 
		return;

	float duration = (float)TimingData::get().lastFrameDuration * 0.003;
	if (duration <= 0.0f) return;

	world->runPhysics(duration);

	// Appliquer la poussée d'Archimède et la traînée à chaque Mover
	for (auto& mover : moversConnection.movers) {
		cyclone::Vector3 archimedesForce = fluid.getArchimedesForce(*mover);
		cyclone::Vector3 dragForce = fluid.getDragForce(*mover);

		mover->body->addForce(archimedesForce);
		mover->body->addForce(dragForce);
	}

	moversConnection.update(duration);
}


void MyGlWindow::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection(false);

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	moversConnection.draw(0);

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	std::cout << "hit: " << hits << std::endl;
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		for (int i = 0; i < hits; i++) {
			std::cout << "Hit " << i << " : " << buf[i * 4 + 3] << std::endl;
		}
		selected = buf[3] - 1;
	}
	else {// nothing hit, nothing selected
		selected = -1;
	}
	//printf("Selected Cube %d\n", selectedCube);
}

void MyGlWindow::setProjection(int clearProjection) {
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, w(), h());
	if (clearProjection)
		glLoadIdentity();
	// compute the aspect ratio so we don't distort things
	double aspect = ((double)w()) / ((double)h());
	gluPerspective(fieldOfView, aspect, 1, 1000);

	// put the camera where we want it to be
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// use the transformation in the ArcBall

	gluLookAt(
		m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z,
		m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z,
		m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z
	);

//	glDisable(GL_BLEND);
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e) {

	switch (e) {
	case FL_SHOW:		// you must handle this, or not be seen!
		show();
		return 1;
	case FL_PUSH:
	{

		m_pressedMouseButton = Fl::event_button();
		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();

		if (m_pressedMouseButton == 1) {
			doPick();
			if (selected >= 0) {
				t1 = clock();
				p1 = moversConnection.movers[selected]->body->getPosition();
				std::cout << "picked" << std::endl;
			}
			damage(1);
			return 1;
		};
		break;


	}
	damage(1);
	return 1;
	case FL_RELEASE:
		if (selected >= 0) {
			cyclone::Vector3 vel;
			t2 = clock();
			p2 = moversConnection.movers[selected]->body->getPosition();
			vel = 100 * (p2 - p1) / (t2 - t1);
			moversConnection.movers[selected]->body->setVelocity(vel);
			run = 1;
			ui->value(1);
			selected = -1;
		}
		m_pressedMouseButton = -1;
		damage(1);
		return 1;
	case FL_DRAG: // if the user drags the mouse
	{


		if (selected >= 0 && m_pressedMouseButton == 1) {

						
			double r1x, r1y, r1z, r2x, r2y, r2z;
			getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

			double rx, ry, rz;
				
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
				static_cast<double>(moversConnection.movers[selected]->body->getPosition().x),
				static_cast<double>(moversConnection.movers[selected]->body->getPosition().y),
				static_cast<double>(moversConnection.movers[selected]->body->getPosition().z),
			rx, ry, rz,
			(Fl::event_state() & FL_CTRL) != 0);
			std::cout << "selected: " << selected << std::endl;
			moversConnection.movers[selected]->body->setPosition(rx, ry, rz);
			damage(1);
		}
		else {


			double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
			double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

			if (m_pressedMouseButton == 1) {
				m_viewer->rotate(fractionChangeX, fractionChangeY);
			}
			else if (m_pressedMouseButton == 2) {
				m_viewer->zoom(fractionChangeY);
			}
			else if (m_pressedMouseButton == 3) {
				m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
			}
			else {
				std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
			}

			m_lastMouseX = Fl::event_x();
			m_lastMouseY = Fl::event_y();
			redraw();
			damage(1);
		}
	}

	return 1;

	case FL_KEYBOARD:
		return 0;

	default:
		return 0;
	}
}

void MyGlWindow::getMouseNDC(float& x, float& y) {
	// notice, we put everything into doubles so we can do the math
	float mx = (float)Fl::event_x();	// remeber where the mouse went down
	float my = (float)Fl::event_y();

	// we will assume that the viewport is the same as the window size
	float wd = (float)w();
	float hd = (float)h();

	// remember that FlTk has Y going the wrong way!
	my = hd - my;

	x = (mx / wd) * 2.0f - 1.f;
	y = (my / hd) * 2.0f - 1.f;
}

void drawStrokeText(char* string, int x, int y, int z)
{
	char* c;
	glPushMatrix();
	glTranslated(x, y + 8, z);
	glScaled(0.2, 0.2, 0.2);
	for (c = string; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();
}

void MyGlWindow::putText(char* string, int x, int y, float r, float g, float b)
{
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ortho();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3f(r, g, b);
	drawStrokeText(string, x, y, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void MyGlWindow::step()
{
	TimingData::get().update();
	float duration = 0.03; // or 0.06
	moversConnection.update(duration);
	std::cout << "step" << std::endl;
}

void MyGlWindow::initCable(cyclone::Particle* pA, cyclone::Particle* pB, float maxLen) {
	cyclone::ParticleCable* c = new cyclone::ParticleCable();
	c->particle[0] = pA;
	c->particle[1] = pB;
	c->maxLength = maxLen;
	c->restitution = 0.1f;
	cables.push_back(c);
}

void MyGlWindow::initRod(cyclone::Particle* pA, cyclone::Particle* pB) {
	cyclone::ParticleRod* r = new cyclone::ParticleRod();
	r->particle[0] = pA;
	r->particle[1] = pB;
	r->length = 2.0f;

	rods.push_back(r);
}

void MyGlWindow::initCableConstraint(cyclone::Particle* p, float maxLen) {
	cyclone::ParticleCableConstraint* c = new cyclone::ParticleCableConstraint();
	c->particle = p;
	cyclone::Vector3 ppos = p->getPosition();
	c->anchor = cyclone::Vector3(ppos.x, 10, ppos.z);
	c->maxLength = maxLen;
	c->restitution = 0.5f;
	supports.push_back(c);
}


MyGlWindow::~MyGlWindow() {
	//delete resolver;
	for (auto generator : contactGenerators) {
		delete generator; // Libérez aussi les générateurs de contacts
	}
	delete m_viewer; // Si nécessaire
}

void MyGlWindow::testValue(float t) {
	if (moversConnection.movers.size() < 3) { return; }
	Mover* A = moversConnection.movers[0];
	Mover* B = moversConnection.movers[1];
	Mover* C = moversConnection.movers[2];

	// --- LERP pour la position ---
	cyclone::Vector3 posA = A->body->getPosition();
	cyclone::Vector3 posC = C->body->getPosition();
	cyclone::Vector3 posB = posA * (1 - t) + posC * t;
	B->body->setPosition(posB);

	// --- SLERP pour l’orientation ---
	cyclone::Quaternion qA = A->body->getOrientation();
	cyclone::Quaternion qC = C->body->getOrientation();

	if (qA.r * qC.r + qA.i * qC.i + qA.j * qC.j + qA.k * qC.k < 0.0f) {
		qC.r = -qC.r;
		qC.i = -qC.i;
		qC.j = -qC.j;
		qC.k = -qC.k;
	}

	float dot = qA.r * qC.r + qA.i * qC.i + qA.j * qC.j + qA.k * qC.k;
	float theta = std::acos(dot);
	if (std::abs(theta) < 0.0001f) {
		B->body->setOrientation(qA);
	}
	else {
		float sin_theta = std::sin(theta);
		float w1 = std::sin((1 - t) * theta) / sin_theta;
		float w2 = std::sin(t * theta) / sin_theta;

		cyclone::Quaternion qB;
		qB.r = w1 * qA.r + w2 * qC.r;
		qB.i = w1 * qA.i + w2 * qC.i;
		qB.j = w1 * qA.j + w2 * qC.j;
		qB.k = w1 * qA.k + w2 * qC.k;
		qB.normalise();

		B->body->setOrientation(qB);
	}

	B->body->getTransform().setOrientationAndPos(B->body->getOrientation(), posB);
}
