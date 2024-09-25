---
layout: post
title: Readthedocs_Sphinx
date: 2024-09-25 14:29 +0900
author: songdaegeun
categories: ["readthedocs"]
tags: ["readthedocs", "sphinx"]
pin: false
math: true
---

본 post는 git repository형태의 프로젝트 문서인 [quadruped-robot-docs](https://github.com/TopHillRobotics/quadruped-robot-docs.git)를 readthedocs로 빌드하는 과정을 정리하기 위해 작성되었다.   

readthedocs는 Sphinx(of MkDocs)를 사용하여 빌드를 수행할 수 있다. 따라서 Sphinx가 사용하는 conf.py를 이용해서 문서 빌드의 세부 사항을 결정한다.   

수행한 절차는 다음과 같다.


1. quadruped-robot-docs clone
2. [readthedocs dashboard](https://readthedocs.org/dashboard/)에서 빌드시도한다.
2. 한번 ~에서 빌드를 시도해본다. 실패했다.
3. repo에 .readthedocs.yaml가 없기때문에 [config. ref.](https://docs.readthedocs.io/en/stable/config-file/v2.html)에 나와있는
Sphinx 예제를 복사해서 {repo의 root directory}/.readthedocs.yaml에 붙혀넣는다. 
4. 빌드 과정에서 sphinx_rtd_theme가 없기때문에 실패했다.
5. requirements.txt를 만들고 sphinx_rtd_theme를 입력한다.
6. .readthedocs.yaml에 다음과 같은 내용을 attach한다.  
python:
  install:
    - requirements: docs/requirements.txt
7. 빌드성공


결과는 다음과 같이 readthedocs dashboard에서 내가 빌드한 프로젝트 문서인 [tophill-qr](https://readthedocs.org/projects/tophill-qr/)에서  
'문서보기'또는 https://tophill-qr.readthedocs.io/en/latest/와 같은 링크로 확인할 수 있다.  


---
### ref

[Sphinx 사용법정리블로그](https://wooiljeong.github.io/python/sphinx-quick-start/)  
[readthedocs tutorial](https://docs.readthedocs.io/en/stable/tutorial/)  
[readthedocs dashboard](https://readthedocs.org/dashboard/)



