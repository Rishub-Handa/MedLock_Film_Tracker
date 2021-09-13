
function plotFourier(fourierTerms, HarmonicsToPlot, fourierSum, amp, period, dcOffset, anonFunc)

    A = amp; % Amplitude
    T = period; % period
    
    delta_t = T/1e3;
    t = 0:delta_t:2*T;

    % Plot harmonics.
    hold on

    legend_strings = cell(1,length(HarmonicsToPlot));
    for i = 1:length(HarmonicsToPlot)
        plot(t,fourierTerms(:,HarmonicsToPlot(i)));
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
    % Then, plot the function versus the Fourier approximation.
    plot(t,anonFunc(t,A,T) + dcOffset,t,fourierSum,'--');
    %plot(t,fourierSum,'--');
    grid on
    xlabel('Time, t (s)');
    ylabel('f(t)');
    legend('Ideal Signal','Fourier Approximation');
end
