---
layout: post
title: 개념-Lipschitz_continuous
date: 2024-05-27 17:46 +0900
author: songdaegeun
categories:
tags: ["Lipschitz constant", "matrix norm", "Rayleigh quotient"]
pin: false
math: true
---

ml_refined교재를 공부하면서 Lipschitz parameter(=Lipschitz constant)가 2번(ex1, ex2)이나 나왔다. 중요한 개념 같아서 정리해본다.  

#### 결론

- Lipschitz constant는 미분값의 상한을 의미한다.
- Rayleigh quotient는 상한/하한을 통해 symmetric Hermitian matrix의 고유값의 최대/최소의 추정에 사용된다.

#### Lipschitz_continuity?  
[Lipschitz_continuity](https://en.wikipedia.org/wiki/Lipschitz_continuity)

- Lipschitz_continuity
Lipschitz 연속성은 함수에 대한 균일한 연속성의 강력한 형태이다. Lipschitz 연속함수의 정의를 보면 알겠지만, 직관적으로 Lipschitz 연속 함수는 미분값에 상한(Lipschitz constant)이 있다.  

- K-Lipschitz-continuous function
Lipschitz-continuous function는 두 점 사이의 거리를 일정 비 이상으로 증가시키지 않는 함수이다.  
정의:  
두 거리 공간  ${\displaystyle (X,d_{X})}$, ${\displaystyle (Y,d_{Y})}$ 사이의 함수 
${\displaystyle f\colon X\to Y}$ 및 음이 아닌 실수 
${\displaystyle K\geq 0}$가 다음 조건을 만족시킨다면, 
${\displaystyle f}$가 
${\displaystyle K}$-립시츠 연속 함수라고 한다.  

임의의  ${\displaystyle x,x'\in X}$에 대하여, ${\displaystyle d_{Y}(f(x),f(x'))\leq Kd_{X}(x,x')}$  
여기서 K를 Lipschitz parameter(=Lipschitz constant)라고 한다.

- Lipschitz-continuous function

두 거리 공간 ${\displaystyle (X,d_{X})}$, ${\displaystyle (Y,d_{Y})}$ 사이의 함수 
${\displaystyle f\colon X\to Y}$가 적어도 하나의 음이 아닌 실수 ${\displaystyle K\geq 0}$에 대하여 ${\displaystyle K}$-립시츠 함수라면, ${\displaystyle f}$를 립시츠 연속 함수라고 한다.

#### ex1. Proof that the Cross Entropy cost is convex
[Rayleigh_quotient](https://en.wikipedia.org/wiki/Rayleigh_quotient)  
여기서는 Lipschitz constant와 함께 Rayleigh_quotient라는 개념이 등장한다.  

- Rayleigh quotient(레일리 몫)
Rayleigh quotient for a given complex Hermitian matrix ${\displaystyle M}$ and nonzero vector ${\displaystyle x}$ is defined as:  
${\displaystyle R(M,x)={x^{H}Mx \over x^{H}x}.}$  

Rayleigh quotient는 최소값으로 ${\displaystyle \lambda _{\min }}$ (the smallest eigenvalue of ${\displaystyle M}$)을 갖는다. 이 때 ${\displaystyle x}$는 ${\displaystyle v_{\min }}$ (the corresponding eigenvector).  
유사하게,  
${\displaystyle R(M,v_{\max })=\lambda _{\max }}$.  
즉,  
$\lambda _{\min } \leq {\displaystyle R(M,x)\leq \lambda _{\max }}$ 이다.

Rayleigh quotient은 모든 고유값의 정확한 값을 얻기 위해 최소-최대 정리 에서 사용된다. 또한 고유값 알고리즘 (예: Rayleigh quotient iteration )에서 고유벡터 근사로부터 고유값 근사를 얻는 데 사용된다.
(고유값 추정에 사용된다는 부분만 알고 넘어가자.)

cross entropy가 convex함을 증명해보자.  
증명:  
cross entropy 공식은 다음과 같다.

$$g(\mathbf{w}) = \frac{1}{P} \sum_{p=1}^{P} g_p(\mathbf{w}) = -\frac{1}{P} \sum_{p=1}^{P} y_p \log(\sigma(\mathbf{x}^{\circ T}_p \mathbf{w})) + (1 - y_p) \log(1 - \sigma(\mathbf{x}^{\circ T}_p \mathbf{w})) $$

g에 대한 second derivative는 다음과 같다.
$$ \nabla^2 g(\mathbf{w}) = \frac{1}{P} \sum_{p=1}^{P} \sigma(\mathbf{x}^{\circ T}_p \mathbf{w}) (1 - \sigma(\mathbf{x}^{\circ T}_p \mathbf{w})) \mathbf{x}^{\circ}_p (\mathbf{x}^{\circ T}_p) $$

우리는 방금 Rayleigh quotient를 배우면서 어떤 square symmetric(Hermitian) matrix의 고유값의 최소값은 Rayleigh quotient의 하한임을 알았다.  

여기서 헤시안 행렬(Hessian matrix) $\nabla^2 g(\mathbf{w})$는 symmetric이며, 따라서 eigen vector는 orthonormal하여 Rayleigh quotient의 분모는 1이므로 다음과 같이 표현할 수 있다.
$$\mathbf{z}^T \nabla^2 g(\mathbf{w}) \mathbf{z}$$

간략함을 위해 다음과 같이 표현한다.  
$\sigma_p = \sigma(\mathbf{x}^{\circ T}_p \mathbf{w}) (1 - \sigma(\mathbf{x}^{\circ T}_p \mathbf{w}))$  - (1)

따라서 Rayleigh quotient는 다음과 같다.

$$\mathbf{z}^T \nabla^2 g(\mathbf{w}) \mathbf{z} = \mathbf{z}^T \left( \frac{1}{P} \sum_{p=1}^{P} \sigma_p \mathbf{x}_p \mathbf{x}_p^T \right) \mathbf{z} = \frac{1}{P} \sum_{p=1}^{P} \sigma_p (\mathbf{z}^T \mathbf{x}_p)^2$$

$0 \leq \sigma_p \leq 1, (\mathbf{z}^T \mathbf{x}_p)^2 \geq 0$이므로,  
Rayleigh quotient의 하한은 0이며, 따라서 헤시안 행렬의 최소의 고유값도 0이므로 헤시안 행렬은 positive semi-definite여서 cross entropy가 convex함을 보였다.

추가로 cross entropy의 second derivative인 헤시안 행렬의 상한, 즉 Lipschitz constant도 구할 수 있다.  
증명:  
아까 정의한 식(1)에서 $\sigma$의 범위가 0부터 1임을 고려하면, 식(1)은 다음과 같이 1/4을 최대값으로 갖는다. 
$$\sigma_p \leq \frac{1}{4}$$
따라서 Rayleigh quotient는 다음과 같다.
$$ \mathbf{z}^T \nabla^2 g(\mathbf{w}) \mathbf{z} \leq \frac{1}{4\mathbf{P}} \mathbf{z}^T \left( \sum_{p=1}^{P} \mathbf{x}^{\circ}_p \mathbf{x}^{\circ}_p{}^T \right) \mathbf{z} $$
$\mathbf{z}^T \left( \sum_{p=1}^{P} \mathbf{x}^{\circ}_p \mathbf{x}^{\circ}_p{}^T \right) \mathbf{z}$의 최대값은 $\sum_{p=1}^{P}\mathring{\mathbf{x}}_p^{\,}\mathring{\mathbf{x}}_p^T $의 maximum eigenvalue이다.(주축정리)  

행렬의 2-norm의 제곱은 그 행렬의 maximum eigenvalue과 같다.  
("행렬 A의 p-norm과 2-norm의 제곱" 참고)  
따라서 cross entropy의 Lipschitz constant는 다음과 같다.
$$
L = \frac{1}{4P}\left\Vert \sum_{p=1}^{P}\mathring{\mathbf{x}}_p^{\,}\mathring{\mathbf{x}}_p^T \right\Vert_2^2
$$

- 행렬 A의 p-norm과 2-norm의 제곱  
[행렬의 norm](https://ghebook.blogspot.com/2021/03/matrix-norm-and-condition-number.html)

행렬 A의 p-norm은 다음과 같이 정의된다.
$$
{\displaystyle \|A\|_{p}=\sup _{x\neq 0}{\frac {\|Ax\|_{p}}{\|x\|_{p}}} \to  \|Ax\|_{p} \leq \displaystyle \|A\|_{p}\|x\|_{p}}
$$
행렬 A의 2-norm의 제곱은 다음과 같다.
$$
\begin{align*}
(\|\mathbf{A}\|_2)^2 &= \max_{\mathbf{x} \neq 0} \left(\frac{\|\mathbf{A}\mathbf{x}\|_2}{\|\mathbf{x}\|_2}\right)^2 = \max_{\mathbf{x} \neq 0} \frac{\mathbf{x}^T \mathbf{A}^T \mathbf{A} \mathbf{x}}{\mathbf{x}^T \mathbf{x}} \\
&= \max_{\mathbf{x} \neq 0} \frac{\mathbf{x}^T \mathbf{S} \mathbf{x}}{\mathbf{x}^T \mathbf{x}}
\end{align*}
$$
(이는 Rayleigh quotient이므로 상한은 $\lambda_{max}$라고 바로 결론지어도 된다.)  
행렬 곱 Sx를 직교하는 고유 벡터의 선형 결합(linear combination)으로 다시 표현하면 다음과 같다.
$$
\begin{align*}
\mathbf{Sx} &= \mathbf{S} (a_1 \hat{\mathbf{x}}_1 + a_2 \hat{\mathbf{x}}_2 + \cdots + a_n \hat{\mathbf{x}}_n) \\
&= a_1 \lambda_1 \hat{\mathbf{x}}_1 + a_2 \lambda_2 \hat{\mathbf{x}}_2 + \cdots + a_n \lambda_n \hat{\mathbf{x}}_n
\end{align*}
$$
따라서 2-norm은 다음과 같이 대수적으로 표현된다.
$$
\begin{align*}
(\|\mathbf{A}\|_2)^2 &= \max \left( \frac{\lambda_1 a_1^2 + \lambda_2 a_2^2 + \cdots + \lambda_n a_n^2}{a_1^2 + a_2^2 + \cdots + a_n^2} \right) \\
&= \max (\lambda_1 r_1^2 + \lambda_2 r_2^2 + \cdots + \lambda_n r_n^2)
\end{align*}
$$
여기서 $r_1^2 + r_2^2 + \cdots + r_n^2=1$이며, 만약 $\lambda_{max} = \lambda_1$인 경우,  
다음과 같이 상한은 $\lambda_{max}$이다.
$$
\lambda_1 r_1^2 + \lambda_2 r_2^2 + \cdots + \lambda_n r_n^2\\
= \lambda_1 + (\lambda_2-\lambda_1) r_2^2 + \cdots + (\lambda_n-\lambda_1) r_n^2\\
\leq \lambda_{max}
$$
즉, 임의의 행렬의 2-norm의 제곱은 그 행렬의 $\lambda_{max}$이다.  

#### ex2. setting conservative step length

motivate: optimize하고자하는 cost function의 convergence를 보장하기위함이다.  
gradient descent의 각 step마다 함수값이 반드시 감소함을 보장하면 된다.  

해석 함수인 cost function은 다음과 같이 $w^0$를 중심으로하는 simple quadratic approximation으로 나타낼 수 있다.  

$$
h_{\alpha}(w) = g(w^{0}) + \nabla g(w^{0})^{T}(w - w^{0}) + \frac{1}{2\alpha} (w - w^{0})^{2}
$$

우변의 3번째 term의 계수를 저렇게 설정한 이유는 각 step마다 갱신되는 w의 값의 위치가 이 approximation의 극소값으로 jump하도록 하기위해서이다.

gradient descent step을 통한 update는 다음과 같이 이루어진다.  
$w^{1} = w^{0} - \alpha \nabla g(w^{0})$

cost function의 다음과 같이 1차 미분해보면, w는 $w^1$과 같다는 것을 알 수 있다.  
$\nabla h_{\alpha}(w) = \nabla g(w^{0}) + \frac{1}{\alpha} (w - w_{0}) = 0$

Backtracking line search나 Exact line search를 사용해도 되지만,  
optimal fixed steplength values는 그러한 search없이 conservative step length를 제시하는  benchmark이다. optimal fixed steplength values는 Lipschitz parameter의 역수이다.

- Backtracking line search
1. Choose an initial value for α(step length), and a scalar ”dampening factor”
2. Create the candidate descent step
3. Test if $\nabla g(w^k) \le \nabla h_{\alpha}(w^k)$. If yes, then choose $w^k$ as the next gradient descent step; otherwise decrease the value of $α$ as $α ←- tα$, and go back to step 2.

- Exact line search  
candidate α(step length) list를 순회하며 다음을 만족하는 α를 찾는다.
$$
\text{minimize}_{α>0} \quad g(w_{k-1} - \alpha \nabla g(w_{k-1}))
$$

- optimal fixed steplength values

$$\text{max}_{\mathbb{w}}||\nabla^2 g(\mathbb{w})||_2 = L$$
such that, L is maximum curvature.

and

$$α = \frac{1}{L}$$

optimal fixed steplength values를 Lipschitz parameter의 역수로 했을 때,  
접점을 제외한 모든 점에 대해 approximation함수가 original function보다 위에 있게 됨을 보이자.

증명:
$$
h_{1/L}(\mathbf{w}) = g(\mathbf{w_{k-1}}) + \nabla g(\mathbf{w_{k-1}})^T (\mathbf{w} - \mathbf{w_{k-1}}) + \frac{L}{2} \left\| \mathbf{w} - \mathbf{w_{k-1}} \right\|^2
$$

$$
g(\mathbf{w}) = g(\mathbf{w_{k-1}}) + \nabla g(\mathbf{w_{k-1}})^T (\mathbf{w} - \mathbf{w_{k-1}}) + \frac{1}{2} (\mathbf{w} - \mathbf{w_{k-1}})^T \nabla^2 g(\mathbf{c}) (\mathbf{w} - \mathbf{w_{k-1}})
$$  
where $c$ is a point on the line segment connecting $w$ and $w_{k-1}$.

since $$ \nabla^2 g \in L \mathbb{I}_{N \times N}$$  
such that L is Lipschitz parameter,  
we have  
$$
\mathbf{a}^T \nabla^2 g(\mathbf{c}) \mathbf{a} \leq L \|\mathbf{a}\|_2^2
$$  
for $a = w - w_{k-1}$, which implies $g(w) ≤ h_{1/L}(w)$.

