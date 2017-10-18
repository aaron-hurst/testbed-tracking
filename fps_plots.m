sh_speeds = [17.74 14.04 9.91 7.97 7.25 5.93];
sh_errors = [ 0.38  0.74 0.01 0.21 0.07 0.01];

hist_speeds = [16.20 15.46 15.11 14.97];
hist_errors = [ 0.18  0.04 0.09  0.03];

figure(1)
clf
hold on
%errorbar(sh_speeds, sh_errors, 'ob', 'MarkerSize', 6, 'MarkerFaceColor',[0,0,0.8])
%errorbar(hist_speeds, hist_errors, 'dr', 'MarkerSize', 6, 'MarkerFaceColor',[0.8,0,0])
errorbar(sh_speeds, sh_errors, '--ob', 'MarkerSize', 7)
errorbar(hist_speeds, hist_errors, '--dr', 'MarkerSize', 7)
xlim([0  7])
ylim([0 22])
legend({'Hue-matching','Histogram-matching'},'FontSize',22)
xlabel('Number of cars','FontSize',22)
ylabel('Frames per second','FontSize',22)
xt = get(gca, 'XTick');
set(gca, 'FontSize', 18)
hold off