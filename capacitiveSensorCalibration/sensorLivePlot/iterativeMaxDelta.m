%%%
%%%
%%% BEST deltaCalc SOLVER
%%%
%%%

function [deltaCalc, wCalc, rCalc, vinCalc] = iterativeMaxdeltaCalc()

    %Circuit components
    wCalc = 0;
    rCalc = 0;
    C2 = 3.5e-12;
    C1 = 3.3e-12;

    %Output of high pass filter
    vout2Temp = 0;
    vout1Temp = 0;
    vout2 = 0;
    vout1 = 0;
    vin = 5;

    %Largest difference between one strip
    deltaCalc = 0;

    for wIndex = 0:1e4:5e6

        for rIndex = 0:1e4:1e6

            for vinIndex = 0:5

                
                %rIndex = 10e3;
                
                vout2Temp = (wIndex*rIndex*C2)/((wIndex^2)*(rIndex^2)*(C2)^2 + 1);
                vout2Temp = vout2Temp * vinIndex;
                vout1Temp = (wIndex*rIndex*C1)/((wIndex^2)*(rIndex^2)*(C1)^2 + 1);
                vout1Temp = vout1Temp * vinIndex;

                if((vout2Temp - vout1Temp) > deltaCalc)

                    deltaCalc = vout2Temp - vout1Temp;
                    vout2 = vout2Temp;
                    vout1 = vout1Temp;
                    wCalc = wIndex;
                    rCalc = rIndex;
                    vinCalc = vinIndex;

                end 

            end

        end

    end

    disp("deltaCalc: " + deltaCalc)
    disp("Frequency: " + wCalc)
    disp("Resistance: " + rCalc)
    disp("Vin:" + vinCalc)
    disp("vout2Temp:" + vout2)
    disp("vout1Temp:" + vout1)
    
    %%%
    %%%
    %%% PLOT OF CAPACITANCES
    %%%
    %%%

    %Capacitor Equation
    d = 18e-3:18e-3:378e-3;
    len = 40e-3;
    wid = 90e-3;
    e0 = 8.85e-12;
    eR = 3;
    CEquation = (e0.*eR.*(len).*(wid)) ./ d;

    %Current Circuit values
    wCurrent = 10e3;
    rCurrent = 1e6;
    vinCurrent = 5;

    %Current Circuit
    voutCurrent = (wCurrent.*rCurrent.*CEquation)./((wCurrent.^2).*(rCurrent.^2).*(CEquation).^2 + 1);
    voutCurrent = voutCurrent * vinCurrent;

    %figure(1);
    %plot(d,voutCurrent);

    %Improved Circuit

    voutCalc = (wCalc.*rCalc.*CEquation)./((wCalc.^2).*(rCalc.^2).*(CEquation).^2 + 1);
    voutCalc = voutCalc * vinCalc;

    %figure(2)
    %plot(d, voutCalc);

    %figure(figureNum);
    %figureNum = figureNum + 1;

    plot(d, voutCurrent);
    hold on
    plot(d, voutCalc);
    hold off
    legend("Current Vout","Improved Vout");

end
