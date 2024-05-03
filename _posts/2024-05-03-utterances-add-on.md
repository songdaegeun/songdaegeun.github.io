---
layout: post
title: utterances add on
date: 2024-05-03 20:36 +0900
author: songdaegeun
categories: ["jekyll"]
tags: [utterances]
pin: false
math: true
---

utterances
> GitHub issues 기반으로 만들어진 lightweight comments widget이다. 블로그 post, wiki pages등에 comment를 달면, GitHub issues에 등록된다.


gitblog post하단에 comment를 입력할 수 있는 기능을 추가하기위해, open source인 utterances를 이용해보자!

### 저장소에 utteranc 설치

[utterances app]() 에서 install
only select repository를 선택 후 드롭다운에서 utterances를 적용할 내 gitblog repository선택


### snippet

아래와 같은 snippet을 _layouts/post.html 맨하단에 붙힌다.


```
<script src="https://utteranc.es/client.js"
	repo="songdaegeun/songdaegeun.github.io"
	issue-term="pathname"
	theme="github-dark"
	crossorigin="anonymous"
	async>
</script>

```
해당 코드는 [utteranc 웹사이트](https://utteranc.es/)에서 configuration란에 utteranc가 설치된 repository를 입력하면 생겨나고, 복사해왔다.    
- 원한다면 theme을 바꿀 수도 있다.(현재는 github dark)\
- .utterances와 .utterances-frame selector를 이용해서 layout을 customize할 수도 있다.  

### 결과 

post에 댓글을 달면, gitblog issue에도 등록되어 확인 및 답장할 수 있다.

### 참고문서

[설치과정](https://www.irgroup.org/posts/utternace-comments-system/)\
[utterances](https://utteranc.es/)