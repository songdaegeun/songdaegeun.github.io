---
layout: post
title: service-worker-remove
date: 2024-05-03 12:20 +0900
author: songdaegeun
categories: ["jekyll"]
tags: [jekyll, service-worker, pwa]
pin: false
math: true
---

### 개요
로컬에서는 Chirpy 테마가 적용된 페이지를 수정 e.g., 디자인 수정, 업로드/삭제 등을 하면 실시간 반영이 되지만 gitblog에는 정상적으로 배포가 되었음에도 불구하고 수정사항이 실시간 반영되지 않는다.
이에 대한 임시 해결책으로 웹 브라우저 내에서 강한 새로고침 (`Ctrl+Shift+R` 또는 `Cmd+Shift+R`)을 통해 수정사항이 적용된 것을 확인할 수 있지만 다시 페이지에 방문해보면 여전히 수정사항이 반영되지 않고, 그대로인 상황이 발생한다.
커밋하고 일정 시간이 지난 이후, 페이지에 접속하면 방문자에게 `A new version of content is available` 창을 띄워주면서 Content를 업데이트할 수 있다는 메시지를 띄워주고 update 버튼을 방문자가 클릭해야 수정사항이 반영되도록 동작한다. 
대부분의 경우, 방문자가 update 버튼을 클릭하도록 유도하는 것이 번거롭고, 수정된 내용이 실시간으로 GitHub 페이지에 잘 적용되었는지 확인하고 싶은 경우가 많을 것이다.

### 문제원인
개발자가 페이지에 접속하지 못하는 상황이 발생하더라도 사용자가 한번이라도 방문한 적이 있는 페이지라면 PWA를 통해 브라우저에 저장된 캐시를 통해 이전에 방문한 페이지를 로드할 수 있도록 구현하였기 때문이다. 

PWA
: PWA(Progressive Web Application)는 모바일 앱과 웹 사이의 중간 형태로, **웹 기술**을 사용하여 사용자 경험을 향상시키는 웹 앱. 다음과 같은 특징을 갖는다.
1. Responsiveness: 다양한 디바이스의 화면 크기에 최적화
2. Connectivity Independence: 오프라인이나 느린 네트워크에서도 작동
3. App-like Experience: 홈 화면에 아이콘을 추가하거나 푸시 알림을 통해 사용자와 상호작용
4. Freshness: 서비스 워커와 캐싱을 통해 콘텐츠를 지속적으로 업데이트
5. Security: HTTPS를 통한 안전한 연결을 사용

### 해결방안
Chripy repository의 issue에서 확인해보니 나와 같이 실시간으로 수정된 내용이 반영되기를 원하는 [사용자의 글](https://github.com/cotes2020/jekyll-theme-chirpy/issues/527#issuecomment-1079998986)을 확인할 수 있었고, 다음과 같이 service worker 설정파일(sw.js) 수정을 통해 커밋과 동시에 페이지 업데이트가 되도록 설정하여 해결할 수 있었다.

service worker
: PWA의 핵심 기능 중 하나이다. 
Service Worker는 웹 앱의 백그라운드에서 실행되는 스크립트이며 브라우저와 웹 앱 사이의 프록시 역할을 한다. 
이를 통해 네트워크 요청을 가로채고 캐시된 리소스를 반환하거나, 오프라인 상태에서 요청을 처리하고, 푸시 알림을 관리할 수 있다. 

> `/assets/js/pwa/sw.js`{: .filepath} 의 기존 내용을 모두 삭제한 이후, 다음 코드를 추가
{: .prompt-info }

```js

  self.addEventListener("install", (event) => {
    self.skipWaiting();
  });
  
  self.addEventListener("activate", (event) => {
    self.registration
      .unregister()
      .then(() => self.clients.matchAll())
      .then((clients) => {
        clients.forEach((client) => {
          if (client.url && "navigate" in client) {
            client.navigate(client.url);
          }
        });
      });
  });

  ``` 

## 참고 문서

[Service worker self-destroying](https://github.com/cotes2020/jekyll-theme-chirpy/issues/527)