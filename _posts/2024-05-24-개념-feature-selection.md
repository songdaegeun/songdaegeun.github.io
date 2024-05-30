---
layout: post
title: 개념-feature selection
date: 2024-05-24 18:01 +0900
author: songdaegeun
categories:
tags:
pin: false
math: true
---

feature selection을 위해 cross validation을 사용할 때, boosting, regularization을 사용하여 early stopping할 수 있다.

통상 사용되는 universial approximator의 종류에 따라, 아래와 같이 model의 복잡도를 결정(cross validation 중 early stopping을 통해)하는 것 같다.
1. fixed shape를 사용할때
regularization을 사용하여 early stopping
2.neural network를 사용할 때
학습 iteration중에 early stopping
3. tree를 사용할 때
gradient boosting(boosting방법 중 하나)을 사용하여 early stopping.
새로운 stump의 leaf를 결정하는 과정은 이전 round에서 결정된 model을 고수하면서 이루어져야 한다.
cost를 계산할때도 이전 모델의 값을 y값에서 뺀 residue에 대한 cost를 계산하는 것이 합리적이다.
이는 gradient boosting의 greedy한 cost계산방식과 일치한다.

