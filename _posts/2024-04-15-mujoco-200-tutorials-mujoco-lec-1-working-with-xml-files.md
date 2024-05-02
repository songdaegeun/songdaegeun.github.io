---
layout: post
title: "Mujoco-200-tutorials-Lec1:Working with XML files"
date: 2024-04-15 18:15 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

XML file을 이용하여 Mujoco에서 사용되는 entity인 **Model**을 정의해보자.

[lecture_url](https://www.youtube.com/watch?v=j1nCeqtfySQ&list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G&index=6)

```
<mujoco>
  <compiler angle="radian"/>
  <visual>
    <headlight ambient=".3 .3 .3" />
  </visual>
  <asset>
	<material name="white" rgba="1 1 1 1"/>
  </asset>
  <option gravity="0 0 -9.8">
    <!-- <flag gravity="enable"/> -->
  </option>

  <worldbody>
    <light diffuse=".3 .3 .3" pos="0 0 3" dir="0 0 -1"/>
    <geom type="plane" size="1 1 .1" rgba="0 .9 0 1"/>
	<body pos=".2 0 2.3">
      <joint type="free"/>
	  <inertial pos="0 0 0" mass="1" diaginertia=".01 .01 .01"/>
      <geom type="sphere" size=".1" rgba="0 0 1 1"/>
    </body>
	<body pos="0 0 2" euler="0 -1.57 0">
      <joint type="free"/>
	  <inertial pos="0 0 0" mass="1" diaginertia=".01 .01 .01"/>
      <geom type="box" size=".1 .2 .3" rgba=".9 0 0 1"/>
    </body>
    <body pos="0 0 1" euler="0 0 0">
      <joint type="free"/>
	  <inertial pos="0 0 0" mass="1" diaginertia=".01 .01 .01"/>
	  <geom type="box" size=".1 .2 .3" material="white"/>
    </body>

  </worldbody>
</mujoco>
```
