---
layout: post
title: 개념-mini_batch
date: 2024-05-17 18:13 +0900
author: songdaegeun
categories:
tags:
pin: false
math: true
---

#### batch
전체 training set을 말할 때도 있지만 엄밀하게는 다음과 같다.

> 모델을 학습할 때 한 iteration당 사용되는 example set.

본 post에서 batch는 통상 mini batch라고 하는 batch size를 상정한다.

#### batch를 사용하는 이유

standard normalization만 생각하면, 매번 batch 데이터가 바뀌는데 normalizaion이 무슨소용이냐는 생각이 들수도 있다.\
하지만 normalization에는 다양한 방법이 있다.\
예컨데 batch norm layer의 경우, learnable parameter가 들어있어서 전체 학습 data에 대해 normalization이 가능하다.\
(end to end training도 achieve한다.)

- memory access 측면에서 효율적이다.
많은 양의 memory를 한 번에 참조하게 되면 overhead도 커지고, disk를 무조건 거쳐야 하기에 느려진다.
- Internal한 covariate shift를 막아준다.\
매번 shuffle이 되므로 initialization에 영향 받지 않는, 즉 더 robust한 network를 구축하는 것이 가능하다.
- training process를 stabilize해준다.\
gradient vanishing/explosion을 막아준다. (residual network 추가하면 더 좋다.)

#### end-to-end

단일 model등에 관계 없이 initial raw data로부터 최종 output까지 joint한 training을 하는 approach 중 하나이다.
보통 좋은 performance를 내기 위해, 또 scalable한 model을 만들기 위해 사용한다.

end-to-end가 아닌 예시:
보통 non-differentiable한 layer가 포함될 경우 분리가 된다.
ex: faster R-CNN에서 region proposal network와 RoI pooling layer 사이에 differentiator한 관계가 성립되지 않아서 end-to-end가 아니다.
