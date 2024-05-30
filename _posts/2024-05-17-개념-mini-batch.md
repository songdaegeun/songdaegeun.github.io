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

- memory access 측면에서 효율적이다.  
많은 양의 memory를 한 번에 참조하게 되면 overhead도 커지고, disk를 무조건 거쳐야 하기에 느려진다.  

- 일반화 성능도 좋아진다.(stochastic에 비해 평균화되어 noise제거)

#### standard normalization외 에 batch normalization을 하는 이유

- 새로운 batch는 새로운 covariance를 갖는다.  
이를 Internal covariate shift라고 한다.  
즉 매번 shuffle이 되므로 매번 feature에 대해 normalization을 해줘야, initialization에 영향 받지 않는 robust한 network를 구축하는 것이 가능하다.  

- training process를 stabilize해준다.  
gradient vanishing/explosion을 막아준다. (residual network 추가하면 더 좋다.)  

1. 표준적인 batch normalization 방법  

$${\displaystyle {\hat {x}}_{i}^{(k)}={\frac {x_{i}^{(k)}-\mu _{B}^{(k)}}{\sqrt {\left(\sigma _{B}^{(k)}\right)^{2}}}}}$$

2. learnable parameter가 추가된 방법  

$${\displaystyle y_{i}^{(k)}=\gamma ^{(k)}{\hat {x}}_{i}^{(k)}+\beta ^{(k)}}$$

매 mini-batch마다 이루어지는 normalization자체가 모델의 표현력을 제한한다.(평균이 0이고 분산이 1이 되도록)  
scale parameter (γ)와 shift parameter (β)를 추가할 경우, training을 통해 모델의 표현력을 회복할 수 있다.
여기서 표현력이란, 근사할 수 있는 함수의 function space의 dimension을 말한다.  

이렇게 하면 normalization의 이점(학습 안정성, 빠른 수렴)을 얻으면서도,  
모델이 데이터의 복잡한 패턴을 학습(perfect data근사)하는 데 제한이 없어진다.

#### end-to-end

단일 model등에 관계 없이 initial raw data로부터 최종 output까지 joint한 training을 하는 approach 중 하나이다.
보통 좋은 performance를 내기 위해, 또 scalable한 model을 만들기 위해 사용한다.

end-to-end가 아닌 예시:
보통 non-differentiable한 layer가 포함될 경우 분리가 된다.
ex: faster R-CNN에서 region proposal network와 RoI pooling layer 사이에 differentiator한 관계가 성립되지 않아서 end-to-end가 아니다.

#### layer normalization

layer normalization은 data가 아닌 feature에 대한 normalization이다.

learnable parameter가 들어있어서 전체 학습 data에 대해 normalization이 가능하다.\
(end to end training도 achieve한다.)