---
layout: post
title: 'mujoco-200-tutorials-Lec8: Modeling and controlling hybrid systems, flying pendulum'
date: 2024-04-23 16:40 +0900
description:
author: rosi
categories: [mujoco]
tags:
math: true
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)

[docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)


hybrid systems: systems with continuous and descrete dynamics/modes

floating base를 가진 pendulum의 dynamics를 확인해보자.\
floating base를 가진 pendulum의 constraint force를 확인해보자.\
FSM으로 hybrid system을 구현해보자.

### Full code

[test.cc](/assets/Mujoco-200-tutorials/lec8/test.cc)\
[test.xml](/assets/Mujoco-200-tutorials/lec8/test.xml)

### dynamics

dynamics는 다음과 같다.($$F_0$$는 constant)\
좌변과 우변이 같은지 확인해보자.

$$
M\ddot{q} + f = \tau + J_0^TF_0
$$

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here
	// void mj_fullM(const mjModel* m, mjtNum* dst, const mjtNum* M);
	const int nv = 3;
	double M[nv*nv] = {0};
	mj_fullM(m,M,d->qM);
	// printf("%f %f %f\n", M[0],M[1],M[2]);
	// printf("%f %f %f\n", M[3],M[4],M[5]);
	// printf("%f %f %f\n", M[6],M[7],M[8]);
	
	double qddot[nv] = {0};
	qddot[0] = d->qacc[0];
	qddot[1] = d->qacc[1];
	qddot[2] = d->qacc[2];
	
	double f[nv] = {0};
	f[0] = d->qfrc_bias[0];
	f[1] = d->qfrc_bias[1];
	f[2] = d->qfrc_bias[2];
	
	double M_qddot[nv] = {0};
	// void mju_mulMatVec(mjtNum* res, const mjtNum* mat, const mjtNum* vec, int nr, int nc);
	mju_mulMatVec(M_qddot, M, qddot, nv, nv);

	double lhs[nv] = {0};
	lhs[0] = M_qddot[0] + f[0];
	lhs[1] = M_qddot[1] + f[1];
	lhs[2] = M_qddot[2] + f[2];
	
	double tau[nv] = {0};
	tau[0] = d->qfrc_applied[0]; 
	tau[1] = d->qfrc_applied[1]; 
	tau[2] = d->qfrc_applied[2]; 

	// void mj_jac(const mjModel* m, const mjData* d, mjtNum* jacp, mjtNum* jacr, const mjtNum point[3], int body);
	// mj_jac이 이미 계산되어 data에 들어가있다.
	double J0[3*nv] = {0};
	for(int i=0; i<3*nv; i++)
		J0[i] = d->efc_J[i];
	
	double F0[3] = {0};
	for(int i=0; i<3; i++)
		F0[i] = d->efc_force[i];
	
	double JT_F[3] = {0};
	mju_mulMatTVec(JT_F, J0, F0, 3, 3);

	double rhs[3] = {0};
	rhs[0] = tau[0] + JT_F[0];
	rhs[1] = tau[1] + JT_F[1];
	rhs[2] = tau[2] + JT_F[2];

	printf("%f %f %f\n", lhs[0],lhs[1],lhs[2]);
	printf("%f %f %f\n", rhs[0],rhs[1],rhs[2]);

	printf("******\n");


  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```

### constrain force계산법과 mujoco trick

dynamics에 constraint condition까지 고려하면 **constrain force**를 계산할 수 있다.
(floating base이니까 0에서 free force이고 가속도가 0이다.)

dynamics는 다음과 같다.

$$M\ddot{q} -J_0^TF_0 = \tau - f$$ - (1)

constraint condition은 다음과 같다.

$$\dot{r}_0 = J_0\dot{q}_0$$\
such that $${r}_0 = [x\ y\ \theta]^T$$

한번 더 미분하면 다음과 같다.

$$\ddot{r}_0 = J_0\ddot{q} + \dot{J}_0\dot{q} = 0$$ \
-> $$J_0\ddot{q} = -\dot{J}_0\dot{q}$$ - (2)

식(1),식(2)를 matrix form으로 표현하면 다음과 같다.

$$
\begin{bmatrix}
M &  -J_0^T\\
J_0 & 0\\
\end{bmatrix}
\begin{bmatrix}
\ddot{q} \\
F_0
\end{bmatrix}
=
\begin{bmatrix}
\tau - f \\
-\dot{J}_0\dot{q}
\end{bmatrix}
$$

따라서 이 식을 풀면 constraint force $$F_0$$를 구할 수 있다. \
하지만 mujoco가 대신 구해놨다. (d->efc_force)

hybrid system구현을 위해 사용할 trick은 다음과 같다.

pendulum 2개를 사용할 것이다.\
pendulum1은 $$\dot{J}_0와 F_0$$를 얻기위한 것이고, simulator에서 숨길것이다.\
pendulum2의 joint에 pendulum1으로 계산한 $$J_0^TF_0$$을 더하면, \
pendulum2는 constraint force를 받고 있는 것이다. 

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here
	// void mj_fullM(const mjModel* m, mjtNum* dst, const mjtNum* M);
	const int nv = 6;
	double M[nv*nv] = {0};
	mj_fullM(m,M,d->qM);
	// original system
	// M0 M1 M2
	// M3 M4 M5
	// M6 M7 M8

	// two pendulum
	// M0 M1 M2 M3 M4 M5
	// M6 M7 M8 M9 M10 M11
	// ...

	// need to left 3x3 mass matrix for pendulum1
	const int nv1 = 3;
	double M1[nv1*nv1] = {0};
	M1[0] = M[0]; M1[1] = M[1]; M1[2] = M[2];
	M1[3] = M[6]; M1[4] = M[7]; M1[5] = M[8];
	M1[6] = M[12]; M1[7] = M[13]; M1[8] = M[14];	

	double qddot[nv] = {0};
	qddot[0] = d->qacc[0];
	qddot[1] = d->qacc[1];
	qddot[2] = d->qacc[2];
	
	double f[nv] = {0};
	f[0] = d->qfrc_bias[0];
	f[1] = d->qfrc_bias[1];
	f[2] = d->qfrc_bias[2];
	
	double M_qddot[nv] = {0};
	// void mju_mulMatVec(mjtNum* res, const mjtNum* mat, const mjtNum* vec, int nr, int nc);
	mju_mulMatVec(M_qddot, M1, qddot, nv, nv);

	double lhs[nv] = {0};
	lhs[0] = M_qddot[0] + f[0];
	lhs[1] = M_qddot[1] + f[1];
	lhs[2] = M_qddot[2] + f[2];
	
	double tau[nv] = {0};
	tau[0] = d->qfrc_applied[0]; 
	tau[1] = d->qfrc_applied[1]; 
	tau[2] = d->qfrc_applied[2]; 

	// void mj_jac(const mjModel* m, const mjData* d, mjtNum* jacp, mjtNum* jacr, const mjtNum point[3], int body);
	// mj_jac이 이미 계산되어 data에 들어가있다.
	double J0[3*nv] = {0};
	for(int i=0; i<3*nv; i++)
		J0[i] = d->efc_J[i];
	// J[3xnv] = J[3x6]
	// J0 J1 J2 J3 J4 J5
	// J6 J7 J8 J9 J10 J11
	// ...
	double J1[3*nv1] = {0};
	J1[0] = J0[0]; J1[1] = J0[1]; J1[2] = J0[2];
	J1[3] = J0[6]; J1[4] = J0[7]; J1[5] = J0[8];
	J1[6] = J0[12]; J1[7] = J0[13]; J1[8] = J0[14];	

	double F0[3] = {0};
	for(int i=0; i<3; i++)
		F0[i] = d->efc_force[i];
	
	double JT_F[3] = {0};

	mju_mulMatTVec(JT_F, J1, F0, 3, 3);

	double rhs[3] = {0};
	rhs[0] = tau[0] + JT_F[0];
	rhs[1] = tau[1] + JT_F[1];
	rhs[2] = tau[2] + JT_F[2];

	// printf("%f %f %f\n", lhs[0],lhs[1],lhs[2]);
	// printf("%f %f %f\n", rhs[0],rhs[1],rhs[2]);
	// printf("******\n");
	d->qfrc_applied[3] = JT_F[0];
	d->qfrc_applied[4] = JT_F[1];
	d->qfrc_applied[5] = JT_F[2];

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```

