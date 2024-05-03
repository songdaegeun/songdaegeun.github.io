---
layout: post
title: jekyll based blog customizing
date: 2024-05-03 23:52 +0900
author: songdaegeun
categories: ["jekyll"]
tags:
pin: false
math: true
---

왠만한 sass는 관리자도구로 태그 id찾아서 변경해주면 된다.  
특별한 변경이 필요한 경우, 다른 블로그를 참고하고, jekyll, liquid, js를 공부해서 구현하도록하자.  
[jekyll docs](https://jekyllrb-ko.github.io/docs/)

### 변경이 필요한 경우

1. default.html의 전반적인 layout변경
2. 블로그 전체 배경,
3. 특정 sidebar-nav-item에 특정 내용 특정 형식 구현

### 변경 예정 기능

1. 블로그의 좋은 구성에 참고할만한 아이디어가 있을 때 해보자.
bgm, blog bot, 프로젝트 시각화 기능 등..

2. 시간많고 뭔가 꾸미고 싶은 기능이 있을 때 해보자.

3.1. jupiter book으로 공부용 서브블로그 [참고](https://hiddenbeginner.github.io/study-notes/contents/intro.html)\
블로그 내부에 아예 공부용 서브블로그를 jupiter book기반으로 만든 분이 있는데, 공부를 전반적으로 정리해야할 때 만들어보자.

3.1. gitbook형식으로 특정 분야 정리하는 sidebar-nav-item [참고](https://hiddenbeginner.github.io/Deep-Reinforcement-Learnings/book/intro.html)\
특정 분야를 깊게 파서 정리해야할 때, gitbook형식으로 정리하자. 아마 한번에 정리는 어려울 것이고 2회독차부터 gitbook정리를 시작할 수 있을 것이다. 

3.1. categories sidebar-nav-item [참고](https://hiddenbeginner.github.io/)\
카테고리를 상단바에 두고, 각 카테고리를 클릭할 때 관련 post가 list up되는 레이아웃으로 변경 

3.2. archive sidebar-nav-item [참고](https://theorydb.github.io/category/dev/)\
연도별로 분류해서 시간순으로 나타내는 레이아웃으로 변경
