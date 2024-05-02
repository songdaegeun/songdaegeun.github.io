---
layout: post
title: 'mujoco-200-tutorials-Lec5: Finite State Machine and Trajectory Tracking for
  Double Pendulum'
date: 2024-04-22 19:39 +0900
description:
author: rosi
categories: [mujoco]
tags:
math: true
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)

[docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)

what is Finite State Machine(FSM)?

how to create trajectory?

how to follow trajectory?


### Full code

[test.cc](/assets/Mujoco-200-tutorials/lec5/test.cc)
[test.xml](/assets/Mujoco-200-tutorials/lec5/test.xml)
[solve_polynomial.m](/assets/Mujoco-200-tutorials/lec5/solve_polynomial.m)
[time_position_plot.m](/assets/Mujoco-200-tutorials/lec5/time_position_plot.m)

### FSM

FSM은 유한한 개수의 상태를 가질 수 있는 오토마타이다. 
이번 경우, state가 변할 때 특정 point에서 다른 point로 제어목표를 옮겨가는 로직이므로, set point control이라고 한다.

```
void mycontroller(const mjModel* m, mjData* d)
{
  //write control here

  // transition
  if(fsm_state == fsm_hold && d->time >= t_hold) {
	fsm_state = fsm_swing1;
  }
  if(fsm_state == fsm_swing1 && d->time >= t_hold+t_swing1) {
	fsm_state = fsm_swing2;
  }
  if(fsm_state == fsm_swing2 && d->time >= t_hold+t_swing1+t_swing2) {
	fsm_state = fsm_stop;
  }
  
  // action
  // start: q0 = -1; q1 = 0
  // intermediate: q0 = 0; q1 = -1.57(-pi/2)
  // end: q0 = 1; q1 = 0
  double q0, q1;
  if(fsm_state == fsm_hold) {
	q0 = -1; q1 = 0;
	d->ctrl[1] = q0;
	d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_swing1) {
	q0 = 0; q1 = -1.57;
	d->ctrl[1] = q0;
	d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_swing2) {
	q0 = 1; q1 = 0;
	d->ctrl[1] = q0;
	d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_stop) {
	q0 = 1; q1 = 0;
	d->ctrl[1] = q0;
	d->ctrl[4] = q1;
  }

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```

### trajectory following control 

boundary condition으로 초기, 종료 시 position, velocity, acceleration을 만족하도록,
trajectory를 5차 polynomial로 정하는 것이 무난하다.

$$ q(t) = a_0 + a_1t + a_2t^2 + a_3t^3 + a_4t^4 + a_5t^5 $$

하지만 이번 경우, B.C.는 hold, stop의 position, velocity만 존재하므로 3차 polynomial을 사용한다.

PD controller는 desired position과 desired velocity의 error를 동시에 수렴시킬 수 있다. 

![position_behavior.png](/assets/Mujoco-200-tutorials/lec5/position_behavior.png){: width="350" height="350"}

```
void mycontroller(const mjModel* m, mjData* d)
{
  //write control here
  double t = d->time;
  // transition
  if(fsm_state == fsm_hold && d->time >= t_hold) {
	fsm_state = fsm_swing1;
	double q_0[2] = {0}, q_f[2] = {0};
	q_0[0] = -1; q_0[1] = 0; 
	q_f[0] = 0.5; q_f[1] = -2; 
	generate_trajectory(t_hold, t_hold+t_swing1, q_0, q_f);
  }
  if(fsm_state == fsm_swing1 && d->time >= t_hold+t_swing1) {
	fsm_state = fsm_swing2;
	double q_0[2] = {0}, q_f[2] = {0};
	q_0[0] = 0.5; q_0[1] = -2; 
	q_f[0] = 1; q_f[1] = 0; 
	generate_trajectory(t_hold+t_swing1, t_hold+t_swing1+t_swing2, q_0, q_f);
  }
  if(fsm_state == fsm_swing2 && d->time >= t_hold+t_swing1+t_swing2) {
	fsm_state = fsm_stop;
  }
  
  // action
  // start: q0 = -1; q1 = 0q
  // intermediate: q0 = 0; q1 = -1.57(-pi/2)
  // end: q0 = 1; q1 = 0
  double q0, q1;
  double kp=500, kv=50;
  
  if(fsm_state == fsm_hold) {
	// q0 = -1; q1 = 0;
	qref[0]=-1; qref[1]=0;
	uref[0]=0; uref[1]=0;

	d->ctrl[0] = kp*(qref[0] - d->qpos[0]) + kv*(uref[0]-d->qvel[0]);
	d->ctrl[3] = kp*(qref[1] - d->qpos[1]) + kv*(uref[1]-d->qvel[1]);
	// d->ctrl[1] = q0;
	// d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_swing1) {
	// q0 = 0; q1 = -1.57;
	for(int i=0; i<2; i++) {
		qref[i] = a0[i] + a1[i]*t + a2[i]*t*t + a3[i]*t*t*t;
		uref[i] =  a1[i] + 2*a2[i]*t + 3*a3[i]*t*t;
	}
	
	d->ctrl[0] = kp*(qref[0] - d->qpos[0]) + kv*(uref[0] -d->qvel[0]);
	d->ctrl[3] = kp*(qref[1] - d->qpos[1]) + kv*(uref[1] -d->qvel[1]);
	// d->ctrl[1] = q0;
	// d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_swing2) {
	// q0 = 1; q1 = 0;
	for(int i=0; i<2; i++) {
		qref[i] = a0[i] + a1[i]*t + a2[i]*t*t + a3[i]*t*t*t;
		uref[i] =  a1[i] + 2*a2[i]*t + 3*a3[i]*t*t;
	}
	
	d->ctrl[0] = kp*(qref[0] - d->qpos[0]) + kv*(uref[0] -d->qvel[0]);
	d->ctrl[3] = kp*(qref[1] - d->qpos[1]) + kv*(uref[1] -d->qvel[1]);
	// d->ctrl[1] = q0;
	// d->ctrl[4] = q1;
  }
  else if(fsm_state == fsm_stop) {
	// q0 = 1; q1 = 0;
	qref[0]=1; qref[1]=0;
	uref[0]=0; uref[1]=0;

	d->ctrl[0] = kp*(qref[0] - d->qpos[0]) + kv*(uref[0]-d->qvel[0]);
	d->ctrl[3] = kp*(qref[1] - d->qpos[1]) + kv*(uref[1]-d->qvel[1]);
	// d->ctrl[1] = q0;
	// d->ctrl[4] = q1;
  }

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```


