#include "Engine.h"
#define RASPBERRY_PI_PORT 60010

static Engine*		_Engine		= 0;
static bool			_mode		= false;
const char* RASPBERRY_PI_IP_ADDRESS = (char*)"10.106.1.80";

static void display(void);
static void idle(void);
static void keyboard(unsigned char key, int x, int y);
static void mousebutton(int btn, int state, int x, int y);
static void mousemotion(int x, int y);
static void resize(int x, int y);
static void specialMouse(void);

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	
	_Engine = new Engine("Fluid Simulation System", argc, argv);
	glutMainLoop();
	delete _Engine;

	return 0;
}

static void display(void) {
	_Engine->Draw();
}
static void idle(void) {
	_Engine->Update();
}
static void keyboard(unsigned char key, int x, int y) {
	_Engine->Keyboard(key, x, y);
}
static void mousebutton(int btn, int state, int x, int y) {
	_Engine->MouseButton(btn, state, x, y);
}
static void mousemotion(int x, int y) {
	_Engine->MouseMotion(x, y);
}
static void resize(int x, int y) {
	_Engine->Resize(x, y);
}
static void specialMouse(void) {
	_Engine->SpecialMouseMode();
}

Engine::Engine(const char* windowTitle, int argc, char* argv[]) {
	_WinX					= 600;
	_WinY					= 600;
	_LeftDown				= false;
	_MiddleDown				= false;
	_RightDown				= false;
	_MouseX					= 0;
	_MouseY					= 0;
	_prevTime				= 0;
	_currentTime			= 0;
	_deltaTime				= 0;

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(_WinX, _WinY);
	glutInitWindowPosition(150, 50);
	_WindowHandle = glutCreateWindow(windowTitle);
	glutSetWindowTitle(windowTitle);
	glutSetWindow(_WindowHandle);

	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemotion);
	glutPassiveMotionFunc(mousemotion);
	glutReshapeFunc(resize);

	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	InstanceProgram = new ShaderProgram("resources/instance.glsl", ShaderProgram::eRender);
	HapticInstanceProgram = new ShaderProgram("resources/hapticSphereInstance.glsl", ShaderProgram::eRender);

	Cam = new Camera;
	Cam->SetAspect(float(_WinX) / float(_WinY));

	HapticSystem = new Controller(0.6f);
	system = new System(10, 0.02f, 1000, 1, 1.04f, 0.15f, -9.8f, 0.2f, *HapticSystem);
	system->ConnectingNetwork(RASPBERRY_PI_IP_ADDRESS, RASPBERRY_PI_PORT);
}

Engine::~Engine() {
	delete Cam;

	glFinish();
	glutDestroyWindow(_WindowHandle);
}

void Engine::Update(void) {
	_currentTime = glutGet(GLUT_ELAPSED_TIME);
	_deltaTime = (_currentTime - _prevTime) / 1000;
	_prevTime = _currentTime;

	Cam->Update();

	system->update(_deltaTime);

	glutSetWindow(_WindowHandle);
	glutPostRedisplay();
}

void Engine::Reset(void) {
	//Cam->Reset();
	Cam->SetAspect(float(_WinX) / float(_WinY));
}

void Engine::Draw(void) {
	glViewport(0, 0, _WinX, _WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	system->draw(Cam->GetViewProjectMtx(), InstanceProgram->GetProgramID());
	if (system->HapticMode) {
		HapticSystem->ControllerSphereDraw(Cam->GetViewProjectMtx(), HapticInstanceProgram->GetProgramID());
	}
	else {
		HapticSystem->HapticParticleReset();
	}
	glFinish();
	glutSwapBuffers();
}

void Engine::Quit(void) {
	glFinish();
	glutDestroyWindow(_WindowHandle);
	exit(0);
}

void Engine::Resize(int x, int y) {
	_WinX = x;
	_WinY = y;
	Cam->SetAspect(float(_WinX) / float(_WinY));
}

void Engine::Keyboard(int key, int x, int y) {
	switch (key) {
	case 0x1b: case 'q': case 'Q':
		Quit();
		break;
	case 'r': case 'R':
		Reset();
		system->reset(*HapticSystem);
		break;
	case 's': case 'S':
		system->startSimulation();
		break;
	case 'm': case 'M':
		system->HapticMode = !system->HapticMode;
		break;
	default:
		break;

	}
}

void Engine::MouseButton(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON) {
		_LeftDown = (state == GLUT_DOWN);
	}
	else if (btn == GLUT_MIDDLE_BUTTON) {
		_MiddleDown = (state == GLUT_DOWN);
	}
	else if (btn == GLUT_RIGHT_BUTTON) {
		_RightDown = (state == GLUT_DOWN);
	}
}

void Engine::MouseMotion(int nx, int ny) {
	int maxDelta = 100;
	int dx = glm::clamp(nx - _MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-(ny - _MouseY), -maxDelta, maxDelta);

	_MouseX = nx;
	_MouseY = ny;

	if (!system->HapticMode) {
		if (_LeftDown) {
			const float rate = 1.0f;
			Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
			Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
		}
		if (_RightDown) {
			const float rate = 0.005f;
			float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
			Cam->SetDistance(dist);
		}
	}
	else {
		if (_LeftDown) {
			const float rate = 1.0f;
			Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
			Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
		}
		if (_RightDown) {
			HapticSystem->HapticParticleHeightMoving(dy);
		}
		else {
			HapticSystem->HapticParticlePosition(dx, dy, 0);
		}
	}
}

void Engine::VerticesLineDraw(void) {
	glDisable(GL_LIGHTING);
	glLineWidth(3.f);

	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
	glVertex3f(1.0,		1.0		, 1.0);
	glVertex3f(1.0,		3.59808	, 1.0);
	glVertex3f(3.59808, 3.59808	, 1.0);
	glVertex3f(3.59808, 1.0		, 1.0);
	glEnd();
}
