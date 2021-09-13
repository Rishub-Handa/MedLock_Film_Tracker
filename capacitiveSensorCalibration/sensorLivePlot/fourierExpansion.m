%%%
%%%
%%% FOURIER EXPANSION
%%%
%%%

%NAME:  fourierExpansion
%DESC:  Returns a vector of fourier terms in terms of t
%ARG:   numTerms: 
%           Number of harmonic terms that make up the fourier series
%       amp: 
%           Amplitude of input signal.
%       period: 
%           Period of input signal.
%       anonymousInputFunction: 
%           Anonymous function that defines the bounds
%           of the periodic signal.
function [fourier_terms, f_fourier] = fourierExpansion(number_of_terms, A, T, a0, f)
 

    % Define anonymous functions and such.
    %f = @(t,A,T) (mod(t,T) >= 0 & mod(t,T) < T/4) .* A +...
    %    (mod(t,T) >= T/4 & mod(t,T) < 3*T/4) .* -A +...
    %    (mod(t,T) >= 3*T/4 & mod(t,T) < T) .* A;
    %a0 = 0;
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


end
