//==============================================================================
/*
\author    Ahmad Tahir and Nasir Osman
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
#include "MyBall.h"
#include "MySpring.h"
#include "MyMaterial.h"
#include "MyProxyAlgorithm.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
C_STEREO_DISABLED:            Stereo is disabled
C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


// live counter
int climberLives;
const int MAX_DEVICES = 2;


const int MAX_NUM_OF_LIVES = 3;



//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cDirectionalLight *light;

// secondary light source to increase intensity of light
cDirectionalLight *light_2;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice[MAX_DEVICES];

// indicators of each device being in collision 
bool isHoldingRock[MAX_DEVICES];

// a label to display the rates [Hz] at which the simulation is running
cLabel* labelRates;

// a label to display debug info
cLabel* labelDebugInfo;

// a label to display game result
cLabel* labelGameResult;

// a label to display game status
cLabel* labelGameStatus;


// Lift Force for shifting workspace up
cVector3d lift;

// Climber's local position
cVector3d climberPos;

// local position vector of the devices
cVector3d localDevicePos1;
cVector3d localDevicePos2;

// a small sphere (cursor) representing the haptic device 
cShapeSphere* cursor;

// workspace vector for z component
cVector3d workspaceVectorInZ;

// workspace vector 
cVector3d workspaceVector;

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = false;

// a state variable for whether debugging mode is enabled
bool debugMode = false;

// a variable for tracking score
int userScore = 0;

// reset game state
bool resetGame = false;

// frame counter for updating score
int hapticCounter = 0;
// win state of the user 
bool winState = false;

// Mesh texture for the "monkey"
cMesh* mesh;
// whether the player has at least one hold on the cliff
bool isHolding = false;

// lose state of the player
bool loseState = false;

// score accumulator based on time
int scoreAccum = 0;

// if the game has ended (user lost all lives)
bool gameOver = false;

// start state of the player 
bool startState = true;

// ready state for climbing
bool goodToClimb = false;

// timer object
cPrecisionClock timer;

// Game Messages
std::string loseMessage = "";
std::string winMessage = "Woah, you actually made it to the top?! Btw, how are your arms feeling? ";


// buttons from haptic device

bool buttonCentre = false;
bool buttonLeft = false;
bool buttonTop = false;
bool buttonRight = false;


// default top and bottom cliff conditions
double topCliff = 0.53;
double botCliff = -0.15;

// offset of the workspace
cVector3d offset;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// a pointer to the custom proxy rendering algorithm inside the tool
MyProxyAlgorithm* proxyAlgorithm;


// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

// variable for managing scene
int sceneType = 2;

//****************************************************************************************************************VARIABLES
//add 3d mesh file
cMultiMesh * monkey;								// we totally decided to call our cliff models monkeys. Why? Because why not! :)
cMultiMesh * monkey2;								
cMultiMesh * monkey3;

cMesh * mesh2;
cMesh * mesh3;

Person* person;
MySpring* springs[MAX_DEVICES];
MyBall* tool[MAX_DEVICES];							// tool cursor for chai3d collision 
													//cToolCursor *tool[MAX_DEVICES];				//the new cursor

int numHapticDevices;

const double vel_g = 9.81;
double mass_1 = 1.0;	//kg
cVector3d debugVector;

//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

//****************************************************************************************************************FUNCTIONS
cVector3d calcForceGravity()
{
	return cVector3d(0.0, 0.0, vel_g * mass_1 * -1);
};
cVector3d calcNetForces(cVector3d f_tool)
{
	return calcForceGravity() + f_tool;
};

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);

void reset_Game();
// global vector for updating tool position
cVector3d updateToolPos;
cVector3d lookAtPos(0.0, 0.0, 0.0);
cVector3d lookAtPos_1(0.0, 0.0, 0.0);
cVector3d lookAtPos_2(0.0, 0.0, 0.05);
cVector3d cameraPos(0.075, 0.0, 0.0);
// global workspace centre position vector
cVector3d workspaceCentre(0.0, 0.0, 0.01);


// initial game parameters
cVector3d initClimberPos(0.08, 0.0, 0.01);
cVector3d initCameraPos(0.13, 0.0, 0.05);
cVector3d initLookAtPos(0.08, 0.0, 0.05);

cTexture2dPtr albedoMap;
void sceneSwitch()
{
	switch (sceneType) 
	{
	case 1:
		world->deleteAllChildren();
		world->addChild(light);
		world->addChild(light_2);
		world->addChild(camera);
		world->addChild(person->m_sphere);
		for (int i = 0; i < numHapticDevices; i++)
		{
			world->addChild(tool[i]->m_tool);

			world->addChild(springs[i]->line_s);
		}
	    

		monkey2 = new cMultiMesh();
		monkey2->loadFromFile("models/boulder_v4.obj");
		mesh2 = monkey2->getMesh(0);
		mesh2->m_material = MyMaterial::create();

		// create a colour texture map for this mesh object
		albedoMap = cTexture2d::create();
		albedoMap->loadFromFile("images/cliff2.jpg");
		albedoMap->setWrapModeS(GL_REPEAT);
		albedoMap->setWrapModeT(GL_REPEAT);

		// assign textures to the mesh
		mesh2->m_texture = albedoMap;
		mesh2->setUseTexture(true);
		monkey2->scale(0.01);
		monkey2->setStiffness(2000);
		monkey2->setFriction(1, 0.5);

		monkey2->createAABBCollisionDetector(0.001);
		world->addChild(monkey2);
		monkey2->setLocalPos(0.0, -0.015, 0.0);

		break;
	case 2:
		world->deleteAllChildren();
		world->addChild(light);
		world->addChild(light_2);
		world->addChild(camera);
		world->addChild(person->m_sphere);
		for (int i = 0; i < numHapticDevices; i++)
		{
			world->addChild(tool[i]->m_tool);

			world->addChild(springs[i]->line_s);
		}

		
		monkey = new cMultiMesh();
		monkey->loadFromFile("models/wall_v10.obj");
		mesh = monkey->getMesh(0);
		mesh->m_material = MyMaterial::create();

		// create a colour texture map for this mesh object
		albedoMap = cTexture2d::create();
		albedoMap->loadFromFile("images/cliff2.jpg");
		albedoMap->setWrapModeS(GL_REPEAT);
		albedoMap->setWrapModeT(GL_REPEAT);

		// assign textures to the mesh
		mesh->m_texture = albedoMap;
		mesh->setUseTexture(true);
		monkey->scale(0.01);
		monkey->setStiffness(2000);
		monkey->setFriction(1, 0.5);

		monkey->createAABBCollisionDetector(0.001);
		world->addChild(monkey);
		monkey->setLocalPos(0.0, -0.015, 0.0);
		break;
	case 3:

		break;
	default:
		break;

	}
}

void resetPosition()
{
	// reset the game states
	loseState = false;
	winState = false;

	// reset workspace centre
	workspaceCentre = cVector3d(0.0, 0.0, 0.01) + initClimberPos;
	for (int i = 0; i < numHapticDevices; i++)
	{
		tool[i]->resetBallPosAndForces();

	}
	person->resetPosAndForces();

	//reset camera angles
	cameraPos = initCameraPos;
	lookAtPos = initLookAtPos;

	// reset the timer
	timer.reset();
	timer.start();
}


//==============================================================================
/*
TEMPLATE:    application.cpp

Description of your application.
*/
//==============================================================================

