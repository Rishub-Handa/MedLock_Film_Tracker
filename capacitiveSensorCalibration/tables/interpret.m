clear;

figure_num = 1;

parentDir = dir;

youngestChildDir = "";

for k = 1:length(parentDir) 
    
    if(contains(parentDir(k).name, "experiment"))
       
        youngestChildDir = parentDir(k).name;
        
    end
    
end

cd(youngestChildDir)
%cd(parentDir(length(parentDir) - 1).name)

childDir = dir;

youngestMeasurement = ""

for k = 1:length(childDir) 
    
    if(contains(childDir(k).name, "measurement"))
       
        youngestMeasurement = childDir(k).name;
        
    end
    
end
  
cd(youngestMeasurement)

% currD = childDir(k).name % Get the current subdirectory name
% % Run your function. Note, I am not sure on how your function is written,
% % but you may need some of the following
% cd(currD); % change the directory (then cd('..') to get back)
%currentDir = dir(currD);
OSM = readtable("OSM.csv");
SIM = readtable("SIM.csv");
RIM = readtable("RIM.csv");

figure(figure_num);
figureVar = figure(figure_num);
pos = get(figureVar,'position');
set(figureVar,'position',[pos(1:2)/4 pos(3:4)*2])
figure_num = figure_num + 1;

time = OSM{:,1};
time = time ./ 1000;
raw = OSM{:,2};
averaging = OSM{:,3};
deviation = OSM{:,4};



sgtitle(youngestMeasurement);
subplot(3,3,1)
plot(time, raw);
title('OSM Raw');

subplot(3,3,2)
plot(time, averaging);
title('OSM Averaging');

subplot(3,3,3)
plot(time, deviation);
title('OSM Deviation');

time = SIM{:,1};
time = time ./ 1000;
raw = SIM{:,2};
averaging = SIM{:,3};
deviation = SIM{:,4};

subplot(3,3,4)
plot(time, raw);
title('SIM Raw');

subplot(3,3,5)
plot(time, averaging);
title('SIM Averaging');

subplot(3,3,6)
plot(time, deviation);
title('SIM Deviation');


time = RIM{:,1};
time = time ./ 1000;
raw = RIM{:,2};
averaging = RIM{:,3};
deviation = RIM{:,4};

subplot(3,3,7)
plot(time, raw);
title('RIM Raw');

subplot(3,3,8)
plot(time, averaging);
title('RIM Averaging');

subplot(3,3,9)
plot(time, deviation);
title('RIM Deviation');

w = waitforbuttonpress;

%pause;

close(figureVar);

cd('..')




% %VERSION THAT STEPS THROUGH EVERY FOLDER AND GRAPHS THE FIGURES
% childDir = dir; % A is a struct ... first elements are '.' and '..' used for navigation.
% for k = 1:length(childDir) % avoid using the first ones
%     
%     if(contains(childDir(k).name, "measurement"))
%     
%         
%           currD = childDir(k).name % Get the current subdirectory name
%           % Run your function. Note, I am not sure on how your function is written,
%           % but you may need some of the following
%           cd(currD); % change the directory (then cd('..') to get back)
%           currentDir = dir(currD);
%           OSM = readtable("OSM.csv");
%           SIM = readtable("SIM.csv");
%           RIM = readtable("RIM.csv");
% 
%           figure(figure_num);
%           figureVar = figure(figure_num);
%           pos = get(figureVar,'position');
%           set(figureVar,'position',[pos(1:2)/4 pos(3:4)*2])
%           figure_num = figure_num + 1;
% 
%           time = OSM{:,1};
%           raw = OSM{:,2};
%           averaging = OSM{:,3};
%           deviation = OSM{:,4};
% 
% 
% 
%           sgtitle(currD);
%           subplot(3,3,1)
%           plot(time, raw);
%           title('OSM Raw');
% 
%           subplot(3,3,2)
%           plot(time, averaging);
%           title('OSM Averaging');
% 
%           subplot(3,3,3)
%           plot(time, deviation);
%           title('OSM Deviation');
% 
%           time = SIM{:,1};
%           raw = SIM{:,2};
%           averaging = SIM{:,3};
%           deviation = SIM{:,4};
% 
%           subplot(3,3,4)
%           plot(time, raw);
%           title('SIM Raw');
% 
%           subplot(3,3,5)
%           plot(time, averaging);
%           title('SIM Averaging');
% 
%           subplot(3,3,6)
%           plot(time, deviation);
%           title('SIM Deviation');
% 
% 
%           time = RIM{:,1};
%           raw = RIM{:,2};
%           averaging = RIM{:,3};
%           deviation = RIM{:,4};
% 
%           subplot(3,3,7)
%           plot(time, raw);
%           title('RIM Raw');
% 
%           subplot(3,3,8)
%           plot(time, averaging);
%           title('RIM Averaging');
% 
%           subplot(3,3,9)
%           plot(time, deviation);
%           title('RIM Deviation');
% 
%           w = waitforbuttonpress;
%           
%           %pause;
% 
%           close(figureVar);
% 
%           cd('..')
% 
%     end
%       
% end



% Files=dir(pwd);
% 
% fileList = zeros(0, length(Files));
% 
% %disp(Files)
% 
% currentMode = "NONE"
% 
% 
%        if(line == "OSM"):
% 
%             currentMode = "OSM"
%         
%         elif(line == "SIM"):
% 
%             currentMode = "SIM"
% 
%         elif(line == "RIM"):
% 
%             currentMode = "RIM"
% 
%         if(currentMode == "OSM"): 
% 
%             if(line != "OSM"):
% 
%                 OSM.write(line + "\n")
%       
%         elif(currentMode == "SIM"):
% 
%             if(line != "SIM"): 
% 
%                 SIM.write(line + "\n")
% 
%         elif(currentMode == "RIM"):
% 
%             if(line != "RIM"):
% 
%                 RIM.write(line + "\n")
%                 
%    
%                 
% for k=3:length(Files)
%     
%     currentFile = fopen(Files(k).name, 'r');
%     
%     disp(currentFile)
%     tline = fgetl(currentFile);
%     while ischar(tline)
%         disp(tline)
%         tline = fgetl(currentFile);
%         
%         if(tline == "OSM")
%             
%             currentMode = "OSM";
%             
%         elseif(tline == "SIM")
%                 
%             currentMode = "SIM";    
%                 
%         elseif(tline == "RIM")
%                 
%             currentMode = "RIM";
%             
%         end 
% 
%         if(currentMode == "OSM")
% 
%                 OSM.write(line + "\n")
%       
%         elseif(currentMode == "SIM")
% 
%             if(line != "SIM"): 
% 
%                 SIM.write(line + "\n")
% 
%         elif(currentMode == "RIM"):
% 
%             if(line != "RIM"):
% 
%                 RIM.write(line + "\n")
%         
%     end
%     
%     currentMode = "NONE";
%     
%     
%     
% end
% 
% for k= 1:length(fileList)
% 
% 
%     
% end
%       
% 
% 
% fid1 = fopen('tfest.csv','r'); %# open csv file for reading
% fid2 = fopen('new.csv','w'); %# open new csv file
% while ~feof(fid1)
%     line = fgets(fid1); %# read line by line
%     A = sscanf(line,'%*[^,],%f,%f'); %# sscanf can read only numeric data :(
%     if A(2)<4.185 %# test the values
%         fprintf(fid2,'%s',line); %# write the line to the new file
%     end
% end
% fclose(fid1);
% fclose(fid2);