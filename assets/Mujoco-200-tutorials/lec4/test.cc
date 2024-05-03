#include<stdbool.h> //for bool
#include "mujoco/mujoco.h"
#include <GLFW/glfw3.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctime"
#include "random"

// simulation end time
double simend = 20;

//related to writing data to a file
FILE *fid;
int loop_index = 0;
const int data_chk_period = 50; //frequency at which data is written to a file

//Change the path 
char path[] = "./";
char datafile[] = "data.csv";
char xmlfile[] = "test.xml";

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

// ****************************
//This function is called once and is used to get the headers
void init_save_data()
{
  //write name of the variable here (conventionally, "t," is header of time series data for csv file.)
   fprintf(fid,"t, ");
   fprintf(fid,"PE, KE, TE,");
   fprintf(fid,"q1, q2,");

   //Don't remove the newline
   fprintf(fid,"\n");
}

//***************************
//This function is called at a set frequency, put data here
void save_data(const mjModel* m, mjData* d)
{
  //data here should correspond to headers in init_save_data()
  //seperate data by a space %f followed by space
  fprintf(fid,"%f, ",d->time);
  fprintf(fid,"%f, %f, %f,",d->energy[0],d->energy[1], d->energy[0]+d->energy[1]);
  fprintf(fid,"%f, %f,",d->qpos[0],d->qpos[1]);

  //Don't remove the newline
  fprintf(fid,"\n");
}

//**************************
void my_controller(const mjModel* m, mjData* d)
{
	// Evaluate energy and store to data (need to use flag/energy)
	mj_energyPos(m,d);
	mj_energyVel(m,d);
	// printf("%f %f %f %f\n", d->time, d->energy[0], d->energy[1], d->energy[0]+d->energy[1]);
	
	// Check dynamics of motion
	// M*qdotdot + qfrc_bias = qfrc_applied + ctrl
	const int nv = 2;
	double inertial_M[nv*nv] = {};
	// matrix는 계산상 효율을 통해 sparse matrix로 변환되어 계산된다.(ex. d->qM)
	// convert sparse inertia rank 2 tensor d->qM into full matrix(4 x 1) inertial_M.
	mj_fullM(m, inertial_M, d->qM);
	// 보기 편한 2 x 2 형태로 변환
	double M[nv][nv] = {0};
	M[0][0] = inertial_M[0];
	M[0][1] = inertial_M[1];
	M[1][0] = inertial_M[2];
	M[1][1] = inertial_M[3];
	// printf("%f %f\n", M[0][0], M[0][1]);
	// printf("%f %f\n", M[1][0], M[1][1]);
	// printf("******************\n");
	double qacc[nv] = {0};
	qacc[0] = d->qacc[0];
	qacc[1] = d->qacc[1];

	double qfrc_bias[nv] = {0};
	qfrc_bias[0] = d->qfrc_bias[0];
	qfrc_bias[1] = d->qfrc_bias[1];
	
	// mju_mulMatVec(mjtNum* res, const mjtNum* mat, const mjtNum* vec, int nr, int nc);
	// double n_torque[nv] = {0};
	// mju_mulMatVec(n_torque, inertial_M, qacc, 2, 2);
	// double lhs[nv] = {0};
	// for(int i=0; i<nv; i++) {
	// 	lhs[i] = n_torque[i] + qfrc_bias[i];
	// }
	// compensation with weight factor(.1, .5) for qfrc_bias(coriolis force, gravity force, etc.)
	// d->qfrc_applied[0] = 0.1*qfrc_bias[0];
	// d->qfrc_applied[1] = 0.5*qfrc_bias[1];
	// double rhs[nv] = {0};
	// for(int i=0; i<nv; i++) {
	// 	rhs[i] = d->qfrc_applied[i];
	// }
	// printf("%f %f\n", lhs[0], rhs[0]);
	// printf("%f %f\n", lhs[1], rhs[1]);
	// printf("******************\n");

	// contol
	double kp1=200, kp2=100;
	double kd1=10, kd2=10;
	double qref1=-0.5, qref2=1.5;

	// PD contorl
	// $$\tau = K_pq_{error} + K_d\dot{q_{error}}$$
	// d->qfrc_applied[0] = kp1*(qref1 - d->qpos[0]) + kd1*(-d->qvel[0]);
	// d->qfrc_applied[1] = kp2*(qref2 - d->qpos[1]) + kd2*(-d->qvel[1]);

	// (gravity+coriolis forces)+PD control
	// $$\tau = $$ qfrc_bias + $$K_pq_{error} + K_d\dot{q_{error}}$$
	// d->qfrc_applied[0] =  d->qfrc_bias[0] + kp1*(qref1 - d->qpos[0]) + kd1*(-d->qvel[0]);
	// d->qfrc_applied[1] =  d->qfrc_bias[1] + kp2*(qref2 - d->qpos[1]) + kd2*(-d->qvel[1]);

	// PD feedback linearization control
	// $$\tau = M(K_pq_{error} + K_d\dot{q_{error}})$$ + qfrc_bias
	double n_torque[nv] = {0};
	double pd_feedback[nv] = {0};
	pd_feedback[0] = kp1*(qref1 - d->qpos[0]) + kd1*(-d->qvel[0]);
	pd_feedback[1] = kp2*(qref2 - d->qpos[1]) + kd2*(-d->qvel[1]);
	mju_mulMatVec(n_torque, inertial_M, pd_feedback, 2, 2);

	d->qfrc_applied[0] =  n_torque[0] + d->qfrc_bias[0] ;
	d->qfrc_applied[1] =  n_torque[1] + d->qfrc_bias[1];


	//write data here (dont change/dete this function call; instead write what you need to save in save_data)
	if (loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

// main function
int main(int argc, const char** argv)
{
    char xmlpath[100]={};
    char datapath[100]={};

    strcat(xmlpath,path);
    strcat(xmlpath,xmlfile);

    strcat(datapath,path);
    strcat(datapath,datafile);

    // load and compile model
    char error[1000] = "Could not load binary model";

    // check command-line arguments
    if( argc<2 )
        m = mj_loadXML(xmlpath, 0, error, 1000);

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

    double arr_view[] = {90, -5, 7, 0.000000, 0.000000, 2.000000};
    cam.azimuth = arr_view[0];
    cam.elevation = arr_view[1];
    cam.distance = arr_view[2];
    cam.lookat[0] = arr_view[3];
    cam.lookat[1] = arr_view[4];
    cam.lookat[2] = arr_view[5];

	mjcb_control = my_controller;

	fid = fopen(datapath,"w");
    init_save_data();
	d->qpos[0] = 0.6;

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
		if (d->time>=simend)
		{
			fclose(fid);
			break;
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
