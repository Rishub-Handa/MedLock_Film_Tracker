clear;
figure_num = 1;

% Define constants and circuit parameters.
number_of_terms = 100;
A = 5; % Amplitude
T = 10e-3; % period

% Define anonymous functions and such.
f = @(t,A,T) (mod(t,T) >= 0 & mod(t,T) < T/4) .* A +...
    (mod(t,T) >= T/4 & mod(t,T) < 3*T/4) .* -A +...
    (mod(t,T) >= 3*T/4 & mod(t,T) < T) .* A;
a0 = 0;
a_n = @(n,A) 2*A*(sin(n*pi/2)-sin(3*n*pi/2))/pi/n;
b_n = @(n,A) 0;

% Define vector of harmonics to plot.
HarmonicsToPlot = [1 3 5 7];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

delta_t = T/1e3;
t = 0:delta_t:2*T;

an = zeros(1,number_of_terms);
bn = zeros(1,number_of_terms);
fourier_terms = zeros(length(t),number_of_terms);
for n = 1:number_of_terms
    an(n) = a_n(n,A);
    bn(n) = b_n(n,A);
    cn = sqrt(an(n)^2+bn(n)^2);
    phi_n = -atan(bn(n)/an(n));
    if (an(n) < 0)
        phi_n = pi + phi_n;
    end
    fourier_terms(:,n) = cn*cos(2*n*pi*t/T + phi_n);
end

% Compute the Fourier approximation.
f_fourier = a0 + sum(fourier_terms,2);

% Plot harmonics.
figure(figure_num); figure_num = figure_num + 1;
hold on

legend_strings = cell(1,length(HarmonicsToPlot));
for i = 1:length(HarmonicsToPlot)
    plot(t,fourier_terms(:,HarmonicsToPlot(i)));
    legend_strings{i} = strcat('c_{',sprintf('%d',HarmonicsToPlot(i)));
    legend_strings{i} = strcat(legend_strings{i},'}cos(2\pi{');
    legend_strings{i} = strcat(legend_strings{i},sprintf('%d',HarmonicsToPlot(i)));
    legend_strings{i} = strcat(legend_strings{i},'}f_0t+\phi_{');
    legend_strings{i} = strcat(legend_strings{i},sprintf('%d',HarmonicsToPlot(i)),'})');
end

hold off
xlabel('Time, t (s)');
ylabel('Amplitude');
grid on
legend(legend_strings);

% Then, plot the function versus the Fourier approximation.
figure(figure_num); figure_num = figure_num + 1;
plot(t,f(t,A,T),t,f_fourier,'--');
grid on
xlabel('Time, t (s)');
ylabel('f(t)');
legend('Analytical','Fourier Approximation');
