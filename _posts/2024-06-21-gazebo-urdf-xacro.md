---
layout: post
title: gazebo_urdf_xacro
date: 2024-06-21 16:19 +0900
author: songdaegeun
categories: ["gazebo", "urdf", "xacro"]
tags: ["gazebo", "urdf", "xacro"]
pin: false
math: true
---

본 post는 aidin_arm package를 이용할 때, gazebo simulator상에 load되는 model의 구조를 이해하기 위해 작성되었다.

#### gazebo 
[gazebo_tutorial](https://classic.gazebosim.org/tutorials)  

gazebo는 3D dynamic simulator이다.  
주로 ROS와 함께 사용된다.  

특징
- ODE, Bullet, Simbody 및 DART 물리 엔진
- 풍부한 로봇 모델 및 환경 라이브러리
- 다양한 센서

기능
- Gazebo 모델에 대한 3D 메시를 생성하기 위해 2D 이미지인 SVG(Scalable Vector Graphics)파일을 extrude.
- Model Editor
- Building Editor
- Logging and playback

#### ignition  
Ignition 시뮬레이터는 gazebo를 기반으로 한 시뮬레이션 프레임워크이다.  
gazebo의 차세대 버전이라고 할 수 있다.  
모듈형 아키텍처로 구성되었다. 물리엔진 모듈, 렌더링 모듈, GUI모듈 등이 있다.  
물리 플러그인 프레임워크를 사용하여 물리엔진들의 종속성 문제를 해결한다.  
각 물리 엔진은 플러그인을 통해 Ignition Gazebo에 인터페이스되므로 각 엔진을 지원하여 시뮬레이터를 컴파일할 필요가 없다.  

#### urdf  
URDF는 XML 기반의 파일 형식으로, 로봇의 물리적 구성 요소를 정의하는 데 사용된다.

#### xacro  
Xacro는 XML을 확장한 매크로 언어로, 복잡한 URDF 파일을 더 효율적으로 작성하고 관리할 수 있게 해준다.  
매크로와 변수, 조건문을 사용하여 URDF 파일의 재사용성과 가독성을 높인다.  

