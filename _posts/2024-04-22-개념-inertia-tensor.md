---
layout: post
title: 개념-inertia tensor
date: 2024-04-22 14:09 +0900
description:
author: rosi
categories: [개념]
tags:
math: true
---

### cheet sheet

1.moment of inertia

$$ I = i_{xx}cos^2\alpha + i_{yy}cos^2\beta + i_{zz}cos^2\gamma + 2i_{xy}{cos\alpha}{cos\beta} + 2i_{yz}{cos\beta}{cos\gamma} + 2i_{xz}{cos\alpha}{cos\gamma} $$ - 식(1)

2.dyadic product

2.1.dyadic product of two vector a and b is denoted by ab (병치)

2.2.dyadic product of two vector a and b in $$\mathbb{R}^3$$ euclidean space is as following form.

$$\mathbf {ab} = a_{1}b_{1}\mathbf {ii} +a_{1}b_{2}\mathbf {ij} +a_{1}b_{3}\mathbf {ik}$$

$$a_{2}b_{1}\mathbf {ji} +a_{2}b_{2}\mathbf {jj} +a_{2}b_{3}\mathbf {jk}$$

$$a_{3}b_{1}\mathbf {ki} +a_{3}b_{2}\mathbf {kj} +a_{3}b_{3}\mathbf {kk}$$

3.moment of inertia tensor

$$ \mathbf{I} = \hat{n}^T \mathbf {I} \hat{n} $$

such that 

$$ \mathbf{I} = {\begin{bmatrix}I_{11}&I_{12}&I_{13} \\ I_{21}&I_{22}&I_{23} \\ I_{31}&I_{32}&I_{33}\end{bmatrix}} $$

$$ \hat{n} = \hat{i}{cos\alpha} + \hat{j}{cos\beta} + \hat{k}{cos\gamma} $$

4.angular momentum

$$ \vec{L} = (\sum (m_ir_i^2) - \sum m_i \vec{r_i} (\vec{r_i})) \cdot \vec{w} = \mathbf{I}\vec{w} $$

여기서, $$\vec{r}_i(\vec{r}_i)$$는 dyadic product이다.

5.kinematics energy

$$T= \frac{1}{2}m_iv_i^2 = \frac{1}{2}\vec{w}\vec{L} = \frac{1}{2}\vec{w}\mathbf{I}\vec{w}^T$$


### process

1.moment of inertia

$$ \hat{n} = \hat{i}{cos\alpha} + \hat{j}{cos\beta} + \hat{k}{cos\gamma} $$

$$ |r_i \times \hat{n}|^2 = r_{i\perp}^2 = 
(y_i^2 + z_i^2)cos^2\alpha + (x_i^2 + z_i^2)cos^2\beta + (x_i^2 + y_i^2)cos^2\gamma - 2x_iy_i{cos\alpha}{cos\beta} 2y_iz_i{cos\beta}{cos\gamma} 2x_iz_i{cos\alpha}{cos\gamma}
$$ 

such that 

$$\hat{n}$$ unit vector of rotational axis, $$r_i$$ is position vector of particle.

$$ I = \sum r_{i\perp}^2m_i $$

2.dyadic product

$$
\mathbf {ab} =\sum _{j=1}^{N}\sum _{i=1}^{N}a_{i}b_{j}\mathbf {e} _{i}\mathbf {e} _{j} 
$$

3.moment of inertia tensor 

식(1) 참고

4.angular momentum, 5.kinematics energy

reference 참고 (angular momentum, kinematics energy represented as inertia tensor)



아래 자료는 다시 정리가 필요할 때 읽어보자.

[3D Rigid Body Dynamics](/assets/img/개념/inertia tensor.pdf)

#### Reference

[moment of inertia] Dynamics(2022, R.C. Hibbeler)-17.2 PLANAR KINETIC EQUATIONS OF MOTION

[moment of inertia and product of inertia](https://blog.naver.com/gdpresent/220245228024)

[angular momentum, kinematics energy represented as inertia tensor](https://blog.naver.com/gdpresent/220245736508)

[dyadic product](https://en.wikipedia.org/wiki/Dyadics#Dyadic,_outer,_and_tensor_products)