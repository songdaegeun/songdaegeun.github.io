clear all
clc
close all
filename = 'data.csv';

% system('./test')

T = readtable(filename); %check T.Properties
VariableNames = T.Properties.VariableNames;

Arr = table2array(T);
[m,n] = size(Arr);

% figure(1)
% subplot(2,1,1);
% plot(Arr(:,1), Arr(:,2),'r'); hold on;
% plot(Arr(:,1), Arr(:,4),'b-.', 'Linewidth',0.5);
% legend('act', 'ref')
% xlabel('time');
% ylabel(cell2mat(VariableNames(2)))
% 
% subplot(2,1,2);
% plot(Arr(:,1), Arr(:,3),'r'); hold on;
% plot(Arr(:,1), Arr(:,5),'b-.', 'Linewidth',0.5);
% legend('act', 'ref')
% xlabel('time');
% ylabel(cell2mat(VariableNames(3)))

% for i=2:n
%     figure(i)
%     % yy = i;
%     % plot(Arr(:,yy),'r');
%     % ylabel(cell2mat(VariableNames(yy)))
%     yy = i;
%     plot(Arr(:,1), Arr(:,yy),'r');
%     ylabel(cell2mat(VariableNames(yy)))
%     xlabel(cell2mat(VariableNames(1)))
% end

plot(Arr(:,2), Arr(:,3),'r');
ylabel(cell2mat(VariableNames(3)))
xlabel(cell2mat(VariableNames(2)))
axis("equal")