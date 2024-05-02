---
layout: post
title: 'mujoco-200-tutorials-Lec12: Inverse Kinematics using Nonlinear Optimization'
date: 2024-04-23 16:41 +0900
description:
author: rosi
categories: [mujoco]
tags:
math: true
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)\
[mujoco docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)\
[NLopt docs](https://nlopt.readthedocs.io/en/latest/NLopt_Reference/)

이번에는 IK를 nonlinear optimization sw인 NLopt를 이용해서 풀어보자.\
거의 치트키인거같으니까 너무 심취하지말자..

### Full code

[test.c](/assets/Mujoco-200-tutorials/lec12/test.c)\
[constrained.c](/assets/Mujoco-200-tutorials/lec12/constrained.c)\
[xref_zref.m](/assets/Mujoco-200-tutorials/lec12/xref_zref.m)\
[test.xml](/assets/Mujoco-200-tutorials/lec12/test.xml)

### mjData* dsim (for simulator)

mjData* d(for robot)\
mjData* dsim(for simulator) 으로 data를 분리한다.

```
mjData* d = NULL;               
mjData* dsim = NULL; 

// make data
d = mj_makeData(m);
dsim = mj_makeData(m);

mj_deleteData(d);
mj_deleteData(dsim);

```

in constrained.c

```

extern mjModel* m;
extern mjData* dsim;

```

### get joint angle using NLopt

```

extern mjModel* m;
extern mjData* dsim;

double x_target;
double z_target;

void simulator(double Xin[2], double Xout[2]) {
	
	dsim->qpos[0] = Xin[0];
	dsim->qpos[1] = Xin[1];
	dsim->ctrl[0] = dsim->qpos[0]; dsim->ctrl[2] = dsim->qpos[1];
	mj_forward(m,dsim);
	Xout[0] = dsim->sensordata[0];
	Xout[1] = dsim->sensordata[2];
}

double mycost(unsigned n, const double *x, double *grad, void *costdata)
{
    double cost = 0;

    return cost;
}


double myequalityconstraints(unsigned m, double *result, unsigned n,
                             const double *x,  double *grad,
                             void *equalitydata)
{
	double Xout[2] = {0};
	simulator(x,Xout);
	result[0] = x_target - Xout[0]; //dsim->sensordata[0];
	result[1] = z_target - Xout[1]; //dsim->sensordata[2];

 }

```

### create Xref (lemniscate)

[lemniscate wolfram alpha](https://mathworld.wolfram.com/Lemniscate.html)

$$
x = \frac{a\cos{t}}{1+\sin^2{t}}\

y = \frac{a\sin{t}\cos{t}}{1+\sin^2{t}}
$$

```
void curve(double t, double Xref[2]) {
	double denominator = 1 + sin(omega*t)*sin(omega*t);
	Xref[0] = center_x + a*cos(omega*t)/denominator;
	Xref[1] = center_z + a*sin(omega*t)*cos(omega*t)/denominator;
}

```

### 주의사항

![back trace](/assets/Mujoco-200-tutorials/lec12/bt.png){:width="300"}\
![internal_external_callback](/assets/Mujoco-200-tutorials/lec12/internal_external_callback.png){:width="300"}

simulator에서 joint angle input에 대한 tip position output을 구하기위해 \
mjData* dsim을 사용했다. 여기서 주의해야할 점이 있다.\
**dsim도 integrator="RK4"계산을 하면서 control callback을 사용한다는 것**이다.\
즉, callback에서 inverse kinematics를 호출하면 inverse kinematics내부에서도 callback을 호출(mj_forward내에서 callback호출)하기때문에\
재귀적으로 무한 호출하게 된다!

다음의 코드는 internal callaback을 사용할 때, 무한 재귀호출이 발생하는 상황이다.\
이거때문에 반나절을 날렸다.. 

```
int main() {
	mjcb_control = mycontroller;
}

void mycontroller(const mjModel* m, mjData* d)
{	
	// mycontroller가 콜백이면,
	// 콜백내에서 다시 콜백을 호출할수 있는 inverse_kinematics()를 호출하기때문에 무한 호출이고, 스택 overflow가 발생하여 segmentation fault가 발생한다.
	inverse_kinematics(Xin,Xref);
}
void inverse_kinematics(double Xin[2],double Xref[2]) {
	nlopt_add_equality_mconstraint(opt, m_eq, myequalityconstraints, NULL, tol_eq);
}

double myequalityconstraints(unsigned m, double *result, unsigned n,
                             const double *x,  double *grad,
                             void *equalitydata)
{
	simulator(x,Xout);
}

void simulator(double Xin[2], double Xout[2]) {
	mj_forward(m,dsim); // **recursive call!**
}

```

다음은 mjcb_control에 대한 docs의 설명이다.

> The control callback is called from within mj_forward and mj_step, just before the controls and applied forces are needed. When using the RK integrator, > it will be called 4 times per step. The alternative way of specifying controls and applied forces is to set them before mj_step, or use mj_step1 and 
> mj_step2.



### 해결책

![figure](/assets/Mujoco-200-tutorials/lec12/figure.png){:width="300"}

재귀호출을 막기위해, 다음과 같이 callback을 명시적으로 루프에서 실행한다.
그림과 같이 성공적으로 Xout이 Xref를 추종하는 제약조건을 준수하는 것을 볼 수 있다.

```
int main() {
	// mjcb_control = mycontroller;
	while(!glfwWindowShouldClose(window)) {
		mycontroller(m,d);
	}
}
```