---
layout: post
title: markdown_script
date: 2024-05-03 00:39 +0900
author: songdaegeun
categories: ["miscellaneous"]
tags:
pin: false
math: true
---

포스팅 시, 주로 사용하는 markdown 구문 정리 

## Lists

### Ordered list

1. 1st item
2. 2nd item
3. 3rd item

### Unordered list

- Chapter
  + Section
    * Paragraph

### ToDo list

- [ ] Job
  - [x] Step 1
  - [x] Step 2
  - [ ] Step 3

### Description list

Sun
: the star around which the earth orbits

Moon
: the natural satellite of the earth, visible by reflected light from the sun

## Block Quote

> This line shows the _block quote_.

> An example showing the `tip` type prompt.
{: .prompt-tip }

> An example showing the `info` type prompt.
{: .prompt-info }

> An example showing the `warning` type prompt.
{: .prompt-warning }

> An example showing the `danger` type prompt.
{: .prompt-danger }


## Mathematics

Add post header **`math: true`**.

```yaml
---
title: TITLE
...
math: true
---
```
Grammar is similar with LaTex. 

### 줄바꿈 (테이블 내에서 줄바꿈할 때 유용)
```
<br>
```

### 일반 텍스트에서 줄바꿈

```
1. 문장 끝에 공백 2개 이상
2. \n바로 앞에 \을 한번 더 붙히기
```
### 블럭 인용구

```
>, >>, >>>, ..
```

### 링크
```
기본 링크 : [홈으로](https://www.irgroup.org/)
새창으로 : [홈으로 새창](https://www.irgroup.org/){:target="_blank"}
```

### 이미지

```
기본 형식 : ![설명](링크)
속성 부여 : ![설명](링크){:style="border:1px solid #eaeaea; border-radius: 7px; padding: 0px;" }
```
_참고 : `{}` 사이에 html attribute를 마음대로 넣을 수 있다_

### 참고문서

[Chripy docs](https://chirpy.cotes.page/)\
[kramdown syntax](https://kramdown.gettalong.org/syntax.html){:target="_blank"}\
[kramdown 사용법](http://gjchoi.github.io/env/Kramdown%28%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4%29-%EC%82%AC%EC%9A%A9%EB%B2%95/){:target="_blank"}\
[Front Matter](https://jekyllrb.com/docs/front-matter/){:target="_blank"}
