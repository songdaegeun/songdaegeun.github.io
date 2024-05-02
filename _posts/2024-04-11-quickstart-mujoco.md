---
layout: post
title: quickstart_mujoco-1.1.목표, 계획, reference
date: 2024-04-11 15:37 +0900
description:
author: rosi
categories: [mujoco]
tags:
---

## 목표

Mujoco에서 Task and Motion Planning

## Motivation

Study for Task and Motion Planning (for path, not for trajectory)
(monte carlo tree search in continuous spaces using Voronoi optimistic optimization with regret bounds - kaist 김범준 교수)
[url](https://www.youtube.com/watch?v=TOw4w9KJb9Q)

## 계획

- Mujoco api, mujoco xml, robosuite(mujoco_py는 deprecated) 사용방법숙지
- Kinematics
- Manipulator (Franka panda, Kuka, UR5e, Sawyer, etc..) 불러오기
- Collision Check
- Joint Position Control
- Cartesian Planning
- RRT\* Motion Planning
- Pick and Place 모듈 개발하기
- Task and Motion Planning 데모

## 향후 필요할 reference

- mujoco colab example <https://colab.research.google.com/github/google-deepmind/mujoco/blob/main/python/tutorial.ipynb#scrollTo=slhf39lGxvDI>
- robosuite <https://robosuite.ai/docs/quickstart.html>
- mujoco example (using python wrapper) <https://github.com/tayalmanan28/MuJoCo-Tutorial>
- mujoco example (c++) <https://pab47.github.io/mujoco.html>

## 참고

- mujoco 학습 roadmap 참고 <https://ropiens.tistory.com/168>
- MJCF 파일 분석(1) - 나만의 Manipulator task를 mujoco에서 만들자 <https://rlwithme.tistory.com/8>
- mujoco bootcamp <https://pab47.github.io/mujoco.html>
- MuJoCo 200 Tutorials <https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G>
- mujoco와 issac gym 시뮬레이터 사이의 Sim2Sim Gap <https://www.youtube.com/watch?v=0rgd-fuL-wc>
- ropiens (https://ropiens.tistory.com/)
- 미니멀공대생 (https://m.blog.naver.com/PostList.naver?blogId=nswve )
- 오로카 (https://cafe.naver.com/openrt/10561)
- install mujoco to work with openai-gym (https://neptune.ai/blog/installing-mujoco-to-work-with-openai-gym-environments)
- MuJoCo 200 Tutorials <https://www.youtube.com/playlist?list=PLc7bpbeTIk758Ad3fkSywdxHWpBh9PM0G>
- [Mujoco MPC](https://github.com/google-deepmind/mujoco_mpc)
