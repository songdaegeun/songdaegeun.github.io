---
layout: post
title: 1_ros1_in_m1_mac
date: 2024-06-09 15:29 +0900
author: songdaegeun
categories:
tags: ["ros1", "noetic"]
pin: false
math: true
---

내 환경: m1, macos 13.3
목표: docker내에서 noetic실행 (패키지 실행시 host display에 출력까지)

#### 결론 

docker내에서 x11 server에 TCP socket통신까지는 성공했지만,  
turtlesim을 실행할 때 segmentation fault가 떳고, docker-for-mac이 m1 칩(arm64)을 지원하는 qemu 버전을 지원하지 않는 것으로 판단하고 포기했다.  

다음 포스팅에서는 pre build된 conda-forge를 사용해서 native한 m1 macos 환경에서 다시 noetic설치를 시도해볼 것이다.
[conda-forge추천](https://discourse.ros.org/t/ros-gui-on-macbook-air-m1-chip/27205/6)

#### 설치과정(mac기준)

```
# XQuartz실행
open -a XQuartz
# localhost를 x11 server(xquartz)에 접근가능한 host목록(xhost)에 추가한다.
xhost +localhost
# xhost확인 
xhost
```

```
./run_for_mac.sh
```
run_for_mac.sh 파일 내용은 다음과 같다.  

```
docker run -it \
    --name="noetic_mac" \
	--env="XDG_RUNTIME_DIR=/tmp/runtime-root" \
    --env="DISPLAY=host.docker.internal:0" \
    --env="QT_X11_NO_MITSHM=1" \
    --volume="/Users/sdg/Documents/project/docker/volume/osrf_ros_noetic_desktop_full:/mnt:rw" \
    osrf/ros:noetic-desktop-full
export containerId=$(docker ps -l -q)
```

```
docker exec -it noetic /bin/bash
```

```
apt update; apt install x11-apps -y
# 그래픽출력확인
xclock
apt install vim ros-noetic-ros-tutorials -y
roscore
```
아래를 실행했는데,
```
rosrun turtlesim turtlesim_node
```
다음과 같은 에러와 함께 Segmentation fault가 떳다.  
```
qemu: uncaught target signal 11 (Segmentation fault) - core dumped
```
turtlesim_node실행 시 linux/amd64 구조로 qemu emulator가 실행되는데, 
docker-for-mac이 m1 칩(arm64)을 지원하는 qemu 버전을 지원하지 않는 것 같다. 

#### 설치과정(window기준)

mac과 비교하여 변경사항만 설명하겠다.  

run_for_win.sh 파일 내용은 다음과 같다.  

```
docker run -it \
    --name="noetic_win" \
    --env="DISPLAY" \
    --env="QT_X11_NO_MITSHM=1" \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    --volume="/Users/sdg/Documents/project/docker/volume/osrf_ros_noetic_desktop_full:/mnt:rw" \
    osrf/ros:noetic-desktop-full
export containerId=$(docker ps -l -q)
```
```
xhost +local:docker
```

#### 에러

1. docker x11 socket 접근실패

docker container가 에 위치한 x11 socket file에 접근하지 못하여 다음과 같은 오류가 발생했다.
```
root@4e2226201d31:/# rosrun turtlesim turtlesim_node
qt.qpa.xcb: could not connect to display /private/tmp/com.apple.launchd.9rUPX3OIpD/org.xquartz:0
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: eglfs, linuxfb, minimal, minimalegl, offscreen, vnc, xcb.

qemu: uncaught target signal 6 (Aborted) - core dumped
Aborted
```

1.1. window
해결하려면 docker container실행 전에 host에서 다음의 명령으로 docker를 xhost에 추가해준다.  
xhost
이는 host에서 실행되는 모든 docker container(일종의 x11 client)가 host의 x11 server(x11 socker file)에 접근할 수 있음을 의미한다.  
```
xhost +local:docker
```

1.2. macos
mac에서는 Docker의 파일 시스템 드라이버 구현문제로 Unix 스타일 소켓이 작동하지않는다.  
그래서 Unix 도메인 소켓 대신 XSystem 통신에 TCP 소켓을 사용한다.  
따라서 run_for_mac.sh파일에서 DISPLAY 환경변수는 다음과 같이 대입하여 전달한다.  
```
env="DISPLAY=host.docker.internal:0"
```

1.3. 그래픽출력확인
성공적으로 remote에서 실행한 그래픽이 host의 display에 출력되는지 확인한다.  
```
apt install x11-apps
xclock
```
2. QStandardPaths: XDG_RUNTIME_DIR not set, defaulting to '/tmp/runtime-root'

XDG_RUNTIME_DIR는 XDG (X Desktop Group) 기반의 환경 변수 중 하나로, 프로세스간 통신을 위해  
임시 파일이나 소켓 파일과 같은 런타임 데이터를 공유하는 데 사용된다.  
그래서 이 디렉토리는 사용자의 세션 중에만 존재한다. 그래서 난 임의로 /tmp/runtime-root를 만들어 지정하였고,  
run_for_mac.sh 파일 내용에 다음을 추가하였다.

```
	--env="XDG_RUNTIME_DIR=/tmp/runtime-root" \
```

3. rviz출력실패
GPU 컨텍스트를 요청하는 앱(예: ROS용 rviz)에서는 이 TCP 소켓통신으로 그래픽을 출력할 수 없다..  
(nvidia-docker2 사용하면 될려나? [참고](https://d-ontory.tistory.com/8))


#### 참고자료
[설치과정](https://www.cv-learn.com/20210912-ros-tutorials-1/)  
[window-DISPLAY 환경변수 및 xhost 설정](https://blog.naver.com/occidere/221133121595)  
[xhost man](https://linux.die.net/man/1/xhost)  
[macos-DISPLAY 환경변수 및 xquartz 설치](http://mamykin.com/posts/running-x-apps-on-mac-with-docker/)  
[에러2](https://stackoverflow.com/questions/59790350/qstandardpaths-xdg-runtime-dir-not-set-defaulting-to-tmp-runtime-aadithyasb)
