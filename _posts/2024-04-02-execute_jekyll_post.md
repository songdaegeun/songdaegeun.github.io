---
layout: post
title: execute_jekyll_post
date: 2024-04-02 15:15 +0800
description:
categories: [jekyll]
tags:
---

| 기기정보        |        |
| --------------- | ------ |
| 사용기기        | M1 MAC |
| bundler version | 2.1.4  |
| jekyll version  | 4.3.1  |

## 개략적인 순서

1.  jekyll-compose 설치
2.  게시글 기본설정
3.  명령어를 통해 파일생성

---

# 1\. jekyll-compose 설치

---

루트디렉토리에 있는 Gemfile에 맨 아래 아래내용을 추가한다.

```
gem 'jekyll-compose', group: [:jekyll_plugins]
```

jekyll-compose 플러그인을 설치한다.

```
bundle
```

# 2\. 게시글 기본 설정

---

이제 jekyll-compose 플러그인을 통해 게시글을 생성할 수 있는데, 그 전에 게시글에 기본 정보를 설정한다.

**\_config.yml** 파일을 열어 아래내용을 맨아래에 추가해준다.

```yaml
jekyll_compose:
  auto_open: true # 내가 설정한 editor에서, post or draft 생성 시 자동 열림 설정.
  default_front_matter:
    posts: # post에 대해, 기본적으로 넣고 싶은 config keys를 넣어준다.
      description:
      category:
      tags:
```

> 게시글을 작성시킬 에디터를 설정해줘야 auto_open이 제대로 동작하므로
> 자신이 게시글 작성에 사용할 에디터(markdown editor)를 쉘 설정에서 설정해주어야한다.
> {: .prompt-warning }

난 **vscode**를 사용할 예정이므로 쉘 설정을 열고

```
vi ~/.zshrc
```

vscode로 에디터를 설정해주고

```
export JEKYLL_EDITOR=code
```

바뀐 내용을 적용시킨다.
물론 code는 미리 설정된 environment variable set이다.

```
source ~/.zshrc
```

# 3\. 명령어를 통해 파일생성

---

이제 명령어를 통해서 게시글이나 초안 등을 생성할 수 있다.

아래의 명령어를 입력하면 \_post폴더에 작성시간이 찍힌 파일이 생성된다.

```
bundle exec jekyll post "글 제목!"
```

다른 명령어

**draft** -> 입력받은 제목으로 초안 생성 (작성시간이 안 찍힘)  
**post** -> 입력받은 제목으로 게시글 생성 (작성시간이 찍힘)  
**publish** -> 입력받은 초안을 \_post로 옮기고 작성시간 찍어줌  
**unpublish** -> 입력받은 게시글을 다시 \_draft로 옭김  
**page** -> 입력받은 이름으로 페이지 생성  
**rename** -> 입력받은 초안의 이름 변경  
**compose** -> 입력받은 이름으로 파일 생성

## 약어 설정

**지금의 명령어는 너무 기므로 alias를 설정해 간편하게 이용해보자.**

다시 쉘 설정을 열고

```
vi ~/.zshrc
```

맨 아래에 아래 내용을 추가시켜준다.

```
# <repo위치>에는 자신의 github page경로를 넣어준다.
alias post='cd <repo위치> && bundle exec jekyll post'
```

다시 또 바뀐 내용을 적용시켜준다.

```
source ~/.zshrc
```

이제 터미널에서 아래 명령어로 간단하게 게시글 생성이 가능해졌다.

```
post "게시글 제목"
```



macos의 경우, 아래와 같은 오류가 뜬다.
```
--- layout: home # Index page ---
```
로컬이 Linux 기반의 플랫폼이 아닌 경우, 다음과 같은 bundle 명령어를 통해 Linux platform을 추가해야한다.
```
bundle lock --add-platform x86_64-linux
```
위의 명령어를 로컬에서 수행하고, git repo에 커밋한 이후, 다시 Git Actions로 설정에서 Jekyll.yml을 커밋하면 build가 정상적으로 수행되고 얼마 이후, Github repo 주소와 Chripy 테마가 적용되는 것을 확인할 수 있다.

## 참고

- gitlob template [https://chirpy.cotes.page/]
- Jekyll-Compose 깃헙주소 [https://github.com/jekyll/jekyll-compose](https://github.com/jekyll/jekyll-compose)
- template 사용자 blog [https://10kseok.github.io/posts/easy-to-make-default-mdfile-to-use-jekyll-compose/]
- build and deployment error [https://friendlyvillain.github.io/posts/chirpy-setup/]
