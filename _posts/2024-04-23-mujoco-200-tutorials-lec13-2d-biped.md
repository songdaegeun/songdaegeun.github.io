---
layout: post
title: 'mujoco-200-tutorials-Lec13: 2D Biped'
date: 2024-04-23 16:41 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)\
[mujoco docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)\
[NLopt docs](https://nlopt.readthedocs.io/en/latest/NLopt_Reference/)

slope를 내려가는 bipedal motion을 만들어보자!\
(hip joint의 gain을 tuning하지않으면 drunken gait이다. chaotic solution을 만족하는 gain을 tuning하면 대칭적인 gait를 얻을 수 있다.)

> drunken gait를 유발하는 gain: kp="10", kv="1" \
> chaotic solution에 가까운 gain : kp="4", kv="1" \
> drunken gait: 양발의 보폭이 다른 보행. \
> [혼돈(Chaos)](https://en.wikipedia.org/wiki/Chaos_theory): 현재가 미래를 결정하지만, 대략적인 현재가 미래를 대략적으로 결정하지 못하는 경우.


## Full code

[test.c](/assets/Mujoco-200-tutorials/lec13/test.c)\
[utility.c](/assets/Mujoco-200-tutorials/lec13/utility.c)\
[test.xml](/assets/Mujoco-200-tutorials/lec13/test.xml)

## biped model and DOF

![dof_model](/assets/Mujoco-200-tutorials/lec13/dof_model.png){:width="400"}\
q1, q2는 world frame에 대한 angle이며, quaternion으로 표현되기때문에 물리적으로 인식하기쉽게 코딩할 때 euler angle로 바꾼다.

### fakey slope 구현 

world좌표계기준으로 실제로 경사진 plane을 만들면 좌표계산이 어려워진다.\
plane과 중력의 관계를 생각해보면, plane대신 중력을 기울여도 동일한 motion을 만들어낼 수 있다.\
따라서 중력을 다음과 같이 설정한다.

```
int main() {
	double gamma = 0.1; // ramp slope
	double gravity = 9.81;
	m->opt.gravity[0] = gravity*sin(gamma);
	m->opt.gravity[2] = -gravity*cos(gamma);
}
```

### FSM for bipedal motion for z_foot (swing)

![swing](/assets/Mujoco-200-tutorials/lec13/swing.png){:width="400"}\
bipedal motion의 phase를 생각해보면,\
한쪽 발이 지면에 닿는 순간, 구동력이 제공되고(swing의 원인. leg2에 대해 작용하고, leg1에 대해 반작용),\
전체 body의 무게중심은 중력방향이므로 평면기준으로 전진한다.\
전진 중, 나머지 발은 끌리게된다(trail + swing).

**trail + swing이기때문에, 여기까지 구현한건 완성된게 아니다.**(아래 영상 참고) \
발이 끌리게 된다면, swing을 하더라도 앞으로 나아갈 수 없다.\
발이 끌리지 않게 하려면 knee에 대한 fsm까지 구현해야한다 (다음 절에서 구현).

![swing_only](/assets/Mujoco-200-tutorials/lec13/swing_only.gif){:width="400"}\

```

void mycontroller(const mjModel* m, mjData* d)
{
	// write control here

	int body_no;
	double quat0, quatx, quaty, quatz;
	double euler_x, euler_y, euler_z;
	double abs_leg1, abs_leg2;

	double q1 = d->qpos[0]; double u1 = d->qvel[0]; 
	double x = d->qpos[1]; double vx = d->qvel[1]; 
	double z = d->qpos[2]; double vz = d->qvel[2]; 
	double l1 = d->qpos[3]; double l1dot = d->qvel[3]; 
	double q2 = d->qpos[4]; double u2 = d->qvel[4]; 
	double l2 = d->qpos[5]; double l2dot = d->qvel[5]; 

	// state etimate
	body_no = 1;
	quat0 = d->xquat[4*body_no];	quatx = d->xquat[4*body_no+1];
	quaty = d->xquat[4*body_no+2];	quatz = d->xquat[4*body_no+3];
	quat2euler(quat0, quatx, quaty, quatz, &euler_x, &euler_y, &euler_z);
	// printf("body:%d, euler angle: %f %f %f\n", body_no, euler_x, euler_y, euler_z);
	abs_leg1 = -euler_y;

	body_no = 3;
	quat0 = d->xquat[4*body_no];	quatx = d->xquat[4*body_no+1];
	quaty = d->xquat[4*body_no+2];	quatz = d->xquat[4*body_no+3];
	quat2euler(quat0, quatx, quaty, quatz, &euler_x, &euler_y, &euler_z);
	// printf("body:%d, euler angle: %f %f %f\n", body_no, euler_x, euler_y, euler_z);
	abs_leg2 = -euler_y;

	body_no = 2;
	z_foot1 = d->xpos[3*body_no+2];

	body_no = 4;
	z_foot2 = d->xpos[3*body_no+2];

	// all transitions
	if(fsm_hip == fsm_leg2_swing && z_foot2 <= 0.05 && abs_leg1 < 0) {
		fsm_hip = fsm_leg1_swing;
	}
	else if(fsm_hip == fsm_leg1_swing && z_foot1 <= 0.05 && abs_leg2 < 0) {
		fsm_hip = fsm_leg2_swing;
	}
	
	// all actions
	if (fsm_hip == fsm_leg2_swing) {
		d->ctrl[0] = 0.5;
	}
	else if(fsm_hip == fsm_leg1_swing) {
		d->ctrl[0] = -0.5;
	}

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```

### FSM for bipedal motion for knee (stance_retract)

![stance_retract](/assets/Mujoco-200-tutorials/lec13/stance_retract.png){:width="400"}\

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here

	int body_no;
	double quat0, quatx, quaty, quatz;
	double euler_x, euler_y, euler_z;
	double abs_leg1, abs_leg2;

	double x = d->qpos[0]; double vx = d->qvel[0]; 
	double z = d->qpos[1]; double vz = d->qvel[1]; 
	double q1 = d->qpos[2]; double u1 = d->qvel[2]; 

	double l1 = d->qpos[3]; double l1dot = d->qvel[3]; 
	double q2 = d->qpos[4]; double u2 = d->qvel[4]; 
	double l2 = d->qpos[5]; double l2dot = d->qvel[5]; 

	// state etimate
	body_no = 1;
	quat0 = d->xquat[4*body_no];	quatx = d->xquat[4*body_no+1];
	quaty = d->xquat[4*body_no+2];	quatz = d->xquat[4*body_no+3];
	
	quat2euler(quat0, quatx, quaty, quatz, &euler_x, &euler_y, &euler_z);
	// printf("body:%d, euler angle: %f %f %f\n", body_no, euler_x, euler_y, euler_z);
	abs_leg1 = -euler_y;

	body_no = 3;
	quat0 = d->xquat[4*body_no];	quatx = d->xquat[4*body_no+1];
	quaty = d->xquat[4*body_no+2];	quatz = d->xquat[4*body_no+3];
	quat2euler(quat0, quatx, quaty, quatz, &euler_x, &euler_y, &euler_z);
	// printf("body:%d, euler angle: %f %f %f\n", body_no, euler_x, euler_y, euler_z);
	abs_leg2 = -euler_y;

	body_no = 2;
	z_foot1 = d->xpos[3*body_no+2];
	body_no = 4;
	z_foot2 = d->xpos[3*body_no+2];

	// all transitions
	if(fsm_hip == fsm_leg2_swing && z_foot2 < 0.05 && abs_leg1 < 0) {
		fsm_hip = fsm_leg1_swing;
		printf("fsm_leg1_swing\n");
	}
	else if(fsm_hip == fsm_leg1_swing && z_foot1 < 0.05 && abs_leg2 < 0) {
		fsm_hip = fsm_leg2_swing;
		printf("fsm_leg2_swing\n");
	}
	if(fsm_knee1 == fsm_knee1_stance && z_foot2 < 0.05 && abs_leg1 < 0) {
		fsm_knee1 = fsm_knee1_retract;
		printf("fsm_knee1_retract\n");
	}
	else if(fsm_knee1 == fsm_knee1_retract && abs_leg1 > 0.1) {
		fsm_knee1 = fsm_knee1_stance;
		printf("fsm_knee1_stance\n");
	}
	if(fsm_knee2 == fsm_knee2_stance && z_foot1 < 0.05 && abs_leg2 < 0) {
		fsm_knee2 = fsm_knee2_retract;
		printf("fsm_knee2_retract\n");
	}
	else if(fsm_knee2 == fsm_knee2_retract && abs_leg2 > 0.1) {
		fsm_knee2 = fsm_knee2_stance;
		printf("fsm_knee2_stance\n");
	}
	
	// all actions
	if (fsm_hip == fsm_leg2_swing) {
		d->ctrl[0] = 0.5;
	}
	else if(fsm_hip == fsm_leg1_swing) {
		d->ctrl[0] = -0.5;
	}
	if(fsm_knee1 == fsm_knee1_stance) {
		d->ctrl[2] = 0;
	}
	else if(fsm_knee1 == fsm_knee1_retract) {
		d->ctrl[2] = -0.25;
	}
	if(fsm_knee2 == fsm_knee2_stance) {
		d->ctrl[4] = 0;
	}
	else if(fsm_knee2 == fsm_knee2_retract) {
		d->ctrl[4] = -0.25;
	}

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```


### 주의사항

**body, geometry, joint 순으로 종속성을 엄밀히 고려하며 xml을 작성해야한다!**

joint간의 순서도 종속성이 있다.\
1번째 case는 z의 하위에 pin이 있지 않기때문에, pin은 제자리에 있다.

![fail](/assets/Mujoco-200-tutorials/lec13/fail.gif){:width="400"}\

```
<!-- pin이 따라가지 못함 -->
<joint name="pin" type="hinge" pos="0 0 0.5" axis="0 -1 0" />
<joint name="x" type="slide" pos="0 0 0.5" axis = "1 0 0" />
<joint name="z" type="slide" pos="0 0 0.5" axis = "0 0 1" /> 
```
2번째 case는 z의 하위에 pin이 있기때문에, pin이 z(leg1)를 따라간다.

![success](/assets/Mujoco-200-tutorials/lec13/success.gif){:width="400"}\


```
<!-- pin이 따라감 -->
<joint name="x" type="slide" pos="0 0 0.5" axis = "1 0 0" />
<joint name="z" type="slide" pos="0 0 0.5" axis = "0 0 1" />
<joint name="pin" type="hinge" pos="0 0 0.5" axis="0 -1 0" />
```

다음과 같이 swing할 때, pin의 위치는 d->ctrl[0]에 해당하는 leg2의 hip을 돌릴 때 반작용으로 영향을 받는다.\
따라서 보행이 불안정해져서 넘어질 수 밖에 없다.

```
if (fsm_hip == fsm_leg2_swing) {
	d->ctrl[0] = 0.5;
}
```