int main(int argc, char* argv[])
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-------------------------------------------------------------" << endl;
	cout << "New Heights, The Rock Climbing Simulator (Powered by CHAI3D) " << endl;
	cout << "-------------------------------------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[c] - Toggle Camera View" << endl;
	cout << "[d] - Debugging Mode for the Devs" << endl;
	cout << "[r] - Ready up to start climbing!" << endl;
	cout << "[g] - Play a[g]ain!" << endl;
	cout << "[1] - Switch to easier cliff" << endl;
	cout << "[2] - Default/Medium but long level cliff" << endl;
	cout << "[q] - Exit application" << endl;
	cout << endl << endl;


	//--------------------------------------------------------------------------
	// OPENGL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLFW library
	if (!glfwInit())
	{
		cout << "failed initialization" << endl;
		cSleepMs(1000);
		return 1;
	}

	// set error callback
	glfwSetErrorCallback(errorCallback);

	// compute desired size of window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int w = 0.8 * mode->height;
	int h = 0.5 * mode->height;
	int x = 0.5 * (mode->width - w);
	int y = 0.5 * (mode->height - h);

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// set active stereo mode
	if (stereoMode == C_STEREO_ACTIVE)
	{
		glfwWindowHint(GLFW_STEREO, GL_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_STEREO, GL_FALSE);
	}

	// create display context
	window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// get width and height of window
	glfwGetWindowSize(window, &width, &height);

	// set position of window
	glfwSetWindowPos(window, x, y);

	// set key callback
	glfwSetKeyCallback(window, keyCallback);

	// set resize callback
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	// set current display context
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(swapInterval);

