---
layout: post
title: 개념-tensor product
date: 2024-04-26 20:43 +0900
description:
author: rosi
categories: [개념]
tags:
math: true
---

### 결론

결론부터 말하자면, **np.dot은 dot product이고, 피연산자가 둘다 2d일때만 특수하게 matrix product이며 \
수학적으로 **matrix product와 tensor product는 다른 거**였다.

matrix가 tensor의 일종이니까, np.dot에서 고차원 tensor인 2x2x2 array는 tensor product로 계산할줄알았는데, \
고차원 tensor라도 np.dot은 dot product(inner product)이니까 vector의 dot product로 계산되는거였다.\
(matrix product도 아니다. a and b are 2-D arrays일 때만 matrix product이다.)\
np.dot의 docs에 아래와 같은 특징이 있었다!\
- If a is an N-D array and b is an M-D array (where M>=2), it is a sum product over the last axis of a and the second-to-last axis of b:\
아래의 형태를 직관적으로 연산할 수 있게 하기위한 dot product였다..

```
dot(a, b)[i,j,k,m] = sum(a[i,j,:] * b[k,:,m])
```

### 문제의 발단과 해결 과정

문제의 발단과 해결 과정은 다음과 같다..

numpy docs에서 np.dot 예시를 보다가 3차이상의 tensor의 곱을 봤는데, 이해가 안갔다. \
예시는 다음과 같다. 

```
a = np.arange(3*4*5*6).reshape((3,4,5,6)) 
b = np.arange(3*4*5*6)[::-1].reshape((5,4,6,3))
print(np.dot(a, b).shape)

# (3,4,5,5,4,3)
```
뭐지? 행렬곱에서 살짝 변형되는 정도일텐데, 왜 이해가 안되지? \
shape가 왜 (3, 4, 5, 5, 4, 3)이지? 하고 구글에 tensor product를 검색해봤다. \
tensor라는 개념자체가 엄밀한 수학적 정의가 요구되는만큼, tensor product도 3차 tensor이상으로가면 복잡한 계산이 필요했고 이걸 자연스럽게 하려면 역시 수학적 개념이 있어야했다..

아래 개념이 모두 필요하다.

[tensor product](https://en.wikipedia.org/wiki/Tensor_product)\
[bilinear map](https://en.wikipedia.org/wiki/Bilinear_map)\
[dual space - linear functional](https://elementary-physics.tistory.com/16)\
[calculus of variations - Euler Lagrange equation](https://en.wikipedia.org/wiki/Calculus_of_variations)\
[kronecker delta](https://en.wikipedia.org/wiki/Kronecker_delta)\
[terson product 설명](https://www.youtube.com/watch?v=qp_zg_TD0qE)\

전부 읽어본건아니고 terson product의 흐름을 이해하기 위해서, \
bilinear map, dual space의 basis인 kronecker delta에 대해 적당한 결과만 이해하면서 2차 tensor의 tensor product예시를 보니까, 3차이상의 tensor product도 이해가 갔다.


_(2차 tensor의 tensor product 예시)_

$$
{\displaystyle A={\begin{bmatrix}a_{1,1}&a_{1,2}\\a_{2,1}&a_{2,2}\\\end{bmatrix}},\qquad B={\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}},}
$$

respectively, then the tensor product of these two matrices is:

$$
{\displaystyle {\begin{bmatrix}a_{1,1}&a_{1,2}\\a_{2,1}&a_{2,2}\\\end{bmatrix}}\otimes {\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}}={\begin{bmatrix}a_{1,1}{\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}}&a_{1,2}{\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}}\\[3pt]a_{2,1}{\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}}&a_{2,2}{\begin{bmatrix}b_{1,1}&b_{1,2}\\b_{2,1}&b_{2,2}\\\end{bmatrix}}\\\end{bmatrix}}={\begin{bmatrix}a_{1,1}b_{1,1}&a_{1,1}b_{1,2}&a_{1,2}b_{1,1}&a_{1,2}b_{1,2}\\a_{1,1}b_{2,1}&a_{1,1}b_{2,2}&a_{1,2}b_{2,1}&a_{1,2}b_{2,2}\\a_{2,1}b_{1,1}&a_{2,1}b_{1,2}&a_{2,2}b_{1,1}&a_{2,2}b_{1,2}\\a_{2,1}b_{2,1}&a_{2,1}b_{2,2}&a_{2,2}b_{2,1}&a_{2,2}b_{2,2}\\\end{bmatrix}}.}
$$

참고로 [Kronecker product](https://en.wikipedia.org/wiki/Kronecker_product)도\
dyad처럼 tensor product의 special case이고, \
[element-wise product](https://en.wikipedia.org/wiki/Hadamard_product_(matrices)) 와는 다른거다.
