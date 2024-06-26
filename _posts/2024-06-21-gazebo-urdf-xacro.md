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

본 post는 aidin_arm package를 이용할 때, gazebo simulator상에 load된 모델을 제어하는데 필요한 제반사항을 이해하기 위해 작성되었다.  

#### gazebo classic
[gazebo tutorial](https://classic.gazebosim.org/tutorials)  
[gazebo classic source](https://github.com/gazebosim/gazebo-classic)  

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

#### 현재 gazebo(이전엔 ignition)
[gazebo source](https://github.com/gazebosim)  

Ignition 시뮬레이터는 gazebo를 기반으로 한 시뮬레이션 프레임워크이다.  
gazebo의 차세대 버전이라고 할 수 있다.  
모듈형 아키텍처로 구성되었다. 물리엔진 모듈, 렌더링 모듈, GUI모듈 등이 있다.  
물리 플러그인 프레임워크를 사용하여 물리엔진들의 종속성 문제를 해결한다.  
각 물리 엔진은 플러그인을 통해 Ignition Gazebo에 인터페이스되므로 각 엔진을 지원하여 시뮬레이터를 컴파일할 필요가 없다.  

특징

- IPC 통신을 위한 ZeroMQ와 데이터 직렬화, 역직렬화를 위한 Protobufs API를 결합하여 Ignition Transport이라는 메시지 전달 시스템을 사용
- Ignition Transport에는 Ignition Messages라는 표준 메시지 정의 세트를 사용
- Ignition Math
- SDF(or SDFormat. XML file format이다) file을 사용하여 모델 정의. SDF library는 이 파일을 parse한다.  

#### .urdf  
URDF는 XML 기반의 파일 형식으로, 로봇의 물리적 구성 요소를 정의하는 데 사용된다.

#### .xacro  
Xacro는 XML을 확장한 매크로 언어로, 복잡한 URDF 파일을 더 효율적으로 작성하고 관리할 수 있게 해준다.  
매크로와 변수, 조건문을 사용하여 URDF 파일의 재사용성과 가독성을 높인다.  

#### .launch
[roslaunch](https://wiki.ros.org/roslaunch)  
[roslaunch/XML](https://wiki.ros.org/roslaunch/XML)  
여러 노드(프로세스)를 한번에 제어할 때 사용한다. XML 기반의 파일형식이다.  
roslaunch/XML문서에서 각 태그의 attribute와 element를 확인할 수 있다.   

1. 로컬프로세스
launch파일에 별다른 설정이 없으면 node는 디폴트로 로컬프로세스로 취급된다.  
roslaunch는 popen을 사용하여 로컬 프로세스를 시작하고 POSIX 신호를 사용하여 로컬 프로세스를 종료한다.  
ROS1 architecture에서는 노드가 초기화되는 시기를 알 수 있는 방법이 없기 때문에,  
roslaunch는 노드 시작에 대한 특정 순서를 보장하지 않는다.  

2. 원격프로세스
node를 원격프로세스로 제어할 수도 있다.  
다른 컴퓨터의 노드를 제어하려면 네트워크 설정(ssh, TCP/IP 등)이 필요하다.  
(ex. ROS_MASTER_URI와 remote host인 ROS_IP의 ip주소를 설정해야한다.)  

기본구조는 다음과 같다.  

```
<launch>
  <!-- Node를 정의하는 예제 -->
  <node pkg="패키지명" type="노드이름" name="노드이름" output="화면">
    <!-- 매개변수를 정의하는 예제 -->
    <param name="매개변수이름" value="값"/>
  </node>
  
  <!-- 다른 launch 파일을 포함하는 예제 -->
  <include file="$(find 패키지명)/launch/다른_파일.launch">
    <arg name="매개변수이름" value="값"/>
  </include>
</launch>
```

3. 태그  
- group  
launch 태그와 기능이 동일하며, launch 파일 내부에서 실행되었기 때문에 단순히 태그 내부에 대한 컨테이너 역할을 한다.  
node들을 grouping하여 setting(launch파일 내에서 일반적으로 사용되는 모든 태그)을 한번에 적용할 수 있다.  
group에 ns를 부여할 수 있다.  

- remap
node 태그 시작 전에 remap을 통해 topic을 redirection할 수 있다.  
즉, remap이후에 redirection된 topic을 사용하는 node는 원래 topic이 아닌 새로운 topic에 대해 통신하는 것으로 간주된다.  
용례는 다음과 같다.  
```
<remap from={원래 topic} to={새로운 topic} />
```

- node 
Launch a node.

- param
Set a parameter on the Parameter Server.

- rosparam
YAML 파일 형식의 여러 parameter가 정의된 rosparam 파일을 dump, load(parameter server로)할 수 있다.  
parameter를 param. server에서 삭제할 수도 있다. 

- substitution args
launch를 시작하기 전에 parsing하여 실행된다.   
다음과 같은 substitution args들이 있다.  
$(dirname)
// 디렉토리 절대경로로 대체  
$(optenv ENVIRONMENT_VARIABLE)
// 환경 변수가 설정된 경우 해당 값을 대체
$(arg foo)
// argument의 값을 꺼내서 대체
$(eval <expression>) 
// 계산 후 대체
$(anon name)
// name은 고유 식별자이며 이를 기반으로 익명의 id를 생성한다.
$(find pkg) 
// particular machine의 위치를 명시하는 대신, ROS package의 상대경로를 찾아주는 syntax이다.  
$(env ENVIRONMENT_VARIABLE)
// 해당 syntax를 사용하여 include 태그와 함께 (shell에 이미 set된)environment variables 기반으로 launch file을 load할 수 있다.  

- include  
현재 launch파일에 다른 .launch 파일을 포함시킨다. 
include되는 file은 master 태그(deprecated)를 제외한 모든 내용이 포함된다.  
이름이 사용자 파일과 충돌하지 않도록 네임스페이스를 할당할 수 있다.  

- env  
machine이나 node를 위해 set되어야하는 environment variables를 정의한다.  
// NOTE: Values set using the <env> tag will not be seen by $(env ...), so the <env> tag cannot be used to parametrize launch files.

- machine  
ROS nodes를 실행할 수 있는 machine을 선언하는 태그이다. 모든 node를 local로 실행한다면 이 태그를 사용할 필요없다. 이 태그는 주로 remote machines에 대한 SSH와 환경변수 setting을 선언하기위해 사용된다. alias를 사용하여 runtime에 어떤 machine이 사용될 지 정의할 수 있다.  
이를 통해 여러 machine에 대해 동일한 node 정의를 재사용할 수 있다.  

- test
Launch a test node (see rostest).
아래와 같은 형식으로 test script를 실행할 수 있다.
```
<test test-name="test_my_node" pkg="my_package" type="test_my_node.py" />
```



#### .world
모델을 추가하고, 센서, 조명, 지형 등의 환경을 정의하는데 사용된다.  
XML 기반의 파일형식이다.  

#### .gazebo 