#ifdef GLEW_VERSION
	// initialize GLEW library
	if (glewInit() != GLEW_OK)
	{
		cout << "failed to initialize GLEW library" << endl;
		glfwTerminate();
		return 1;
	}
#endif


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	world->m_backgroundColor.setBlueLight();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	// position and orient the camera
	camera->set(initCameraPos,    // camera position (eye)
		cVector3d(0.0, 0.0, 0.0),    // look at position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

									 // set the near and far clipping planes of the camera
	camera->setClippingPlanes(0.01, 10.0);

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.01);
	camera->setStereoFocalLength(0.5);

	// set vertical mirrored display mode
	camera->setMirrorVertical(mirroredDisplay);

	// create a directional light source
	light = new cDirectionalLight(world);
	light_2 = new cDirectionalLight(world);
	// insert light source inside world
	world->addChild(light);
	world->addChild(light_2);
	// enable light source
	light->setEnabled(true);
	light_2->setEnabled(true);

	// define direction of light beam
	light->setDir(-0.5, -0.2, 0.0);
	light_2->setDir(-0.5, -0.2, 0.0);
	// create a sphere (cursor) to represent the haptic device

	// create a sphere (cursor) to represent the haptic device
	cursor = new cShapeSphere(0.01);

	// insert cursor inside world
	//world->addChild(cursor);

	//****************************************************************************************************************INITIALIZE STUFF
	//--------------------------------------------------------------------------
	// Game Texture Setup 
	//--------------------------------------------------------------------------

	monkey = new cMultiMesh();
	monkey->loadFromFile("models/wall_v10.obj");
	mesh = monkey->getMesh(0);
	mesh->m_material = MyMaterial::create();

	
	// create a colour texture map for this mesh object
	cTexture2dPtr albedoMap = cTexture2d::create();
	albedoMap->loadFromFile("images/cliff2.jpg");
	albedoMap->setWrapModeS(GL_REPEAT);
	albedoMap->setWrapModeT(GL_REPEAT);


	// assign textures to the mesh
	mesh->m_texture = albedoMap;
    mesh->setUseTexture(true);

	monkey->scale(0.01);
	monkey->setStiffness(2000);
	monkey->setFriction(1, 0.5);


	monkey->createAABBCollisionDetector(0.001);
	world->addChild(monkey);
	monkey->setLocalPos(0.0, -0.015, 0.0);

	//--------------------------------------------------------------------------
	// Game Logic Setup
	//--------------------------------------------------------------------------

	climberLives = MAX_NUM_OF_LIVES;

	//--------------------------------------------------------------------------
	// HAPTIC DEVICE
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();

	// get number of haptic devices
	numHapticDevices = handler->getNumDevices();

	//person = new cShapeSphere(0.001);
	//person = new MyBall(world);
	//person->setBallPos(cVector3d(0.01, 0, 0));
	person = new Person(0.001, initClimberPos);
	
	//uncomment to add the mass visually
	world->addChild(person->m_sphere);

	for (int i = 0; i < numHapticDevices; i++)
	{
		// get a handle to the first haptic device
		handler->getDevice(hapticDevice[i], i);

		// open a connection to haptic device
		hapticDevice[i]->open();

		// calibrate device (if necessary)
		hapticDevice[i]->calibrate();

		// retrieve information about the current haptic device
		cHapticDeviceInfo info = hapticDevice[i]->getSpecifications();

		//****************************************************************************************************************CHANGES FOR CURSOR

		tool[i] = new MyBall(world, initClimberPos);
		tool[i]->m_tool->setHapticDevice(hapticDevice[i]);
		tool[i]->m_tool->setRadius(0.001);
		tool[i]->m_tool->enableDynamicObjects(true);		//variant of god obj that is beter than standard. use it. ESPECIALLY FOR MOVING THINGS. THIS IS FOR MOVING THINGS
		tool[i]->m_tool->start();
		world->addChild(tool[i]->m_tool);

		springs[i] = new MySpring(tool[i], person);
		world->addChild(springs[i]->line_s);
		// if the device has a gripper, enable the gripper to simulate a user switch
		hapticDevice[i]->setEnableGripperUserSwitch(true);
	}



	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a font
	cFontPtr font = NEW_CFONTCALIBRI20();


	// create a label to display the haptic and graphic rates of the simulation
	labelRates = new cLabel(font);
	labelRates->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelRates);

	// create a label to display the results of the player either winning or losing
	labelGameResult = new cLabel(font);
	labelGameResult->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelGameResult);

	// a label to display game status
	labelGameStatus = new cLabel(font);
	labelGameStatus->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelGameStatus);

	// label for displaying debug variables
	labelDebugInfo = new cLabel(font);
	labelDebugInfo->m_fontColor.setWhite();
	camera->m_frontLayer->addChild(labelDebugInfo);

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);


	//--------------------------------------------------------------------------
	// MAIN GRAPHIC LOOP
	//--------------------------------------------------------------------------

	// call window size callback at initialization
	windowSizeCallback(window, width, height);

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		// get width and height of window
		glfwGetWindowSize(window, &width, &height);

		// render graphics
		updateGraphics();

		// swap buffers
		glfwSwapBuffers(window);

		// process events
		glfwPollEvents();

		// signal frequency counter
		freqCounterGraphics.signal(1);
	}

	// close window
	glfwDestroyWindow(window);

	// terminate GLFW library
	glfwTerminate();

	// exit
	return 0;
}