### implement hybrid system by FSM

```
void mycontroller(const mjModel* m, mjData* d)
{
	// write control here
	// void mj_fullM(const mjModel* m, mjtNum* dst, const mjtNum* M);
	const int nv = 6;
	double M[nv*nv] = {0};
	mj_fullM(m,M,d->qM);
	// original system
	// M0 M1 M2
	// M3 M4 M5
	// M6 M7 M8

	// two pendulum
	// M0 M1 M2 M3 M4 M5
	// M6 M7 M8 M9 M10 M11
	// ...

	// need to left 3x3 mass matrix for pendulum1
	const int nv1 = 3;
	double M1[nv1*nv1] = {0};
	M1[0] = M[0]; M1[1] = M[1]; M1[2] = M[2];
	M1[3] = M[6]; M1[4] = M[7]; M1[5] = M[8];
	M1[6] = M[12]; M1[7] = M[13]; M1[8] = M[14];	

	double qddot[nv] = {0};
	qddot[0] = d->qacc[0];
	qddot[1] = d->qacc[1];
	qddot[2] = d->qacc[2];
	
	double f[nv] = {0};
	f[0] = d->qfrc_bias[0];
	f[1] = d->qfrc_bias[1];
	f[2] = d->qfrc_bias[2];
	
	double M_qddot[nv] = {0};
	// void mju_mulMatVec(mjtNum* res, const mjtNum* mat, const mjtNum* vec, int nr, int nc);
	mju_mulMatVec(M_qddot, M1, qddot, nv, nv);

	double lhs[nv] = {0};
	lhs[0] = M_qddot[0] + f[0];
	lhs[1] = M_qddot[1] + f[1];
	lhs[2] = M_qddot[2] + f[2];
	
	double tau[nv] = {0};
	tau[0] = d->qfrc_applied[0]; 
	tau[1] = d->qfrc_applied[1]; 
	tau[2] = d->qfrc_applied[2]; 

	// void mj_jac(const mjModel* m, const mjData* d, mjtNum* jacp, mjtNum* jacr, const mjtNum point[3], int body);
	// mj_jac이 이미 계산되어 data에 들어가있다.
	double J0[3*nv] = {0};
	for(int i=0; i<3*nv; i++)
		J0[i] = d->efc_J[i];
	// J[3xnv] = J[3x6]
	// J0 J1 J2 J3 J4 J5
	// J6 J7 J8 J9 J10 J11
	// ...
	double J1[3*nv1] = {0};
	J1[0] = J0[0]; J1[1] = J0[1]; J1[2] = J0[2];
	J1[3] = J0[6]; J1[4] = J0[7]; J1[5] = J0[8];
	J1[6] = J0[12]; J1[7] = J0[13]; J1[8] = J0[14];	

	double F0[3] = {0};
	for(int i=0; i<3; i++)
		F0[i] = d->efc_force[i];
	
	double JT_F[3] = {0};

	mju_mulMatTVec(JT_F, J1, F0, 3, 3);

	double rhs[3] = {0};
	rhs[0] = tau[0] + JT_F[0];
	rhs[1] = tau[1] + JT_F[1];
	rhs[2] = tau[2] + JT_F[2];

	// printf("%f %f %f\n", lhs[0],lhs[1],lhs[2]);
	// printf("%f %f %f\n", rhs[0],rhs[1],rhs[2]);
	// printf("******\n");

	// transition
	if(fsm == fsm_swing && d->qpos[5] > 3.14*2 + 3.14/3) {
		fsm = fsm_free;
	}

	if(fsm == fsm_swing) {
		// 기존에 joint velocity를 4로 control하고 있던 상황
		d->qfrc_applied[2] = 5*(8-d->qpos[2]);

		d->qfrc_applied[3] = JT_F[0];
		d->qfrc_applied[4] = JT_F[1];
		d->qfrc_applied[5] = JT_F[2] + d->qfrc_applied[2];
		printf("swinging..\n");
	}
	else if(fsm == fsm_free) {
		d->qfrc_applied[3] = 0;
		d->qfrc_applied[4] = 0;
		d->qfrc_applied[5] = 0;
		printf("free~~\n");
	}

  //write data here (dont change/dete this function call; instead write what you need to save in save_data)
  if ( loop_index%data_chk_period==0)
    {
      save_data(m,d);
    }
  loop_index = loop_index + 1;
}

```