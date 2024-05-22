---
layout: post
title: RBF_kernel
date: 2024-05-21 20:21 +0900
author: songdaegeun
categories:
tags:
pin: false
math: true
---

#### RBF(radial basis function)
 
radial function은 property $${\textstyle \varphi (\mathbf {x} )={\hat {\varphi }}(\left\|\mathbf {x} \right\|)}$$를 만족하는  
어떤 function $$𝜑{\textstyle \varphi }$$이다.  
즉 input vector와 fixed vector사이의 거리에만 의존하는 함수이다.  
radial function이 어떤 function space의 basis가 되는 경우, radial basis function이라고 한다.  
radial basis functions의 합은 보통 어떤 함수를 근사하기위해 사용된다.  
(ex. RBF kernel. 이 kernel은 근사하고자하는 nonlinear함수를 포함하는 function space를 span한다.)

Infinitely Smooth RBFs은 다음과 같다.  
여기서 $${\displaystyle \varepsilon }$$은 shape parameter이며, 
이 RBF들은 $${\displaystyle C^{\infty }(\mathbb {R} )}$$에 속하고(실수집합, 무한미분가능하고 연속)
strictly positive definite functions이다.  
- Gaussian:
$${\displaystyle \varphi (r)=e^{-(\varepsilon r)^{2}},}$$  
- Inverse quadratic:
$${\displaystyle \varphi (r)={\dfrac {1}{1+(\varepsilon r)^{2}}},}$$
- Inverse multiquadric:
$${\displaystyle \varphi (r)={\dfrac {1}{\sqrt {1+(\varepsilon r)^{2}}}}}$$

이 외에도 Polyharmonic spline, Compactly Supported RBFs가 있지만,  
**"RBF kernel"**은 기계학습에서 Infinitely Smooth RBFs에 속하는 Gaussian RBF를 칭한다.

#### (gaussian) RBF kernel 

RBF kernel은 gaussian rbf function의 형태를 갖는 kernel이기때문에, Gaussian kernel이라고도 한다.  

RBF kernel on two samples $${\displaystyle \mathbf {x} \in \mathbb {R} ^{k}}$$ and $${\displaystyle \mathbf {x'} }$$은  
input space내에 feature vector(우리는 kernelization을 다루고 있으므로, nonlinear feature transformation은 명시적으로 계산할 필요없음을 주의)로 표현되며, 다음과 같다.  
$${\displaystyle K(\mathbf {x} ,\mathbf {x'} )=\exp \left(-{\frac {\|\mathbf {x} -\mathbf {x'} \|^{2}}{2\sigma ^{2}}}\right)}$$  
An equivalent definition involves a parameter $${\displaystyle \textstyle \gamma ={\tfrac {1}{2\sigma ^{2}}}}$$:  
$${\displaystyle K(\mathbf {x} ,\mathbf {x'} )=\exp(-\gamma \|\mathbf {x} -\mathbf {x'} \|^{2})}$$
where γ is a parameter that sets the “spread” of the kernel.  

#### The RBF kernel as a projection into infinite dimensions
Recall a kernel is any function of the form:
K(x, x0) = <ψ(x), ψ(x0)>
where ψ is a function that projections vectors x into a new vector space.(ex. nonlinear feature transformation)  
The kernel function computes the inner-product between two projected vectors.   

우리는 아래를 증명함으로써, RBF kernel에 대한 ψ function은 vector를 infinite dimensional space로 project한다.  
For Euclidean vectors, this space is an infinite dimensional Euclidean space.  
즉, $$ψ_{RBF} : R^n → R^∞$$를 증명해보자.

#### The RBF kernel as a projection into infinite dimensions - proof

Without loss of generality, let γ = $$1/2$$ ($${\displaystyle \sigma =1}$$)  
$$K_{\text{RBF}}(\mathbf{x}, \mathbf{x}_0) = \exp \left( -\frac{1}{2} \|\mathbf{x} - \mathbf{x}_0\|^2 \right)$$  
= $$\exp \left( -\frac{1}{2} \langle \mathbf{x} - \mathbf{x}_0, \mathbf{x} - \mathbf{x}_0 \rangle \right)$$ \\
= $$\exp \left( -\frac{1}{2} \left( \langle \mathbf{x}, \mathbf{x} - \mathbf{x}_0 \rangle - \langle \mathbf{x}_0, \mathbf{x} - \mathbf{x}_0 \rangle \right) \right)$$ \\
= $$\exp \left( -\frac{1}{2} \left( \langle \mathbf{x}, \mathbf{x} \rangle - \langle \mathbf{x}, \mathbf{x}_0 \rangle - \langle \mathbf{x}_0, \mathbf{x} \rangle + \langle \mathbf{x}_0, \mathbf{x}_0 \rangle \right) \right)$$ \\
= $$\exp \left( -\frac{1}{2} \left( \|\mathbf{x}\|^2 + \|\mathbf{x}_0\|^2 - 2 \langle \mathbf{x}, \mathbf{x}_0 \rangle \right) \right)$$ \\
= $$\exp \left( -\frac{1}{2} \|\mathbf{x}\|^2 - \frac{1}{2} \|\mathbf{x}_0\|^2 \right) \exp \left( \langle \mathbf{x}, \mathbf{x}_0 \rangle \right)$$ \\
= $$C \exp \left( \langle \mathbf{x}, \mathbf{x}_0 \rangle \right)$$

