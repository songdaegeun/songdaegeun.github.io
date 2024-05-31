---
layout: post
title: article-BN_and_ICS
date: 2024-05-31 10:47 +0900
author: songdaegeun
categories:
tags:
pin: false
math: true
---

#### 개요

본 포스팅은 "Intro to Optimization in Deep Learning: Busting the Myth About Batch Normalization"에 대한 정리입니다.

#### 결론
논문 [How Does Batch Normalization Help Optimization?(2018)](https://proceedings.neurips.cc/paper/2018/hash/905056c1ac1dad141560467e0a99e1cf-Abstract.html)에 따르면, BN은 ICS를 해결해주지않는다. (BN이 2015에 나온 개념이라 정설까지는 아니지만, 현재 가장 유력한 주장이다.)  
하지만 BN은  
1. 이전 layer의 weight변화에 대한 정보를 담고 있는 hyperparameter로 인한 check pointing mechanism같은 작동 
2. noise에 대한 regulizer에 대한 역할을 수행  
할 수 있기때문에 효과적이다.

#### 정리

Motivation: Does Batch Norm really solve internal covariate shift?  

layer에 공급되는 data의 distribution은 다음 2가지 특성을 만족할 때, 수렴속도와 수렴성이 좋아진다.
1. zero centered
2. constant through time and data(mini-batch) 

internal covariant shift(ICS)란, 논문 [Batch Normalization: Accelerating Deep Network Training by Reducing Internal Covariate Shift(2015)](https://arxiv.org/pdf/1502.03167?ref=blog.paperspace.com)에서 정의되었으며 다음과 같다.  

	"NN의 layer에 input distribution이 flucturating하게 될 때"  
	- internal이라는 말의 뜻은 이 fluctuation이 'intermediate layer'에서 발생한다는 사실을 참조한다. 
	- covariate라는 말의 뜻은 distribution이 각각 다를 수 있는 weight set에 의해 parameterized된다는 사실을 참조한다.
	(각 feature는 이전 layer의 weight를 인자로 가지니까 intermediate layer의 feature distribution은 당연히 달라진다.)
	- Shift는 distribution이 바뀐다는 것을 의미한다. 

back propagation 과정을 생각해보면, weight를 갱신하기위해 각 layer의 derivative를 곱하는 과정에서, 각 layer의 derivative는 그 앞의 layer의 derivative가 곱해진다. 이 때 곱해지는 derivative는 해당 epoch에 대한 weight에 parameterized된 internal input distribution를 기준으로 한다.  
``즉, learning 중에 internal input distribution이 fructuation되고 있다는게 문제이다.``  
(fructuation이 model의 형상을 바꾸므로, cost funciton이 weight에 의해 epoch때마다 미세하게 변하고 있다. 각 weight갱신은 이를 극복할만큼 많이 cost를 감소시켜야한다.)  
weight updata자체는 주어진 mini-batch에 대한 근사가 이루어지는 과정이므로 필요한 과정이다.  
(그러나 weight가 자꾸 interal input distribution에 영향을 주어서 input feature의 distribution을 근사하는 weight를 얻는 것에 방해가 되기는 한다.)  
하지만 full batch보다 mini-batch에서 ICS의 문제점이 도드라진다.  
```
mini-batch에서 ICS가 learning을 방해하는 이유는, NN이 항상 input distribution의 denser region에서 더 잘작동하기 때문이다.
(average loss에서 densor region의 data point들이 dominate하여 그러한 data에 대한 loss가 더 많이 줄어든다.) 

하지만 ICS는 각 iteration(mini-batch학습단위)이 끝날 때마다 denser region의 input distribution도 변하게 만든다. 그렇기때문에 이전 iteration에 의해 학습된 weight는 현재 iteration에서 학습이 일어나고 있는 시점에서는 ICS에 의해 optimal이 아니다. 이전 iternation의 mini-batch가 dense했다면 현재 iteration의 학습 중에 발생하는 ICS가 loss의 증가에 비교적 크게 영향을 미친다.

이 영향을 받는 iteration(이전 iteration에 기인한, 현재 iteration중에 ICS에 의한 loss증가 - 현재 iteration에 의한 loss감소)은 layer은 다음 mini-batch에서 loss를 전혀 감소시키지 않는 output을 생산하도록 할 가능성이 있다.

이러한 점을 고려하여, reasonable한 learning을 하기위해서는 현재 iteration에서의 learning rate를 작게 잡는다던지 하는 매우 세심한 hyperparameter의 tuning이 필요하다. 
```
각 layer에 대해 standard normalization을 통해 unit variance를 갖게 하면,  
기존에 dense/sparse했던 distribution을 정규화시켜서 우리의 각 mini-batch에 대한 최적화가 
특정한 region of the input distribution에 over-specialize하지않게 할 수 있을 것이라고 기대할 수 있다.  
이게 Batch normalization의 의도이다!

그러나, 이는 통상 20 layer이하의 shallow NN에 대해서만 효과가 있으며, 정규화를 했더라도 발생할 수 밖에 없는 minor한 distribution의 차이와 이에 기인한 ICS에 의한 full-batch cost에서의 fluctuations in weights는 layer가 깊어질수록 커져서 큰 차이를 만들어낼 가능성이 있다.(낮은 확률로 학습이 아예 안될 수도 있는 것이다.) 

그럼에도 불구하고 BN은 효과적이다. 이유는 다음과 같다.
1. Ian Goodfellow는 batch norm layer의 two hyperparameter(scale, shift)때문이라고 주장한다.   hyperparameter가 없다면 임의의 layer의 weight는 그 다음 layer들의 분포에 영향을 주지만, hyperparameter를 사용하면 ```2개의 hyperparameter가 이전 layer들의 weight변화에 의한 현재 input distribution에 대한 영향을 내재하게 되어 check pointing mechanism처럼 작동```한다는 것이다.  
이는 convergence를 매우 빠르게하며, 신중하게 initialization and hyperparameter tuning을 할 필요가 없게 해준다.  
또 hyperparameter를 사용하여 distribution을 재배치한다는 것은, 만약 충분히 적절하게 training한다면, original distribution을 회복할 수 있다는 것을 의미한다!

2. BN을 적용하지 않으면 각 mini-batch의 noise가 cost에 적용되기때문에,  
BN은 noise를 제거하기 위한 regularizer로써의 역할도 한다.

#### 참고자료

[Intro to Optimization in Deep Learning: Busting the Myth About Batch Normalization](https://blog.paperspace.com/busting-the-myths-about-batch-normalization/)

