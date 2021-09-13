% INCOMPLETE LOGIC
function fourierOutput(rInput, inputW, vinInput, fourierTerms) 

    vout = (wInput.*rInput.*cCurrent)./((wInput.^2).*(rInput.^2).*(cCurrent).^2 + 1);

    for n = 1:length(fourierTerms)

        fourierTerms = vout * vinInput;

    end 

end