where
$$C := \exp \left( -\frac{1}{2} \|\mathbf{x}\|^2 - \frac{1}{2} \|\mathbf{x}_0\|^2 \right)$$
is a constant.

Using the Taylor expansion of $$e^x$$, we get:  
$$C \exp \left( \langle \mathbf{x}, \mathbf{x}_0 \rangle \right) = C \sum_{n=0}^{\infty} \frac{\langle \mathbf{x}, \mathbf{x}_0 \rangle^n}{n!} = C \sum_{n=0}^{\infty} \frac{K_{\text{poly}}^{(n)}(\mathbf{x}, \mathbf{x}_0)}{n!}$$

We see that the RBF kernel is formed by taking an infinite sum over [polynomial kernels]((https://en.wikipedia.org/wiki/Polynomial_kernel)).  

RBF kernel이 polynomial kernels의 무한 합이다.
kernel은 nonlinear feature vector의 inner product이고, 두 kernel의 합은 각 inner product의 합을 의미한다.  
그 결과를 다시 kernel로 표현하려면, 두 nonlinear feature vector의 inner product로 표현해야한다.
이 경우 더해지기 전 두 kernel의 nonlinear feature vector를 fa, fb라고 하고  
더해진 후의 nonlinear feature vector fc라고 할 때,  
$$fc = [fa fb]^T$$ 로 생각할 수 있다. 
즉 두 kernel의 합이 되는 kernel의 nonlinear feature vector fc의 차원은 다음과 같다.  
dim(fc) = dim(fa)+dim(fb)  
따라서 kernel의 무한 합이 되는 kernel의 nonlinear feature vector는 infinite dimentional vector가 된다.  
(해당내용은 [RBF kernel lecture note](https://pages.cs.wisc.edu/~matthewb/pages/notes/pdf/svms/RBFKernel.pdf) 참고)  

#### mth entry (or feature) of infinite dimentional nonlinear feature fransform vector

실제로 RBF kernel에 상응하는, 무한차원의 nonlinear feature fransform vector는 다음과 같다.  
let γ = $$1/2$$ ($${\displaystyle \sigma =1}$$)  
its expansion using the multinomial theorem is:  

$$
{\displaystyle {\begin{alignedat}{2}\exp \left(-{\frac {1}{2}}\|\mathbf {x} -\mathbf {x'} \|^{2}\right)&=\exp({\frac {2}{2}}\mathbf {x} ^{\top }\mathbf {x'} -{\frac {1}{2}}\|\mathbf {x} \|^{2}-{\frac {1}{2}}\|\mathbf {x'} \|^{2})\\[5pt]&=\exp(\mathbf {x} ^{\top }\mathbf {x'} )\exp(-{\frac {1}{2}}\|\mathbf {x} \|^{2})\exp(-{\frac {1}{2}}\|\mathbf {x'} \|^{2})\\[5pt]&=\sum _{j=0}^{\infty }{\frac {(\mathbf {x} ^{\top }\mathbf {x'} )^{j}}{j!}}\exp \left(-{\frac {1}{2}}\|\mathbf {x} \|^{2}\right)\exp \left(-{\frac {1}{2}}\|\mathbf {x'} \|^{2}\right)\\[5pt]&=\sum _{j=0}^{\infty }\quad \sum _{n_{1}+n_{2}+\dots +n_{k}=j}\exp \left(-{\frac {1}{2}}\|\mathbf {x} \|^{2}\right){\frac {x_{1}^{n_{1}}\cdots x_{k}^{n_{k}}}{\sqrt {n_{1}!\cdots n_{k}!}}}\exp \left(-{\frac {1}{2}}\|\mathbf {x'} \|^{2}\right){\frac { {x'}_{1}^{n_{1}}\cdots {x'}_{k}^{n_{k}}}{\sqrt {n_{1}!\cdots n_{k}!}}}\\[5pt]&=\langle \varphi (\mathbf {x} ),\varphi (\mathbf {x'} )\rangle \end{alignedat}}}
$$
$$
{\displaystyle \varphi (\mathbf {x} )=\exp \left(-{\frac {1}{2}}\|\mathbf {x} \|^{2}\right)\left(a_{\ell _{0}}^{(0)},a_{1}^{(1)},\dots ,a_{\ell _{1}}^{(1)},\dots ,a_{1}^{(j)},\dots ,a_{\ell _{j}}^{(j)},\dots \right)}
$$

where $${\displaystyle \ell _{j}={\tbinom {k+j-1}{j}}},$$\
$$
{\displaystyle a_{\ell }^{(j)}={\frac {x_{1}^{n_{1}}\cdots x_{k}^{n_{k}}}{\sqrt {n_{1}!\cdots n_{k}!}}}\quad |\quad n_{1}+n_{2}+\dots +n_{k}=j\wedge 1\leq \ell \leq \ell _{j}}
$$

#### 참고문서

[RBF](https://en.wikipedia.org/wiki/Radial_basis_function)  
[RBF kernel](https://en.wikipedia.org/wiki/Radial_basis_function_kernel#cite_note-Chang2010-1)  
[RBF kernel lecture note](https://pages.cs.wisc.edu/~matthewb/pages/notes/pdf/svms/RBFKernel.pdf)  
[RBF 설명 blog](https://m.blog.naver.com/sw4r/221497261535)

