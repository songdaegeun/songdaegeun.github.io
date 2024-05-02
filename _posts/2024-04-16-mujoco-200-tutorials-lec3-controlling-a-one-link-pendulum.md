---
layout: post
title: "Mujoco-200-tutorials-Lec3:Controlling a one-link pendulum"
date: 2024-04-16 17:51 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

One-link pendulum에 대해 position control with position servo(only kp gain, marginal stable), velocity control with velocity servo(only kd gain, stable), position control with position servo and velocity servo(kp gain, kd gain. PD contorl), potition control with torque input(PD control) 각각의 controller를 설계해보자.

[lecture_url](https://www.youtube.com/watch?v=JNgO_OPVb5k&list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G&index=8)

[docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)

![position servo attribute](/assets/Mujoco-200-tutorials/lec3/lec3.png){: width="512" height="512" }

### Full code

[test.cc](/assets/Mujoco-200-tutorials/lec3/test.cc) [test.xml](/assets/Mujoco-200-tutorials/lec3/test.xml)

### Modify pendulum.xml

### 1. Create free swining pendulum

```
<mujoco>
  <worldbody>
    <body pos="0 0 2" euler="0 180 0">
      <joint name="pin" type="hinge" pos="0 0 .5" axis="0 -1 0"/>
      <geom type="cylinder" size=".05 .5" rgba="0 .9 0 1" mass="1"/>
    </body>
  </worldbody>
</mujoco>

```

### 2. Add 3 actuator: torque control, position servo, velocity servo

```
<mujoco>
  <actuator>
	<motor joint="pin" name="torque" ctrllimited="true" ctrlrange="-100 100"/>
	<!-- PD control for position control-->
	<position name="position servo" joint="pin" kp="5"/>
	<velocity name="velocity servo" joint="pin" kv="1"/>
	<!-- PD control -->
  </actuator>
</mujoco>

```

### 3. Add sensors: position, velocity

```
  <sensor>
  	<!-- <jointpos joint="pin" noise="0.2"/> --> // noise attribute is deprecated
	<jointpos joint="pin"/>
	<!-- <jointvel joint="pin" noise="1"/>  -->  // noise attribute is deprecated
	<jointvel joint="pin"/>
  </sensor>
```

### Modify main.c

### 4. Torque control(potition control with torque input, w/w.o. sensor noise)

int main() {
// position of joint(in this example, pin)
d->qpos[0]=1.57; // pi/2
mjcb_control = my_controller;
}
w.o. sensor noise

```
void my_controller(const mjModel* m, mjData* d)
{
	// 0: torque actuator (applied to input torque in dynamics for this simulation)
	d->ctrl[0] = 5*(0-d->qpos[0]) + 1*(0-d->qvel[0]); // PD position control; kp=5, kd=1
}
```

with sensor noise

```
void my_controller(const mjModel* m, mjData* d)
{
	// make noise
	unsigned int seed = static_cast<unsigned int>(time(NULL));
	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution(0.0, .2);
	double noise = distribution(generator);
	double n_s0 = d->sensordata[0]+noise;
	double n_s1 = d->sensordata[1]+noise;

	// 0: torque actuator (applied to input torque in dynamics for this simulation)
	// d->ctrl[0] = 5*(0-d->qpos[0]) + 1*(0-d->qvel[0]); // PD position control; kp=5, kd=1
	d->ctrl[0] = 10*(0-n_s0) + 1*(0-n_s1); // PD position control; kp=5, kd=1
}
```

on/off by c++ code

```
void set_torque(const mjModel*m, int flag) {
	int actuator_no = 0;
	m->actuator_gainprm[mjNGAIN*actuator_no+0] = flag;
}
void my_controller(const mjModel* m, mjData* d)
{
	set_torque(m, 1);
}
```

### 5. Position servo(spring, only kp)

```
int main()  {
	// position of joint(in this example, pin)
	d->qpos[0]=1.57; // pi/2
	mjcb_control = my_controller;
}
void my_controller(const mjModel* m, mjData* d)
{
	// 1: position servo (position의 setpoint)
	d->ctrl[1] = 0.5;
}
<position name="position servo" joint="pin" kp="5"/>
<velocity name="velocity servo" joint="pin" kv="0"/>
```

### 6. Velocity servo(velocity control, only kd)

```
int main()  {
	// position of joint(in this example, pin)
	d->qpos[0]=1.57; // pi/2
	mjcb_control = my_controller;
}
void my_controller(const mjModel* m, mjData* d)
{
	// 2: velocity servo (velocity의 setpoint. error dynamics를 풀어보면 steady state error=0)
	d->ctrl[2] = 0.2;
}
<position name="position servo" joint="pin" kp="0"/>
<velocity name="velocity servo" joint="pin" kv="10"/>
```

### 7. Position and velocity servo (position control, kp, kd)

1. by xml

```
int main()  {
	// position of joint(in this example, pin)
	d->qpos[0]=1.57; // pi/2
	mjcb_control = my_controller;
}
void my_controller(const mjModel* m, mjData* d)
{
	// 1: position servo (position의 setpoint)
	d->ctrl[1] = 0.5;
}
<position name="position servo" joint="pin" kp="5"/>
<velocity name="velocity servo" joint="pin" kv="1"/>
```

2. by c++ code

```
<mujoco>
  <actuator>
	<motor joint="pin" name="torque" ctrllimited="true" ctrlrange="-100 100"/>
	<!-- PD control for position control-->
	<position name="position servo" joint="pin" kp="0"/>
	<velocity name="velocity servo" joint="pin" kv="0"/>
	<!-- PD control -->
  </actuator>
</mujoco>

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
	// 1: position servo (position의 setpoint)
	double kp = 1000;
	set_kp(m, kp);
	double kv = 1;
	set_kv(m, kv);

	// // actuator_no(1 or 2)의 gainprm, biasprm에 대해, mjNGAIN를 순회하며 출력하여 xml파일에서 kp설정시 몇번째에서 변하는지 확인.
	// int actuator_no = 2;
	// for(int i=0; i<mjNGAIN;i++) {
	// 	// printf("%f\n", m->actuator_gainprm[mjNGAIN*actuator_no+i]);
	// 	printf("%f\n", m->actuator_biasprm[mjNGAIN*actuator_no+i]);
	// }
	// printf("**********\n");

	d->ctrl[1] = -3.14/4;
}
```
