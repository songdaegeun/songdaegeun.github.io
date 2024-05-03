clear all
clc
close all
filename = 'data.csv';

T = readtable(filename); %check T.Properties
VariableNames = T.Properties.VariableNames;

Arr = table2array(T);
[m,n] = size(Arr);

for i=2:n
    figure(i)
    % yy = i;
    % plot(Arr(:,yy),'r');
    % ylabel(cell2mat(VariableNames(yy)))
    yy = i;
    plot(Arr(:,1), Arr(:,yy),'r');
    ylabel(cell2mat(VariableNames(yy)))
    xlabel(cell2mat(VariableNames(1)))
    hold on;
    if(i==n-1 ||i==n)
    % steady state로 수렴하는 시간 찾기
    steady_state_value = Arr(m,i); 
    tolerance = abs(0.02*steady_state_value); % 수렴 허용 오차
    
    index = find(abs(Arr(:,i) - steady_state_value) > tolerance, 1, 'last');
    steady_state_time = Arr(index,1);
    plot(steady_state_time, Arr(index,i), 'bo', 'MarkerSize', 10);
  
    % 결과 출력
    fprintf('q%d: Steady state로 수렴하는 시간: %.2f\n',i-(n-2), steady_state_time);
    
    end
    hold off;
end
