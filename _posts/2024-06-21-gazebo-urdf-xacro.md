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
URDF는 XML 기반의 파일 형식으로, 로봇 모델의 물리적 구성 요소를 정의하는 데 사용된다.

#### .xacro  
[urdf_XML_](https://wiki.ros.org/urdf/XML)  
Xacro는 XML을 확장한 매크로 언어로, 복잡한 URDF 파일을 더 효율적으로 작성하고 관리할 수 있게 해준다.  
매크로와 변수, 조건문을 사용하여 URDF 파일의 재사용성과 가독성을 높인다.  
ros1에서는 재사용성을 높이기위해 urdf에서 확장된 xacro로 모델을 주로 정의한다.  

#### sdf(SDFormat)
[SDFormat extensions to URDF (the \<gazebo\> tag)](http://sdformat.org/tutorials?tut=sdformat_urdf_extensions&cat=specification&)  
[sdformat specification](http://sdformat.org/spec?ver=1.11&elem=collision#link_collision)  
```
urdf외에 SDF를 사용할 수도 있다.  
URDF는 Gazebo에 의해 처리되기 전에 먼저 SDFormat으로 변환된다.  
gazebo classic은 urdf의 확장으로 SDFormat를 사용할 수 있고,  
gazebo ignition은 SDFormat를 모델정의의 기본으로 사용한다.  
```

#### step1 -> drift issue
[urdf_XML_link](https://wiki.ros.org/urdf/XML/link)  
[Using a URDF in Gazebo](https://classic.gazebosim.org/tutorials?tut=ros_urdf&cat=connect_ros)  
[SDFormat extensions to URDF (the \<gazebo\> tag)](http://sdformat.org/tutorials?tut=sdformat_urdf_extensions&cat=specification&)  
[sdformat specification](http://sdformat.org/spec?ver=1.11&elem=collision#link_collision)  

0. dont_collapse제거
urdf_XML_link를 참조하면, link 태그의 속성에는 dont_collapse가 없다.  
dont_collapse는 issac sim의 urdf에서 사용되는 문법인 것같다.  

before
```
<joint name="RBJ_foot" type="fixed" dont_collapse="true">
```
dont_collapse를 제거한다.  
after
```
<joint name="RBJ_foot" type="fixed">
```

1. collision sdf element 문법
Gazebo에 의해 처리되기 전에, URDF는 SDFormat으로 conversion된다.  
따라서 urdf파일은 urdf_XML문법 외에도 sdformat문법을 사용할 수 있다.  

conversion과정에서 issue를 진단하기 위해서 다음과 같은 명령을 사용했다.  
```
# 문제있음
gz sdf -p aidin8_arm_cover_masszero.xacro
# 문제없음
gz sdf -p base.world
```






urdf에서 surface, friction에 대한 ode속성은 변경할 수 있지만, simulation전반에 적용되는 ode자체의 iter속성을 바꿀수는 없는거야? surface, friction에 대한 ode는 어떤 방정식을 풀때의 속성이니?




before
```
<collision name="RF_foot_col1">
  <origin rpy="0  0  0" xyz="0.011  -0.007  0"/>
  <geometry>
    <sphere radius="0.04"/>
  </geometry>
  <contact>
    <lateral_friction value="1.0" />
    <rolling_friction value="0.001"/>
    <torsional_friction value="0.001"/>
    <contact_cfm value="0.0"/>
    <contact_erp value="0.3"/>
    <ode>
      <mu>1.0</mu>
      <mu2>1.0</mu2>
      <minDepth>0.0002</minDepth>
      <kp  value="1000000.0"/>
      <kd  value="500.0"/>
    </ode>
  </contact>
</collision>
```
현재 sdformat element들(contact_cfm, mu->mu1 등)의 사용이 잘못되었다.  
다음과 같이 고친다.  
after
```
<collision name="RF_foot_col1">
  <origin rpy="0  0  0" xyz="0.011  -0.007  0"/>
  <geometry>
    <sphere radius="0.04"/>
  </geometry>
  <surface>
    <contact>
      <ode>
        <soft_cfm value="0.0"/>
        <soft_erp value="0.3"/>
        <kp value="1000.0"/>
        <kd value="500.0"/>
        <min_depth>0.0002</min_depth>
      </ode>
    </contact>
    <friction>
      <torsional>
        <coefficient>0.001</coefficient> 
      </torsional>
      <ode>
        <mu>10000.0</mu>                
        <mu2>0.001</mu2>           
      </ode>
    </friction>
  </surface>
</collision>
```

```
개선없음.
lateral friction coef.(mu)가 10000인데도 미끌리는것처럼보이는걸로보아,  
RF_foot과 ground_plane의 contact빈도가 매우 높을 것으로 예상된다.   
```
2. inertial에 pose주기
inertial에 pose를 안주니까 미끄러지는 것을 확인했다.(3_two_link_drift.mov)  
aidin8_arm_cover_masszero에도 줬지만 여전히 미끄러진다.  

3. foot의 형상을 sphere -> box
뒤로 미끌리다가 발의 형상때문에 point contact의 방향이 앞이라서 제자리를 유지한다.  
하지만 지면과 접촉시 foot에 모멘트가 크게 발생해서 sphere가 아닌 형태의 발은 좋지 않다.  
또 foot을 deformable하게 만들면 contact force에 noise가 발생한다.  

4. RF_foot과 ground_plane의 contact빈도확인
[empty.world](https://github.com/arpg/Gazebo/blob/master/worlds/empty.world)  
[ground_plane](https://github.com/arpg/Gazebo/blob/master/models/ground_plane/model.sdf)  
[contact manager API](https://osrf-distributions.s3.amazonaws.com/gazebo/api/2.0.0/classgazebo_1_1physics_1_1ContactManager.html)  
[gazebo_all Physics Engines and physics parameter](https://classic.gazebosim.org/tutorials?tut=physics_params&cat=physics#Contactparameters)  
[sdformat extension to URDF](http://sdformat.org/tutorials?tut=sdformat_urdf_extensions&cat=specification&)  
[URDF tag in Gazebo](https://classic.gazebosim.org/tutorials?tut=ros_urdf&cat=connect_ros)  

gazebo_physics parameter 링크를 참고하면 알겠지만, gazebo_physics parameter를 world파일에서 정의하는거나 urdf파일에 sdf element를 사용하는거나 같다.  

```
std::cout << "# of contact: " << contactManager->GetContactCount() << std::endl;
```
RF_foot의 컨택을 확인하고 싶은데, RF_foot은 커녕 위 명령의 결과가 0이 나와서  
contact중인 모델이 아예 없다는 이상한 결론이 나왔다.  
-> gz client에서 view-contact을 선택하면 contact정보를 다음의 명령으로 확인할 수 있다.  

```
gz topic -l
gz topic -e {ex. /gazebo/default/box/link/my_contact}
```

또 이건 내가 contactManager api를 이용해서 debug용으로 출력함.  
```
# of contact: 1
RB_knee
# of contact: 4
RF_knee
RB_knee
LF_knee
LB_knee
# of contact: 1
RB_knee
# of contact: 3
```

gz client에서 시각적으로 보이고, debug 출력에서 볼 수 있듯이, contact point가 변동되고 있다(flickering contact point).  

5. flickering contact point
[flickering contact point](https://github.com/osrf/collision-benchmark/issues/6)  
[유사한사례](https://robotics.stackexchange.com/questions/111332/issues-with-legs-vibrating-and-contact-flickering-in-gazebo-ros-noeitc)  

```
하지만 이것은 drift와 상관이 있는 것 같지는 않다.
```

6. gz client 비정상종료 이슈

```
[INFO] [1720161662.229288, 0.000000]: temporary hack to **fix** the -J joint position option (issue #93), sleeping for 1 second to avoid race condition.
```
ps aux로 확인중에 왜인지모르게 해결되었다.  

7. foot에 대한 최대 contact parameter찾기
```
<gazebo reference="RF_foot">
  <mu1>5.0</mu1>
  <mu2>5.0</mu2>
  <cfm>01e-4</cfm>
  <minDepth>0.0002</minDepth>
  <kp  value="100.0"/>
  <kd  value="100000.0"/>
</gazebo>
```

```
gz client-physics-gravity를 -9.81 -> -3까지 줄여봤는데 drift는 무게랑 상관없음을 알 수 있었다.    
```

#### 물리엔진 관련 이슈
1. Step 1. Start initializing encoder에서 제자리에 서기
max_step_size를 "0.001->0.0005"로 변경하니, 
제자리에 설때 "빠르게 발구르기 -> 안정적으로서기" 로 변했다.  


2. sdf: physics-ode-solver-sor
sor: Set the successive over-relaxation parameter.
Successive Over-Relaxation method(연속과이완기법)에 사용되는 parameter이다. 
솔버의 수렴 속도를 조절하는 데 사용되며, 1보다 크거나 같아야한다.   

3. contact_surface_layer
Description: The depth of the surface layer around all geometry objects. Contacts are allowed to sink into the surface layer up to the given depth before coming to rest. The default value is zero. Increasing this to some small value (e.g. 0.001) **can help prevent jittering problems** due to contacts being repeatedly made and broken.
0.001 -> 0.0001 : 시작하자마자 과도한 컨택때문에 로봇이 날라간다.  




#### .launch
[roslaunch](https://wiki.ros.org/roslaunch)  
[roslaunch/XML](https://wiki.ros.org/roslaunch/XML)  

```
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
```
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
ex.
```
<group ns="$(arg robot_namespace)">
</group>
```

- remap
node 태그 시작 전에 remap을 통해 topic을 redirection할 수 있다.  
즉, remap이후에 redirection된 topic을 사용하는 node는 원래 topic이 아닌 새로운 topic에 대해 통신하는 것으로 간주된다.  
용례는 다음과 같다.  
```
<remap from={원래 topic} to={새로운 topic} />
```

- node 
Launch a node.  
roslaunch는 노드가 어떤 순서로 시작하는지에 대한 보장을 제공하지 않는다.  
ex.
```
<node name="robot_state_publisher" pkg="robot_state_publisher" type="robot_state_publisher">
  <param name="publish_frequency" value="1000.0"/>
</node>
```

- param
Set a parameter on the Parameter Server.  
Instead of value, you can specify a textfile, binfile or command attribute to set the value of a parameter.  
<param> 태그는 <node> 태그 안에 넣을 수 있으며, 이 경우 private parameter로 처리된다.
```
<param name="robot_description" command="$(find xacro)/xacro --inorder '$(find aidin8_sim)/urdf/aidin8_arm_cover_masszero.xacro'" />
<param name="tf_prefix" value="$(arg robot_namespace)" /> 
```

private parameter
: private name을 가진 parameter

Graph Resource Names convention
: There are four types of Graph Resource Names in ROS: base, relative, global, and private.
: 1. 기본적으로, 이름은 namespace에 relative하다.  
: 2. namespace가 없으면 base name이다.  
: 3. "/"로 시작하는 name은 global name이다. 전역이름은 code portability를 제한시키기때문에 가급적 피한다.  
: 4. "~"로 시작하는 name은 private name이다. namespace를 포함하여 node의 이름 뒤에 ~이하가 추가된 형식으로 변환된다.  
Node types은 노드의 실행파일명이다.  
messege type/service type은 각 messege, service형식을 명시한 파일이름이다.  

Graph Resource
: ROS1에서, 노드(Node) 간의 통신 및 데이터 흐름을 ROS Computation Graph라고하며 여기에 사용되는 node, parameter, messege등의 자원을 Graph Resource라고 한다.
ex.  
```
<param name="robot_description" command="$(find xacro)/xacro --inorder '$(find aidin8_sim)/urdf/aidin8_arm_cover_masszero.xacro'" />
<param name="tf_prefix" value="$(arg robot_namespace)" /> 
```

- rosparam
YAML 파일 형식의 여러 parameter가 정의된 rosparam 파일을 dump, load(parameter server로)할 수 있다.  
parameter를 param. server에서 삭제할 수도 있다. 
ex.
```
<rosparam file="$(find aidin8_sim)/config/aidinarm_control.yaml" command="load"/>
```

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
ex.  
```
<include file="$(find gazebo_ros)/launch/empty_world.launch">
    <arg name="paused" value="true"/>
</include>
```

- arg
The <arg> tag allows you to create more re-usable and configurable launch files by specifying values that are passed via the command-line, passing in via an <include>, or declared for higher-level files. **Args are not global**. 
```
<arg name="init_pose" default="-x 0.0 -y 0.0 -z 0.0"/>
```

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

#### robot_state_publisher package
[robot_state_publisher docs](https://wiki.ros.org/robot_state_publisher)  
이 패키지는 joint_states 토픽을 subscribe하여 TF frame정보로 변환하고 tf2_ros package에 전달한다.  


#### tf package
[TF docs](https://wiki.ros.org/tf2)  
로봇 시스템은 일반적으로 world frame, base frame, gripper frame 등과 같이 시간에 따라 변하는 3D coordinate frame들을 갖는다.  
tf2는 시간에 따른 이러한 모든 frame을 추적하고 다음과 같은 질문에 답한다.  
- 5초 전, world frame 기준으로 head frame은 어디에 있었나요?
- base를 기준으로 그리퍼에 있는 물체의 pose는 어떻습니까?
- map frame에서 base frame의 현재 pose는 무엇입니까?

크게 다음 두 가지 작업을 수행한다.  
Broadcasting transforms  
: 노드가 자신과 관련된 좌표 변환 정보를 다른 노드들에게 전송한다.  
Listening for transforms  
: 다른 노드들이 broadcast한 변환 정보를 수신하고 이를 이용하여 좌표 변환을 수행한다.  

ex. 
```
# 현재 frame들의 관계를 tree로 구성하여 pdf파일로 저장
rosrun tf2_tools view_frames.py
# reference frame에 대한 target frame의 pose를 출력
rosrun tf tf_echo {ref. frame} {target frame}
```

#### gazebo_ros package
[gazebo_ros_pkgs(of gazebo classic) docs](https://wiki.ros.org/simulator_gazebo/Tutorials)  
[gazebo_ros package source code](https://github.com/ros-simulation/gazebo_ros_pkgs/blob/noetic-devel/gazebo_ros)  

- node: spawn_robot  
[spawn_model source code](https://github.com/ros-simulation/gazebo_ros_pkgs/blob/noetic-devel/gazebo_ros/scripts/spawn_model)  

Gazebo 시뮬레이션 환경에 로봇 모델을 spawn하는 데 사용되는 노드이다.  
-unpause: 모델이 스폰된 후 시뮬레이션을 즉시 재개한다.  
-J: revolute joint의 초기값(radian)을 설정한다.  

```
<node name="spawn_robot" pkg="gazebo_ros" type="spawn_model"
args="$(arg init_pose) -urdf -model $(arg robot_namespace) -param robot_description
      -J arm_joint2 -2
      -J arm_joint3 2
      -J arm_joint5 0.5
      -unpause"
output="screen"/>
```

#### ros control scheme(gazebo_ros_control pkg, controller manager pkg)
[controller_manager docs](https://wiki.ros.org/controller_manager)  
[ros control docs_1](https://wiki.ros.org/ros_control)  
[ros control docs_2](https://classic.gazebosim.org/tutorials?tut=ros_control)  
[ex.rrbot roscontrol](https://github.com/ros-simulation/gazebo_ros_demos)  

![ros control overview](/assets/img/개념/2024-06-21-gazebo-urdf-xacro/ros%20control%20overview.png){: width="200"}  
![ros control overview2](/assets/img/개념/2024-06-21-gazebo-urdf-xacro/ros%20control%20overview_2.png){: width="200"}  

```
gazebo_ros_control pkg를 통해, gazebo에서 robot의 controllers를 simulating할 수 있다.  
위 그림은 simulation, hardware, controllers and transmissions사이의 관계와, data flow를 나타냈다.  
예를들면 다음과 같은 형식이다.  
- The contoller manager는 robot's actuator's encoders로부터 joint state data와 set point를 input으로 받는다.  
- 이는 hardware_interface::RobotHW instance인 contoller manager에 ~ 으로 정의된 transmission을 통해 이루어진다.  
- PID같은 포괄적인 control loop feedback mechanism을 사용하여 control한다. 
- controller의 output은 다시 contoller manager를 통해 actuator로 전달된다.  

gazebo_ros_control pkg는 controller manager, Gazebo plugin adapter, libgazebo_ros_control.dylib등을 포함한다.  
```

0. gazebo_ros_control pkg
gazebo_ros_control pkg의 일부인 libgazebo_ros_control.dylib은 gazebo classic을 설치할 때 함께 설치된다.  
controller manager pkg를 포함하고 있다.  

gazebo_ros_control pkg을 사용하려면 다음 두가지를 수행해야한다.  

0.1. gazebo_ros pkg 정의
다음과 같이 gazebo_ros pkg에 정의된 gazebo ros control 플러그인을 load한다.  
ex.  
```
<node name="spawn_robot" pkg="gazebo_ros" type="spawn_model"
  args="$(arg init_pose) -urdf -model $(arg robot_namespace) -param robot_description
        -J arm_joint2 -2
        -J arm_joint3 2
        -J arm_joint5 0.5
        -unpause"
  output="screen"/>
```
0.2. libgazebo_ros_control.dylib 로드
다음과 같이 urdf/xacro에서 libgazebo_ros_control 동적라이브러리(gazebo에서 plugin이라고 부름)를 load한다.  
ex. 
```
<!-- ROS control plugin -->
<gazebo>
  <plugin name="gazebo_ros_control" filename="libgazebo_ros_control.dylib">
    <robotNamespace>/aidin81</robotNamespace>
    <robotSimType>gazebo_ros_control/DefaultRobotHWSim</robotSimType>
  </plugin>
</gazebo>
```

1. controller manager  
```
hardware_interface::RobotHW instance로 표현되는, robot mechanism을 control하기위한 hard-realtime-compatible loop를 제공한다.  
이는 simulator(ex. gazebo), planner(ex. moveIt)를 위한 ROS interface이다.  
구체적으로는 controller를 load, unload, start, stop하기위한 infrastructure를 제공한다.  
rqt 플러그인인 rqt_controller_manager로 컨트롤러를 그래픽으로 load, unload, start, stop할 수 있을 뿐만 아니라, load된 컨트롤러에 대한 정보를 표시할 수 있다.  
controller manager packge를 실행하려면 다음과 같이 launch파일에서 controller_manager pkg에 포함된 controller_spawner node를 정의한다.  
```
ex.  
if you just want to load the controller, but not start it yet:
```
 <launch>
   <node pkg="controller_manager"
         type="spawner"
         args="--stopped controller_name1 controller_name2" />
 </launch>
```

2. plugin: JointStateController, JointEffortController  
```
controller manager의 관리를 받는 ros_controllers에 포함된  
controller plugin(gazebo에서 공유라이브러리를 plugin이라고 부름)이다.  
```

2.1. JointStateController
```
hardware_interface::JointStateInterface에 register된 모든 resource들의 state를  
sensor_msgs/JointState 타입의 topic으로 publish한다. 
```
2.2. JointEffortController
```
desired effort(force/torque)를 Hardware Interface에 Command한다.  
effort input을 받고 effort output을 보낸다. 단지 forward_command_controller의 input으로 전달한다.  
```
2.3. hardware_interface::RobotHW(ex. hardware_interface::EffortJointInterface)  
```
실제 로봇의 드라이버에 joint limit, effort transmission, (forward) state transmission등의 명령에 관련된 데이터를 읽고 쓰기 위해서는, hardware_interface::RobotHW를 상속받거나 controler manager에 새로운 hardware_interface를 정의해야한다.  
Hardware Interface의 구현체는 기본적으로 read(), write(), init() 메소드를 구현하여 드라이버에 데이터를 읽고 쓰는 과정을 정의한다.  
hardware_interface를 상속받는 구현체가 로봇 제조사나 커뮤니티에서 패키지로 제공되는 경우(ex. ur_modern_driver) 그걸 사용해야하고, 없으면 정의해야한다. 
```
2.4. example  

```
// .launch
<node name="controller_spawner" pkg="controller_manager" type="spawner" respawn="false"
output="screen" ns="/aidin81" args="joint_state_controller
          joint1_effort_controller
          joint2_effort_controller
          joint3_effort_controller
          joint4_effort_controller
          joint5_effort_controller
          joint6_effort_controller"/>
```

controller를 사용하려면, 다음과 같이 ros parameter server에 load할 controller 설정을 정의해야한다.  
```
// aidinarm_control.yaml
joint_state_controller:
  type: joint_state_controller/JointStateController
  publish_rate: 1000  # 50
  
joint1_effort_controller:
  type: effort_controllers/JointEffortController
  joint: arm_joint1
  pid: {p: 100.0, i: 0.01, d: 10.0}
```
이를 launch에서 Load한다.  
```
<rosparam file="$(find aidin8_sim)/config/aidinarm_control.yaml" command="load"/>
```

```
parameter file내에 보이는 joint1_effort_controller 필드는  
controller_manager에 인자로 전달된 controller name이다.  
type은 effort_controllers plugin에 미리 정의된, namespace인 effort_controllers과 매크로변수 JointEffortController(HW interface를 값으로 가짐)이다.  
즉, type을 통해 controller manager에 정의된 HW interface를 결정한다.  
pid는 plugin에 전달되는 pid gain이 명시된 인자이다.  
```


#### joy package
[joy docs](https://wiki.ros.org/joy)  

#### gazebo client(GUI)
[OBB와 AABB](https://handhp1.tistory.com/6)  
UI상의 왼쪽 패널의 model들을 눌렀을 때 model주변에 나타나는 흰색 wireframe box는 AABB(Axis-Aligned Bounding Box)이다.  
AABB는 3D world에서 교차검사(충돌검출이나 타격판정등을 위한) bounding box이다. urdf에서 정의하는 collision 영역이랑은 무관하다.  

#### .world
모델을 추가하고, 센서, 조명, 지형 등의 환경을 정의하는데 사용된다.  
XML 기반의 파일형식이다.  

#### .gazebo 
