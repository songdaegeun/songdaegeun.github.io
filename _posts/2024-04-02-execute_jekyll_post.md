---
layout: post
title: jekyll-theme-chirpy 기반 블로그 setup log
date: 2024-04-02 15:15 +0800
description:
categories: [jekyll]
tags:
---

본 게시글은 필자가 jekyll기반 open source template인 [chirpy](https://github.com/cotes2020/jekyll-theme-chirpy)를 복제하여,\
블로그를 만들 때 겪은 과정을 소개한다.

### 개요

1. 로컬시스템에 설치 및 github repository에 연결
2. 원활한 블로그 운영을 위한 기본적인 setup 

| 기기정보                |
| --------------- | ------ |
| 사용기기        | M1 MAC |
| bundler version | 2.1.4  |
| jekyll version  | 4.3.1  |


### 1.1. rbenv 설치

Jekyll은 HTML,CSS,JS 같은 정적 파일을 생성하여 서버에 배포할 수 있도록 하는 정적사이트 생성기이다. 
Ruby 언어를 통해 개발되었기 떄문에, Jekyll 실행을 위해선 Ruby 런타임환경이 필요하다.

> macOS는 기본적으로 Ruby가 설치되어있지만, Jekyll에서 필요로 하는 Ruby보다 버전이 낮을 수 있으므로 확인해야 한다.

Ruby를 설치하는 여러 방법이 있지만, 필자는 이 중 rbenv라고 하는 버전 관리 툴을 통해 설치했다.

```
$ brew install rbenv ruby-build
$ rbenv init
## terminal 재시작
$ git clone https://github.com/rbenv/rbenv.git ~/.rbenv
$ echo 'eval "$(~/.rbenv/bin/rbenv init - zsh)"' >> ~/.zshrc
```

### 1.2. Ruby 설치

```
## 설치가능한 ruby 버전 확인
$ rbenv install -l
## ex. rbenv install 3.3.1
$ rbenv install [Version]
## 설치받은 ruby의 버전을 global로 사용할 것인지 local에서만 사용할 것인지 설정
$ rbenv global 3.3.1
# gem은 ruby의 패키지관리도구이다.
# ruby 패키지의 종속성 관리 도구인 Bundler를 설치한다. 
# Bundler는 Gemfile에 명시된 종속성들을 자동으로 설치해준다.
$ gem install bundler
```

### 1.3. Github 설정

github repository 이름은 다음과 같이 짓는다.

> ex. songdaegeun.github.io
> [Username].github.io

Visibility는 Public으로 한다.

### 1.4. Theme 설정

[chirpy](https://github.com/cotes2020/jekyll-theme-chirpy) repository에서 소스를 **zip 파일**로 다운받는다.(.git파일이 없다.)

```
cd [압축푼 zip 파일경로]
## jekyll과 chirpy Theme에 필요한 모듈 설치
git init
git remote add origin [내 저장소]
$ bundle install
$ bundle lock --add-platform x86_64-linux
$ git add .
$ git commit -m [commit message]
$ git push --set-upstream origin main
# 로컬호스트로 블로그구성확인
$ jekyll serve
```

### 1.5. 버그 기록

- 로컬이 linux 기반의 플랫폼이 아닌 경우, 다음과 같이 bundle 명령어를 통해 linux platform을 추가해야한다.
```
$ bundle lock --add-platform x86_64-linux
```
1.4. 설치과정에 포함되어있지만, 필자는 이걸 빼먹었었다.

그래서 아래와 같은 오류가 떴었다.
```
--- layout: home # Index page ---
```

고치려면 bundle 명령어를 통해 linux platform을 추가하고, 원격저장소에 push하고, \
원격저장소의 setting-pages-Build and deployment패널-source를 github Actions로 설정하고,\
jekyll.yml을 커밋한다.
커밋에 대한 배포가 끝난 후, [Username].gitblog.io에 접속해보면 기본화면이 나타난다.

- 웹캐시문제
혹시 문제가 있어서 작업 중이던 gitblog를 삭제했다가 다시 [Username].gitblog.io에 접속하는 경우, \
이전의 gitblog내용이 대신 나오는 경우가 있다. 웹캐시가 남아있기 때문인데, [웹캐시제거](https://sukyungdev.github.io/posts/Post-03/)방법을 따라\
ctrl+shift+R로 강한 새로고침을 해주면 캐시적용없는 새로고침이 가능해서 도메인이 가리키는 진짜 gitblog내용을 볼 수 있다.

### 2.1. jekyll-compose 설치

이거 안하면 jekyll post실행 안된다.

루트디렉토리에 있는 Gemfile에 맨 아래 아래내용을 추가한다.

```
gem 'jekyll-compose', group: [:jekyll_plugins]
```

jemfile에 등록된 종속성인 jekyll-compose 플러그인을 다음의 명령으로 설치한다.

```
bundle
```

### 2.2. post 기본 설정

이제 jekyll-compose 플러그인을 통해 게시글을 생성할 수 있는데, 그 전에 게시글에 기본 정보를 설정하자.\
**config.yml** 파일을 열어 아래내용을 아무데나 추가한다.

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
물론 code는 미리 설정된 environment variable이다.

```
source ~/.zshrc
```

### 2.3. 명령어를 통해 post파일 생성

이제 명령어를 통해서 게시글이나 초안 등을 생성할 수 있다.

아래의 명령어를 입력하면 _post폴더에 작성시간이 찍힌 파일이 생성된다.

```
bundle exec jekyll post [글 제목]
```

다른 명령어

**draft** -> 입력받은 제목으로 초안 생성 (작성시간이 안 찍힘)  
**post** -> 입력받은 제목으로 게시글 생성 (작성시간이 찍힘)  
**publish** -> 입력받은 초안을 _post로 옮기고 작성시간 찍어줌  
**unpublish** -> 입력받은 게시글을 다시 _draft로 옭김  
**page** -> 입력받은 이름으로 페이지 생성  
**rename** -> 입력받은 초안의 이름 변경  
**compose** -> 입력받은 이름으로 파일 생성

### 2.4. post 약어 설정

기존의 post생성 명령어는 너무 길다. **alias를 설정**해 간편하게 이용해보자.

다시 쉘 설정을 열고

```
vi ~/.zshrc
```

맨 아래에 아래 내용을 추가시켜준다.

```
# [repo위치]에는 자신의 github page경로를 넣어준다.
alias post='cd [repo위치] && bundle exec jekyll post'
```

다시 또 바뀐 내용을 적용시켜준다.

```
source ~/.zshrc
```

이제 터미널에서 아래 명령어로 간단하게 게시글 생성이 가능해졌다.

```
post [글 제목]
```

### 참고문서

- [theme 설치](https://iiibreakeriii.github.io/Post2(ChirpyBlog))
- [chirpy docs](https://chirpy.cotes.page/)
- [Jekyll-Compose](https://github.com/jekyll/jekyll-compose)
- [post default setting관련 config.yml](https://10kseok.github.io/posts/easy-to-make-default-mdfile-to-use-jekyll-compose/)
- [build and deployment error](https://friendlyvillain.github.io/posts/chirpy-setup/)
- [웹캐시제거](https://sukyungdev.github.io/posts/Post-03/)