#include<stdbool.h> //for bool
#include "mujoco/mujoco.h"
#include <GLFW/glfw3.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctime"
#include "random"

char filename[] = "test.xml";

// MuJoCo data structures
mjModel* m = NULL;                  // MuJoCo model
mjData* d = NULL;                   // MuJoCo data
mjvCamera cam;                      // abstract camera
mjvOption opt;                      // visualization options
mjvScene scn;                       // abstract scene
mjrContext con;                     // custom GPU context

// mouse interaction
bool button_left = false;
bool button_middle = false;
bool button_right =  false;
double lastx = 0;
double lasty = 0;

// holders of one step history of time and position to calculate dertivatives
mjtNum position_history = 0;
mjtNum previous_time = 0;

// controller related variables
float_t ctrl_update_freq = 100;
mjtNum last_update = 0.0;
mjtNum ctrl;

// keyboard callback
void keyboard(GLFWwindow* window, int key, int scancode, int act, int mods)
{
    // backspace: reset simulation
    if( act==GLFW_PRESS && key==GLFW_KEY_BACKSPACE )
    {
        mj_resetData(m, d);
        mj_forward(m, d);
    }
}

// mouse button callback
void mouse_button(GLFWwindow* window, int button, int act, int mods)
{
    // update button state
    button_left =   (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS);
    button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS);
    button_right =  (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS);

    // update mouse position
    glfwGetCursorPos(window, &lastx, &lasty);
}


// mouse move callback
void mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    // no buttons down: nothing to do
    if( !button_left && !button_middle && !button_right )
        return;

    // compute mouse displacement, save
    double dx = xpos - lastx;
    double dy = ypos - lasty;
    lastx = xpos;
    lasty = ypos;

    // get current window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // get shift key state
    bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)==GLFW_PRESS);

    // determine action based on mouse button
    mjtMouse action;
    if( button_right )
        action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
    else if( button_left )
        action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
    else
        action = mjMOUSE_ZOOM;

    // move camera
    mjv_moveCamera(m, action, dx/width, dy/height, &scn, &cam);
}

// scroll callback
void scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    // emulate vertical mouse motion = 5% of window height
    mjv_moveCamera(m, mjMOUSE_ZOOM, 0, -0.05*yoffset, &scn, &cam);
}

void set_torque(const mjModel*m, int flag) {
	int actuator_no = 0;
	m->actuator_gainprm[mjNGAIN*actuator_no+0] = flag;
}
void set_kp(const mjModel*m, double kp) {
	int actuator_no = 1;
	m->actuator_gainprm[mjNGAIN*actuator_no+0] = kp;
	m->actuator_biasprm[mjNGAIN*actuator_no+1] = -kp;
}

void set_kv(const mjModel*m, double kv) {
	int actuator_no = 2;
	m->actuator_gainprm[mjNGAIN*actuator_no+0] = kv;
	m->actuator_biasprm[mjNGAIN*actuator_no+2] = -kv;
}

void my_controller(const mjModel* m, mjData* d)
{
	// make noise
	unsigned int seed = static_cast<unsigned int>(time(NULL));
	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution(0.0, .01);
	double noise = distribution(generator);
	double n_s0 = d->sensordata[0]+noise;
	double n_s1 = d->sensordata[1]+noise;

	// 0: torque actuator (applied to input torque in dynamics for this simulation)
	set_torque(m, 1);
	// d->ctrl[0] = 10*(0-d->qpos[0]) + 1*(0-d->qvel[0]); // PD position control; kp=5, kd=1
	d->ctrl[0] = 10*(0-n_s0) + 1*(0-n_s1); // PD position control; kp=5, kd=1
	// 1: position servo (position의 setpoint)
	// double kp = 1000;
	// set_kp(m, kp);
	// double kv = 1;
	// set_kv(m, kv);

	// // actuator_no(1 or 2)의 gainprm, biasprm에 대해, mjNGAIN를 순회하며 출력하여 xml파일에서 kp설정시 몇번째에서 변하는지 확인.
	// int actuator_no = 2;
	// for(int i=0; i<mjNGAIN;i++) {
	// 	// printf("%f\n", m->actuator_gainprm[mjNGAIN*actuator_no+i]);
	// 	printf("%f\n", m->actuator_biasprm[mjNGAIN*actuator_no+i]);
	// }
	// printf("**********\n");

	// d->ctrl[1] = -3.14/4;
	// 2: velocity servo (velocity의 setpoint. error dynamics를 풀어보면 steady state error=0)
	// d->ctrl[2] = 10;
}

// main function
int main(int argc, const char** argv)
{
    // load and compile model
    char error[1000] = "Could not load binary model";

    // check command-line arguments
    if( argc<2 )
        m = mj_loadXML(filename, 0, error, 1000);

    else
        if( strlen(argv[1])>4 && !strcmp(argv[1]+strlen(argv[1])-4, ".mjb") )
            m = mj_loadModel(argv[1], 0);
        else
            m = mj_loadXML(argv[1], 0, error, 1000);
    if( !m )
        mju_error_s("Load model error: %s", error);

    // make data
    d = mj_makeData(m);

    // init GLFW
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create window, make OpenGL context current, request v-sync
    GLFWwindow* window = glfwCreateWindow(1244, 700, "Demo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // initialize visualization data structures
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjv_defaultScene(&scn);
    mjr_defaultContext(&con);
    mjv_makeScene(m, &scn, 2000);                // space for 2000 objects
    mjr_makeContext(m, &con, mjFONTSCALE_150);   // model-specific context

    // install GLFW mouse and keyboard callbacks
    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_button);
    glfwSetScrollCallback(window, scroll);

    double arr_view[] = {90, -5, 5, 0.000000, 0.000000, 1.000000};
    cam.azimuth = arr_view[0];
    cam.elevation = arr_view[1];
    cam.distance = arr_view[2];
    cam.lookat[0] = arr_view[3];
    cam.lookat[1] = arr_view[4];
    cam.lookat[2] = arr_view[5];

	// position of joint(in this example, pin)
	d->qpos[0]=1.57; // pi/2
	mjcb_control = my_controller;

    // use the first while condition if you want to simulate for a period.
    while( !glfwWindowShouldClose(window))
    {
        // advance interactive simulation for 1/60 sec
        // Assuming MuJoCo can simulate faster than real-time, which it usually can,
        // this loop will finish on time for the next frame to be rendered at 60 fps.
        // Otherwise add a cpu timer and exit this loop when it is time to render.
        mjtNum simstart = d->time;
        while(d->time - simstart < 1.0/60.0)
        {
            mj_step(m, d);
        }

       // get framebuffer viewport
        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

		// update scene and render
        mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
        mjr_render(viewport, &scn, &con);
        // printf("{%f, %f, %f, %f, %f, %f};\n",cam.azimuth,cam.elevation, cam.distance,cam.lookat[0],cam.lookat[1],cam.lookat[2]);

        // swap OpenGL buffers (blocking call due to v-sync)
        glfwSwapBuffers(window);

        // process pending GUI events, call GLFW callbacks
        glfwPollEvents();
    }

    // free visualization storage
    mjv_freeScene(&scn);
    mjr_freeContext(&con);

    // free MuJoCo model and data, deactivate
    mj_deleteData(d);
    mj_deleteModel(m);

    // terminate GLFW (crashes with Linux NVidia drivers)
    #if defined(__APPLE__) || defined(_WIN32)
        glfwTerminate();
    #endif

    return 1;
}
