---
layout: post
title: "Mujoco-200-tutorials-Lec2:Projectile with drag"
date: 2024-04-15 18:14 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

Drag force를 가진 projectile을 simulation해보자.

[lecture_url](https://www.youtube.com/watch?v=Ga3twtlgKsM&list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G&index=8)

[docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)

[mujoco/sample/basic](https://github.com/google-deepmind/mujoco/blob/main/sample/basic.cc)

![lec2](/assets/Mujoco-200-tutorials/lec2/lec2.png){: width="512" height="512" }

## Full code

[test.cc](/assets/Mujoco-200-tutorials/lec2/test.cc)

[test.xml](/assets/Mujoco-200-tutorials/lec2/test.xml)

## change the initial view of camera(mjvCamera)

```
double arr_view[] = &#123;90, -45, 8, 0.000000, 0.000000, 0.000000&#125;;
cam.azimuth = arr_view[0];
cam.elevation = arr_view[1];
cam.distance = arr_view[2];
cam.lookat[0] = arr_view[3];
cam.lookat[1] = arr_view[4];
cam.lookat[2] = arr_view[5];
```

## change gravity(mjModel)

```
m->opt.gravity[2] = -1;
```

## Set initial position/velocity(mjData, d->qpos, d->qvel)

```
// qpos is dimension nqx1(7x1). (refer to MJMODEL.TXT got by clicking 'print model' in ./simulate)
d->qpos[2] = .1;
d->qvel[2] = 5;
d->qvel[0] = 10;
```

## visualize world frame(mjvOption)

```
// mjvOption (visualization option)
opt.frame = mjFRAME_WORLD;
```

## Apply drag force(mjData, d->qfrc_applied)

```
// drag force = -c*v^2*unit_vector(v);
// vector(v) = vx*i + vy*j + vz*k
// v=sqrt(vx^2+vy^2+vz^2)
// unit_vector(v) = vector(v)/v
// fx = -c*v*vx;
// fy = -c*v*vy;
// fz = -c*v*vz;
double vx,vy,vz;
vx = d->qvel[0]; vy = d->qvel[1]; vz = d->qvel[2];
double v;
v = sqrt(pow(vx,2)+pow(vy,2)+pow(vz,2));
double fx,fy,fz,c;
c = 10;
fx = -c*v*vx;
fy = -c*v*vy;
fz = -c*v*vz;
d->qfrc_applied[0] = fx;
d->qfrc_applied[1] = fy;
d->qfrc_applied[2] = fz;
```
