clear;
figureNum = 1;

[deltaCalc, wCalc, rCalc, vinCalc] = iterativeMaxDelta();

numTerms = 100;
amp = 2.5;
period = 1/10e3;
dcOffset = 2.5;

%Anonymous square wave function
%Change fourier coefficients to match a proper square that is A from 0 to
%T/2 and A during T/2 to T
f = @(t,A,T) (mod(t,T) >= 0 & mod(t,T) < T/2) .* A +...
    (mod(t,T) >= T/2 & mod(t,T) < 3*T) .* -A +...
    (mod(t,T) >= 3*T/4 & mod(t,T) < T) .* A;

%Generate square wave harmonics
%[fourier_terms, f_fourier] = fourierExpansion(numTerms, amp, period, dcOffset, f);

%Function prototype to remind myself this exists
%fourierOutput(rInput, inputW, vinInput, fourierTerms);

% Define vector of harmonics to plot.
%HarmonicsToPlot = [1 3 5 7];
%plotFourier(fourier_terms, HarmonicsToPlot, f_fourier, amp, period, dcOffset, f);

%Spreadsheet Data Interpretation

experimentalTable = readtable('experimentalDataForMatlab.xlsx');


