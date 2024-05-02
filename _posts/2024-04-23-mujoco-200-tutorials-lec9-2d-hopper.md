---
layout: post
title: 'mujoco-200-tutorials-Lec9: 2D Hopper'
date: 2024-04-23 16:40 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)

[docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)

2-D hopper에 대해 fsm으로 제어해보자.\
1. knee의 position control\
2. torso의 height control

### 구조

![hopper_structure](/assets/Mujoco-200-tutorials/lec9/hopper_structure.png){: width="500"}

### FSM state transition

![fsm_transition](/assets/Mujoco-200-tutorials/lec9/fsm_transition.png){: width="500"}

### Full code

[test.cc](/assets/Mujoco-200-tutorials/lec9/test.cc)
[test.xml](/assets/Mujoco-200-tutorials/lec9/test.xml)

### knee의 position control

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here

	// get data for transition
	int body_no;
	body_no = 3;
	// // xpos는 body frame position, qpos는 joint등의 position
	// printf("%f %f %f \n", d->xpos[3*body_no+0], d->xpos[3*body_no+1], d->xpos[3*body_no+2]);
	// printf("z velocity of torso: %f \n", d->qvel[1]);
	// printf("===============\n");

	// all transition
	double z_foot = d->xpos[3*body_no+2];
	double vz_torso = d->qvel[1];
	if(fsm == fsm_air1 && z_foot < 0.05) {
		fsm = fsm_stance1;
		// printf("fsm_stance1\n");
	}
	else if(fsm == fsm_stance1 && vz_torso > 0) {
		fsm = fsm_stance2;
		// printf("fsm_stance2\n");
	}
	else if(fsm == fsm_stance2 && z_foot > 0.05) {
		fsm = fsm_air2;
		// printf("fsm_air2\n"); 
	}
	else if(fsm == fsm_air2 && vz_torso < 0) {
		fsm = fsm_air1;
		// printf("fsm_air1\n");
		step_no++;
		printf("step no = %d \n", step_no);
	}
	// all actions
	int actuator_no;
	if(fsm == fsm_air1) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,1000);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,0);
	}
	else if(fsm == fsm_stance1) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,1000);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,0);
	}
	else if(fsm == fsm_stance2) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,1000);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,0);
	}
	else if(fsm == fsm_air2) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,100);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,10);
	}
	
  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}
```

### torso의 height control

fsm_stance2에서 hip을 d->ctrl[0] = -0.2로 살짝돌렸다가,\
fsm_stance1에서 hip을 d->ctrl[0] = 0으로 원래대로 돌린다.\
그럼 총 에너지는 그대로이지만, 앞으로 전진하기위해 에너지를 사용하므로 높이는 점점 감소하는 것을 볼 수 있다.(step no = 28에서 정지)

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here

	// get data for transition
	int body_no;
	body_no = 3;
	// // xpos는 body frame position, qpos는 joint등의 position
	// printf("%f %f %f \n", d->xpos[3*body_no+0], d->xpos[3*body_no+1], d->xpos[3*body_no+2]);
	// printf("z velocity of torso: %f \n", d->qvel[1]);
	// printf("===============\n");

	// all transition
	double z_foot = d->xpos[3*body_no+2];
	double vz_torso = d->qvel[1];
	if(fsm == fsm_air1 && z_foot < 0.05) {
		fsm = fsm_stance1;
		// printf("fsm_stance1\n");
	}
	else if(fsm == fsm_stance1 && vz_torso > 0) {
		fsm = fsm_stance2;
		// printf("fsm_stance2\n");
	}
	else if(fsm == fsm_stance2 && z_foot > 0.05) {
		fsm = fsm_air2;
		// printf("fsm_air2\n"); 
	}
	else if(fsm == fsm_air2 && vz_torso < 0) {
		fsm = fsm_air1;
		// printf("fsm_air1\n");
		step_no++;
		printf("step no = %d \n", step_no);
	}
	// all actions
	int actuator_no;
	if(fsm == fsm_air1) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,100);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,10);
	}
	else if(fsm == fsm_stance1) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,1000);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,0);
	}
	else if(fsm == fsm_stance2) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,1000);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,0);
		d->ctrl[0] = -0.2;
	}
	else if(fsm == fsm_air2) {
		actuator_no = 2; //pservo-knee
		set_position_servo(m,actuator_no,100);
		actuator_no = 3;  //vservo-knee
		set_velocity_servo(m,actuator_no,10);
		d->ctrl[0] = 0;
	}
	
  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```

xml에서 light가 torso를 비추게한다.

```
<visual>
		<headlight ambient=".3 .3 .3"/>
  </visual>
  <worldbody>
	<!-- <light diffuse=".5 .5 .5" pos="0 0 3" dir="0 0 -1"/> -->
	<!-- <light mode="targetbody" target="torso"/> -->

```
