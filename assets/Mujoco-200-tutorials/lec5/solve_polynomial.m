syms t0 tf q0 qf;

% 주어진 조건
x1 = t0; y1 = q0; m1 = 0; % 경계 조건 1
x2 = tf; y2 = qf; m2 = 0; % 경계 조건 2

% 4x4 행렬 구성
A = [
    x1^3, x1^2, x1, 1;
    x2^3, x2^2, x2, 1;
    3*x1^2, 2*x1, 1, 0;
    3*x2^2, 2*x2, 1, 0
];

B = [y1; y2; m1; m2];

% 행렬 방정식 풀기
coefficients = A \ B;

% 결과 출력
disp('다항식의 계수:');
disp(coefficients);
