#include <stdio.h>
#include <math.h>

#include "nlopt.h"

/* optimization condition

cost = 0;

equality contrainits:
	ceq_1 = x_target - sensordata[0];
	ceq_2 = z_target - sensordata[2];

decision variables:
	Xin = {q_1, q_2}; joint angles

bounds:
	q_1, q_2 = {-3.14, 3.14};

*/

extern mjModel* m;
extern mjData* dsim;

double x_target;
double z_target;

void simulator(double Xin[2], double Xout[2]) {
	
	dsim->qpos[0] = Xin[0]; dsim->qpos[1] = Xin[1];
	dsim->ctrl[0] = dsim->qpos[0]; dsim->ctrl[2] = dsim->qpos[1];
	
	mj_forward(m,dsim);
	Xout[0] = dsim->sensordata[0];
	Xout[1] = dsim->sensordata[2];
	mj_resetData(m,dsim);
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

void inverse_kinematics(double Xin[2],double Xref[2])
{
int i;
nlopt_opt opt;

x_target = Xref[0]; z_target = Xref[1];

//establish sizes
unsigned n = 2; //number of decision variables
unsigned m_eq = 2; //number of equality constraints

//bounds for decision variables
double lb[] = { -3.14, -3.14 }; /* lower bounds */
double ub[] = { 3.14, 3.14 }; /* lower bounds */

//Set the algorithm and dimensionality
//L,G = global/local
//D,N = derivative / no derivative
opt = nlopt_create(NLOPT_LN_COBYLA, n); /* algorithm and dimensionality */

//Set the lower and upper bounds
nlopt_set_lower_bounds(opt, lb);
nlopt_set_upper_bounds(opt, ub);

//Set up cost
// mycost_data costdata;
// for (i=0;i<n;i++)
//   costdata.a[i]=1;
nlopt_set_min_objective(opt, mycost, NULL);

//set up equality constraint
double tol_eq[]={1e-8,1e-8};
// myequalityconstraints_data equalitydata;
// equalitydata.ceq_1 = 5;
// equalitydata.ceq_2 = 2;
nlopt_add_equality_mconstraint(opt, m_eq, myequalityconstraints, NULL, tol_eq);

// double tol_in[]={1e-8};
// myinequalityconstraints_data inequalitydata;
// inequalitydata.cin_1 = 5;
// nlopt_add_inequality_mconstraint(opt, m_in, myinequalityconstraints,&inequalitydata, tol_in);

nlopt_set_xtol_rel(opt, 1e-4);
// double x[] = { 1, 1, 1, 2, 1 };  // initial guess
double minf; /* `*`the` `minimum` `objective` `value,` `upon` `return`*` */
double error;
if ((error = nlopt_optimize(opt, Xin, &minf)) < 0) {
    printf("nlopt failed![%f]: %f %f\n",error, Xin[0], Xin[1]);
}
else {
    printf("found minimum at f(%g,%g) = %0.10g\n", Xin[0], Xin[1], minf);
}

nlopt_destroy(opt);
}