/*
chai3d::cVector3d getFallingCameraPos(chai3d::cVector3d cameraPos, )
{

}
*/
//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// update window size
	width = a_width;
	height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
	cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	// filter calls that only include a key press
	if (a_action != GLFW_PRESS)
	{
		return;
	}

	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	// option - toggle fullscreen
	else if (a_key == GLFW_KEY_F)
	{
		// toggle state variable
		fullscreen = !fullscreen;

		// get handle to monitor
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		// get information about monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// set fullscreen or window mode
		if (fullscreen)
		{
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
		else
		{
			int w = 0.8 * mode->height;
			int h = 0.5 * mode->height;
			int x = 0.5 * (mode->width - w);
			int y = 0.5 * (mode->height - h);
			glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
	}

	// option - toggle camera angle views
	else if (a_key == GLFW_KEY_C)
	{
		if (lookAtPos.equals(lookAtPos_1))
		{
			lookAtPos = lookAtPos_2;
		}
		else
		{
			lookAtPos = lookAtPos_1;
		}

		camera->set(cameraPos,    // camera position (eye)
			lookAtPos,    // look at position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
	}
	// option - toggle vertical mirroring
	else if (a_key == GLFW_KEY_M)
	{
		mirroredDisplay = !mirroredDisplay;
		camera->setMirrorVertical(mirroredDisplay);
	}

	// option - toggle debugging mode
	else if (a_key == GLFW_KEY_D)
	{
		debugMode = !debugMode;
	}

	// option - toggle ready up state
	else if (a_key == GLFW_KEY_R)
	{
		goodToClimb = !goodToClimb;
	}

	// option - toggle ready up state
	else if (a_key == GLFW_KEY_G)
	{
		if (resetGame == false)
			resetGame = true;

	}
}

//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	for (int i = 0; i < numHapticDevices; i++)
	{
		// close haptic device
		hapticDevice[i]->close();
	}

	// delete resources
	delete hapticsThread;
	delete world;
	delete handler;
}

//------------------------------------------------------------------------------
double calculateUserScore(double max_score, double acc)
{
	if(max_score - (acc) > 0)
		return max_score - (acc);
	return 0;
}
//-------------------------------------------------------------------------------
void updateGraphics(void)
{
	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update haptic and graphic rate data
	labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
		cStr(freqCounterHaptics.getFrequency(), 0) + " Hz" );

	// update position of label
	labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);

	std::string debugInfo;

	if (debugMode)
	{
		debugInfo += "isTouchingRock: " + cStr(isHolding);
		debugInfo += "\nNet Body Force: " + debugVector.str();
		debugInfo += "\nGood To Climb: " + cStr(goodToClimb);
		debugInfo += "\nWorkSpaceinZ: " + workspaceVectorInZ.str();
		debugInfo += "\nWorkSpaceVector: " + workspaceVector.str();
		debugInfo += "\nWorkSpaceCentre: " + workspaceCentre.str();
		debugInfo += "\noffset: " + offset.str();
		debugInfo += "\nDevicePos1: " + localDevicePos1.str();
		debugInfo += "\nDevicePos2: " + localDevicePos2.str();
		debugInfo += "\nCameraPos: " + cameraPos.str();
		debugInfo += "\nClimberPos: " + climberPos.str();
		debugInfo += "\nButtonCentre: " + cStr(buttonCentre);
		debugInfo += "\nButtonLeft: " + cStr(buttonLeft);
		debugInfo += "\nButtonTop: " + cStr(buttonTop);
		debugInfo += "\nButtonRight: " + cStr(buttonRight);
		debugInfo += "\nWinState: " + cStr(winState);
		debugInfo += "\nLoseState: " + cStr(loseState);
		debugInfo += "\nGameOver: " + cStr(gameOver);
		// update haptic and graphisc rate data
	}

	std::string gameResult;
	std::string gameStatus;

	if (loseState)
	{
		gameResult = "You died!";
		gameResult = loseMessage;
		labelGameResult->setText(gameResult);
		cSleepMs(1000);
		climberLives -= 1;
		if (climberLives == 0)
		{
			gameOver = true;
		}

		resetPosition();
	}
		
	else if (winState)
	{
		gameResult = winMessage;
		labelGameResult->setText(gameResult);
		cSleepMs(1000);
	}
	
	else if (gameOver)
	{
		gameResult = loseMessage;
		labelGameResult->setText(gameResult);
		cSleepMs(3000);
		climberLives = MAX_NUM_OF_LIVES;
		gameOver = false;
	}
	else
	{
		gameResult = "";
	}
	

	gameStatus = "Lives: " + cStr(climberLives) + " Score: " + cStr(userScore);

	labelGameResult->setText(gameResult);
	labelGameStatus->setText(gameStatus);

	labelGameStatus->setLocalPos((int)(width - width/4.0),(int) height - height/10.0);
	labelGameResult->setLocalPos((int)(width - width / 2.0), (int) height - height/2.0);

	labelDebugInfo->setText(debugInfo);
	// update position of label
	

	labelDebugInfo->setLocalPos((int)(0, 25));


	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps(false, mirroredDisplay);

	// render world
	camera->renderView(width, height);

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error:  %s\n" << gluErrorString(err);
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{


	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop	
	cameraPos = cVector3d(cameraPos.x(), person->pos_p.y(), person->pos_p.z());
	
	lookAtPos = cVector3d(lookAtPos.x(), person->pos_p.y(), person->pos_p.z());

	reset_Game();

	while (simulationRunning)
	{
		double t = timer.getCurrentTimeSeconds();
		lift.zero();
		
		// silly way of updating score based on updates every second
		if (hapticCounter == 1000)
		{
			// update score
			scoreAccum += 10*t;
			hapticCounter = 0;
		}

		
		for (int i = 0; i < numHapticDevices; i++)
		{
			springs[i]->getSpringForce();
		}
		
		climberPos = person->pos_p;
		
		if (climberPos.z() >= topCliff)
		{
			winState = true;
			loseState = false;
		}

		else if (climberPos.z() <= botCliff)
		{
			loseState = true;
			winState = false;
		}

		
		workspaceVector = cVector3d(person->pos_p.x() - workspaceCentre.x(), person->pos_p.y() - workspaceCentre.y(), person->pos_p.z() - workspaceCentre.z());
		updateToolPos = workspaceVector;
		for (int i = 0; i < numHapticDevices; i++)
		{

			
			//****************************************************************************MAGIC
			//need to do this when obj with parent child relationship
			world->computeGlobalPositions();
			//****************************************************************************MAGIC
			/////////////////////////////////////////////////////////////////////
			// READ HAPTIC DEVICE
			/////////////////////////////////////////////////////////////////////

			// read position 
			cVector3d position;
			hapticDevice[i]->getPosition(position);

			if (numHapticDevices == 2)
			{
				isHolding = tool[0]->m_tool->m_hapticPoint->isInContact(mesh) || tool[1]->m_tool->m_hapticPoint->isInContact(mesh);
			}
			else
			{
				isHolding = tool[i]->m_tool->m_hapticPoint->isInContact(mesh);
			}
			


			// not good news for climber, they start to descend fast as well as the workspace!
			if ((goodToClimb == true) && (isHolding == false))
			{
				workspaceVectorInZ = cVector3d(0.0, 0.0, 0.1);
				offset = -0.0008 * workspaceVectorInZ;
				tool[i]->m_tool->setLocalPos(tool[i]->m_tool->getLocalPos() + offset);
				//tool[i]->m_tool->setLocalPos(person->pos_p);
				person->pos_p += offset / numHapticDevices;
				cameraPos += offset / numHapticDevices;
				lookAtPos += offset / numHapticDevices;
				lookAtPos_1 += offset / numHapticDevices;
				lookAtPos_2 += offset / numHapticDevices;
				workspaceCentre += offset / numHapticDevices;
				camera->set(cameraPos,    // camera position (eye)
					lookAtPos,    // look at position (target)
					cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
				if (i == 0)
				{
					localDevicePos1 = tool[0]->m_tool->getGlobalPos();
				}
				else
				{

					localDevicePos2 = tool[1]->m_tool->getGlobalPos();
				}
			}
			else
			{				
				cVector3d offset = 0.0003 * workspaceVector;

				//workspaceCentre += offset;
				tool[i]->m_tool->setLocalPos(tool[i]->m_tool->getLocalPos() + offset);
				
				//cameraPos += offset / 2.0;
				//lookAtPos += offset / 2.0;
				//lookAtPos_1 += offset / 2.0;
				//lookAtPos_2 += offset / 2.0;
				//workspaceCentre += offset/2.0;

				cameraPos += offset / numHapticDevices;
				lookAtPos += offset / numHapticDevices;
				lookAtPos_1 += offset / numHapticDevices;
				lookAtPos_2 += offset / numHapticDevices;
				workspaceCentre += offset / numHapticDevices;
				camera->set(cameraPos,    // camera position (eye)
					lookAtPos,    // look at position (target)
					cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
			}
			// read orientation 
			cMatrix3d rotation;
			hapticDevice[i]->getRotation(rotation);

			// read user-switch status (button 0)
		
			hapticDevice[i]->getUserSwitch(0, buttonCentre);
			hapticDevice[i]->getUserSwitch(1, buttonLeft);
			hapticDevice[i]->getUserSwitch(2, buttonTop);
			hapticDevice[i]->getUserSwitch(3, buttonRight);

			/////////////////////////////////////////////////////////////////////
			// UPDATE 3D CURSOR MODEL
			/////////////////////////////////////////////////////////////////////
			//****************************************************************************MAGIC
			//// update position and orienation of cursor
			//cursor->setLocalPos(position);
			//cursor->setLocalRot(rotation);
			tool[i]->m_tool->updateFromDevice();
			
			//****************************************************************************MAGIC

		

			/////////////////////////////////////////////////////////////////////
			// COMPUTE FORCES
			/////////////////////////////////////////////////////////////////////
			//****************************************************************************MAGIC
			//cVector3d force(0, 0, 0);
			//cVector3d torque(0, 0, 0);
			//double gripperForce = 0.0;
			//tool[i]->m_tool->computeInteractionForces();
			//tool[i]->m_tool->setDeviceLocalForce(calcNetForces(tool[i]->m_tool->getDeviceLocalForce()));
			tool[i]->moveBall();
			debugVector = tool[i]->m_tool->getDeviceLocalForce();
			lift += tool[i]->force_p_device;
			//this gets the forces acting on the device
			//cout << tool[i]->getDeviceLocalForce() << endl;
			//****************************************************************************MAGIC

			/////////////////////////////////////////////////////////////////////
			// APPLY FORCES
			/////////////////////////////////////////////////////////////////////

			// send computed force, torque, and gripper force to haptic device
			//hapticDevice->setForceAndTorqueAndGripperForce(force, torque, gripperForce);


			// issues:
			// device anchored in space - what should be happening is your arm things movement should not impact the body! 
			// signal frequency counter
			freqCounterHaptics.signal(1);
			//****************************************************************************MAGIC
			tool[i]->m_tool->applyToDevice();
			springs[i]->setLine();
		}	//****************************************************************************MAGIC

		userScore = calculateUserScore(100000, scoreAccum);
		person->force_p_device = lift * 0.1;
		person->moveBall();
		
	/*	for (int i = 0; i < numHapticDevices; i++)
		{
			springs[i]->limitMovement();
		}*/

		//reset seems to not work properly...
		if (resetGame == true)
		{
			reset_Game();
		}
		hapticCounter++;
	}
	// exit haptics thread
	simulationFinished = true;

}
void reset_Game()
{
	userScore = 10000;
	resetGame = false;
	climberLives = MAX_NUM_OF_LIVES;

	scoreAccum = 0;
	// reset the game states
	loseState = false;
	winState = false;

	// reset workspace centre
	workspaceCentre = cVector3d(0.0, 0.0, 0.01) + initClimberPos;
	for (int i = 0; i < numHapticDevices; i++)
	{
		tool[i]->resetBallPosAndForces();

	}
	person->resetPosAndForces();

	//reset camera angles
	cameraPos = initCameraPos;
	lookAtPos = initLookAtPos;

	// reset the timer
	timer.reset();
	timer.start();
}

//------------------------------------------------------------------------------
