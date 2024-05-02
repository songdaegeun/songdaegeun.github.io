---
layout: post
title: 'mujoco-200-tutorials-Lec11: Initial Value Problem using optimization'
date: 2024-04-23 16:40 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

[lecture_url](https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G)\
[mujoco docs](https://mujoco.readthedocs.io/en/stable/APIreference/APIfunctions.html)\
[NLopt docs](https://nlopt.readthedocs.io/en/latest/NLopt_Reference/)

optimization을 이용해서 Initial Value Problem을 해결해보자.

### Full code

코드를 돌려보면, v=54.737026, theta=0.384417 time_of_flight=0.100000으로 최적의 제어변수를 얻을 수 있다.

[test.c](/assets/Mujoco-200-tutorials/lec11/test.c)\
[constrained.cc](/assets/Mujoco-200-tutorials/lec11/constrained.c)\
[test.xml](/assets/Mujoco-200-tutorials/lec11/test.xml)

### 문제상황

![problem.png](/assets/Mujoco-200-tutorials/lec11/problem.png){: width="300"}


### optimal condition

![optimal_condition](/assets/Mujoco-200-tutorials/lec11/optimal_condition.png){: width="300"}

이 조건은 boundary와 equality constraint만 존재한다.\
+\) lower boundary가 0이면 적분시 에러가 발생하기 때문에 0.1로 둔다.\
+\) time_of_flight는 theta와 v, x_end, z_end로 이루어진 운동방정식에 의해 결정되지만, cost함수가 되어 더 빠른 궤적을 찾게한다.
