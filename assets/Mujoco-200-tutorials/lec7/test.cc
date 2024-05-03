#include<stdbool.h> //for bool
#include "mujoco/mujoco.h"
#include <GLFW/glfw3.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctime"
#include "random"

// simulation end time
double simend = 10;
int lqr = 0;

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

  //Don't remove the newline
  fprintf(fid,"\n");
}

/******************************/
void set_torque_control(const mjModel* m,int actuator_no,int flag)
{
  if (flag==0)
    m->actuator_gainprm[10*actuator_no+0]=0;
  else
    m->actuator_gainprm[10*actuator_no+0]=1;
}
/******************************/


/******************************/
void set_position_servo(const mjModel* m,int actuator_no,double kp)
{
  m->actuator_gainprm[10*actuator_no+0]=kp;
  m->actuator_biasprm[10*actuator_no+1]=-kp;
}
/******************************/

/******************************/
void set_velocity_servo(const mjModel* m,int actuator_no,double kv)
{
  m->actuator_gainprm[10*actuator_no+0]=kv;
  m->actuator_biasprm[10*actuator_no+2]=-kv;
}
/******************************/

void f(const mjModel* m, mjData* d, double input[5], double output[4]) {
	// state = [q1, q1dot, q2, q2dot]
	// input = [q1, q1dot, q2, q2dot u]
	// output = [q1dot, q1ddot, q2dot, q2ddot]
	d->qpos[0] = input[0];
	d->qvel[0] = input[1];
	d->qpos[1] = input[2];
	d->qvel[1] = input[3];
	d->ctrl[0] = input[4];
	mj_forward(m,d);
	double q1dot, q2dot;
	q1dot = d->qvel[0];
	q2dot = d->qvel[1];
	// dynamics 
	// Mqacc + qfrc_bias = ctrl
	const int nv = 2;
	double M[nv*nv] = {0};
	// void mj_fullM(const mjModel* m, mjtNum* dst, const mjtNum* M);
	mj_fullM(m,M,d->qM);
	double det_M = M[0]*M[3] - M[1]*M[2];
	double inv_M[nv*nv] = {M[3],-M[1],-M[2],M[0]}; 
	for(int i=0; i<4; i++) {
		inv_M[i] /= det_M;
	}
	// f = ctrl - qfrc_bias
	double f[2] = {0};
	f[0] = (0 - d->qfrc_bias[0]); // joint1 is no control
	f[1] = (d->ctrl[0] - d->qfrc_bias[1]); 
	// qacc = inv_M(ctrl - qfrc_bias)
	double qacc[2] = {0};
	// void mju_mulMatVec(mjtNum* res, const mjtNum* mat, const mjtNum* vec, int nr, int nc);
	mju_mulMatVec(qacc, inv_M, f, 2, 2);
	double q1ddot, q2ddot;
	q1ddot = qacc[0];
	q2ddot = qacc[1];
	output[0] = q1dot;
	output[1] = q1ddot;
	output[2] = q2dot;
	output[3] = q2ddot;
}

//**************************
void init_controller(const mjModel* m, mjData* d)
{
	double input[5] = {0};
	double output[4] = {0};
	double perturb = 0.001;

	double f0[4] = {0};
	f(m,d,input,output);
	for(int i=0; i<4; i++){
		f0[i] = output[i];
	}

	double A[4][4] = {0};
	
	for(int j=0; j<4; j++) {
		for(int i=0; i<5; i++){
			input[i] = 0;
		}
		input[j] = perturb;
		f(m,d,input,output);
		
		for(int i=0; i<4; i++){
			A[i][j] = (output[i]-f0[i])/perturb;
		}
	}
	// printf("A = [...\n");
	// for(int i=0; i<4; i++) {
	// 	for(int j=0; j<4; j++) {
	// 		printf("%f ", A[i][j]);
	// 	}
	// 	printf(";\n");
	// }
	// printf("];\n\n");

	double B[4] = {0};
	for(int i=0; i<5; i++){
		input[i] = 0;
	}
	input[4] = perturb;
	f(m,d,input,output);
	for(int i=0; i<4; i++){
		B[i] = (output[i]-f0[i])/perturb;
	}
	// printf("B = [...\n");
	// for(int i=0; i<4; i++) {
	// 	printf("%f ", B[i]);
	// 	printf(";\n");
	// }
	// printf("];\n\n");
}

//**************************
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here
	if(lqr)
	{
		double K[4] = {-3.9288,   -1.4486,   -1.0113,   -0.4192};
		d->ctrl[0] = -K[0]*1000*d->qpos[0]-K[1]*1000*d->qvel[0]-K[2]*1000*d->qpos[1]-K[3]*1000*d->qvel[1];

		double noise;
		// mjtNum mju_standardNormal(mjtNum* num2);
		mju_standardNormal(&noise);
		int body = 2;
		d->xfrc_applied[6*body+0]=10*noise;
		d->qfrc_applied[0] = noise;
		d->qfrc_applied[1] = noise;
	}

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
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

    double arr_view[] = {90, -5, 5, 0.000000, 0.000000, 1.000000};
    cam.azimuth = arr_view[0];
    cam.elevation = arr_view[1];
    cam.distance = arr_view[2];
    cam.lookat[0] = arr_view[3];
    cam.lookat[1] = arr_view[4];
    cam.lookat[2] = arr_view[5];

	mjcb_control = mycontroller;

	fid = fopen(datapath,"w");
    init_save_data();
	init_controller(m,d);
	lqr = 1;
	// d->qpos[0] = 0.02;

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
