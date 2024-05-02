---
layout: post
title: 'mujoco-200-tutorials-Lec10: Install and run nonlinear optimization software
  NLOPT'
date: 2024-04-23 16:40 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)\
[NLopt_Reference](https://nlopt.readthedocs.io/en/latest/NLopt_Reference/)\
[Nlopt20분강의](https://www.youtube.com/watch?v=1iuhMCnF3w4)\
[강의 article](https://aleksandarhaber.com/solve-optimization-problems-in-c-c-by-using-nlopt-library/)

nonlinear optimization SW인 NLOPT를 install하고,\
간단한 예제를 running해보자.\

### tutorial

```
gcc tutorial.c -o tutorial -w -lnlopt -lm
```

```
#include <stdio.h>
#include <math.h>

#include "nlopt.h"

/* Optimization problem
min sqrt (x2)

subject to:
x2 >= (1-x1)^3
x2 >= (2 x1)^3

*/


double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)
{
    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]); //sqrt(x2)
}

typedef struct {
    double a, b;
} my_constraint_data;

double myconstraint(unsigned n, const double *x, double *grad, void *data)
{
    my_constraint_data *d = (my_constraint_data *) data;
    double a = d->a, b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
 }

int main() {

double lb[2] = { -HUGE_VAL, 0 }; /* lower bounds */
nlopt_opt opt;

unsigned n = 2;
opt = nlopt_create(NLOPT_LD_MMA, n); /* algorithm and dimensionality */
nlopt_set_lower_bounds(opt, lb);
nlopt_set_min_objective(opt, myfunc, NULL);

my_constraint_data data[2] = { {2,0}, {-1,1} };

nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);

nlopt_set_xtol_rel(opt, 1e-4);

double x[2] = { 1.234, 5.678 };  /* `*`some` `initial` `guess`*` */
double minf; /* `*`the` `minimum` `objective` `value,` `upon` `return`*` */
if (nlopt_optimize(opt, x, &minf) < 0) {
    printf("nlopt failed!\n");
}
else {
    printf("found minimum at f(%g,%g) = %0.10g\n", x[0], x[1], minf);
}

nlopt_destroy(opt);
}

```

### constrained

```
gcc constrained.c -o constrained -w -lnlopt -lm
```

```
#include <stdio.h>
#include <math.h>

#include "nlopt.h"

/* Optimization problem

\begin{align}
\min\limits_{x}  f(x) &= a1*x_1^2 + a2*x_2^2 + a3*x_3^2 + a4*x_4^2 + a5*x_5^2 \nonumber \\

\mbox{subject to:} & \nonumber \\
x_1+x_2+x_3 &= ceq_1 \nonumber \\
x_3^2+x_4  &= ceq_2 \nonumber \\
x_1 &>= 0.3 \nonumber \\
x_3 &<= 5 \nonumber \\
x_4^2 + x_5^2 &<= cin_1 \nonumber
\end{align}

a1 = a2 = a3 = a4 = a5 = 1;
ceq_1 = 5;
ceq_2 = 2;
cin_1 = 5;

Solution is
1.7736
  1.7736
  1.4527
 -0.1104
 -0.0000

 Cost = 8.414
*/

typedef struct {
    double a[5];
}mycost_data;

typedef struct {
    double ceq_1;
    double ceq_2;
}myequalityconstraints_data;

typedef struct {
    double cin_1;
}myinequalityconstraints_data;


double mycost(unsigned n, const double *x, double *grad, void *costdata)
{
    mycost_data *data = (mycost_data *) costdata;

    int i;
    double a[5]={0};
    for (i=0;i<n;i++)
      a[i] = data->a[i];

    double cost = 0;
    for (i=0;i<n;i++)
      cost += a[i]*x[i]*x[i];

    return cost;
}


double myequalityconstraints(unsigned m, double *result, unsigned n,
                             const double *x,  double *grad,
                             void *equalitydata)
{
    myequalityconstraints_data *data = (myequalityconstraints_data *) equalitydata;

    double c1 = data->ceq_1;
    double c2 = data->ceq_2;
    double x1 = x[0], x2 = x[1], x3 = x[2], x4 = x[3], x5 = x[4];
    result[0] = x1+x2+x3-c1; //5;
    result[1] = x3*x3+x4-c2; //2;
 }

 double myinequalityconstraints(unsigned m, double *result, unsigned n,
                                const double *x,  double *grad,
                                void* inequalitydata)
 {
     myinequalityconstraints_data *data = (myinequalityconstraints_data *) inequalitydata;

     double c1 = data->cin_1;
     double x1 = x[0], x2 = x[1], x3 = x[2], x4 = x[3], x5 = x[4];
     result[0] = x4*x4+x5*x5-c1; //5;
  }


void main()
{
int i;
nlopt_opt opt;

//establish sizes
unsigned n = 5; //number of decision variables
unsigned m_eq = 2; //number of equality constraints
unsigned m_in = 1; //number of inequality constraints

//bounds for decision variables
double lb[] = { 0.3, -HUGE_VAL, -HUGE_VAL,  -HUGE_VAL, -HUGE_VAL }; /* lower bounds */
double ub[] = { HUGE_VAL, HUGE_VAL, 5, HUGE_VAL, HUGE_VAL }; /* lower bounds */

//Set the algorithm and dimensionality
//L,G = global/local
//D,N = derivative / no derivative
opt = nlopt_create(NLOPT_LN_COBYLA, n); /* algorithm and dimensionality */

//Set the lower and upper bounds
nlopt_set_lower_bounds(opt, lb);
nlopt_set_upper_bounds(opt, ub);

//Set up cost
mycost_data costdata;
for (i=0;i<n;i++)
  costdata.a[i]=1;
nlopt_set_min_objective(opt, mycost, &costdata);

//set up equality constraint
double tol_eq[]={1e-8,1e-8};
myequalityconstraints_data equalitydata;
equalitydata.ceq_1 = 5;
equalitydata.ceq_2 = 2;
nlopt_add_equality_mconstraint(opt, m_eq, myequalityconstraints, &equalitydata, tol_eq);

double tol_in[]={1e-8};
myinequalityconstraints_data inequalitydata;
inequalitydata.cin_1 = 5;
nlopt_add_inequality_mconstraint(opt, m_in, myinequalityconstraints,&inequalitydata, tol_in);


nlopt_set_xtol_rel(opt, 1e-4);
double x[] = { 1, 1, 1, 2, 1 };  // initial guess
double minf; /* `*`the` `minimum` `objective` `value,` `upon` `return`*` */
if (nlopt_optimize(opt, x, &minf) < 0) {
    printf("nlopt failed!\n");
}
else {
    printf("found minimum at f(%g,%g,%g,%g,%g) = %0.10g\n", x[0], x[1],x[2], x[3],x[4], minf);
}

nlopt_destroy(opt);
}
```
