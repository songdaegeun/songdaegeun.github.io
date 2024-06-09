---
layout: post
title: ros1_cheat_sheet
date: 2024-06-09 20:32 +0900
author: songdaegeun
categories: ["ros1", "noetic"]
tags: ["ros1", "noetic"]
pin: false
math: true
---

ROS1은 shell environment를 사용하는 combining spaces의 개념에 의존하기 때문에,  
shell environment가 제대로 setup되는 것이 중요하다.  
다음 명령으로 ROS_PACKAGE_PATH, ROS_ROOT 등 유용한 변수들을 확인할 수 있다.  
```
printenv | grep ROS
```

ws의 src에서 catkin_make할 때마다 다음의 명령을 실행해야한다.(zsh기준일때)
```
source devel/setup.zsh
```

다음은 ROS_PACKAGE_PATH내에 존재하는 package에 대한 명령들이다.
```
# package 위치출력
rospack find [package_name]
# package/stack(/subdir)으로 cd
roscd <package-or-stack>[/subdir]
# package/stack(/subdir)에 대해 ls
rosls <package-or-stack>[/subdir]
```

package의 dependency 출력
``` 
# first dependency
rospack depends1 rospy
# all nested dependency
rospack depends beginner_tutorials

```
package를 생성하는 방법
```
catkin_create_pkg <package_name> [depend1] [depend2] [depend3]
```
이후 dependency관련은 package.xml에서, build관련은 CMakeLists.txt에서 조정한다.  

package를 빌드하는 방법
```
# package가 정의(CMakeLists.txt, [package name])된 directory가 'src'(default)인 경우
catkin_make
# package가 정의(CMakeLists.txt, [package name])된 directory가 'my_src'인 경우
catkin_make --source my_src
# CMake project 빌드 내부과정에 "make install"이 포함된 경우
catkin_make install  # (optionally)
```

node

```

```