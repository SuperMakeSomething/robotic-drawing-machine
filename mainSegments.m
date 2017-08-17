%{ 
    MATLAB Code for Continuous Line Drawing Robot (Continuous Line Drawing Machine - Super Make Something Episode 13
    by: Alex - Super Make Something
    date: January 15, 2017
    license: Creative Commons - Attribution - Non-Commercial.  More information available at: http://creativecommons.org/licenses/by-nc/3.0/
%}

% Code utilizes "floydHalftone.m" method written by Athi available at: https://www.mathworks.com/matlabcentral/fileexchange/25302-image-halftoning-by-floyd-s-method 

clc;
clear all;
close all;

regionSize=5; %Region size

INPUT = 'image.jpg'; %Load image
in_img = imread(INPUT);
in_img = rgb2gray(in_img);

[sizeX,sizeY] = size(in_img); %Check if image is square
if sizeX~=sizeY
    disp('Input image is not square.')
    disp('Please load a square image.')
    return
end

in_img = imresize(in_img,[280 280]); %Resize image

%Show input image
figure(1); 
imshow(in_img);
title('Input Image');

figure(2);
halftone_img=floydHalftone(in_img);
imshow(halftone_img);
title('Halftone Image');

%Initialize path arrays
xPath=[0];
yPath=[0];

for j = 1:2*regionSize:size(in_img,2) %Process image via regions using nearest neighbor search
    for k = 1:regionSize:size(in_img,1) %Odd rows
        
        halftone_region = floydHalftone(in_img(j:j+regionSize-1, k:k+regionSize-1)); %Convert region to halftone
        [x,y]=find(halftone_region==0); %Find locations of non-zero elements
        
        xPathRegion=ones(size(x)); %Pre-allocate local region arrays for speed
        yPathRegion=ones(size(y));
        
        idxNN=knnsearch([x y],[0 0]); % Use NN to top left corner as region path start
        
        if isempty(idxNN) %Error case for purely white regions
            xPathRegion=0;
            yPathRegion=0;
        else        
            xPathRegion(1)=x(idxNN); 
            yPathRegion(1)=y(idxNN);
            i=2;
            while size(x,1)>2
                idxNN=knnsearch([x y],[xPathRegion(i-1) yPathRegion(i-1)]); %Find index of NN
                xPathRegion(i)=x(idxNN); %Add visited location to path
                yPathRegion(i)=y(idxNN);
                x(idxNN)=[]; %Remove visited location from array of unvisited points
                y(idxNN)=[];
                i=i+1;
            end
        end
        
        xPathRegion=xPathRegion+j; %Offset local path for global location in image
        yPathRegion=yPathRegion+k;
        
        xPath=[xPath xPathRegion']; %Append neighborhood path to total path
        yPath=[yPath yPathRegion'];
        
        figure(3) %Plot current total path as animation
        plot(yPath,-xPath,'k');
        title('Pen Path');
        axis([0 size(in_img,2) -size(in_img,1) 0]);  
        drawnow;
    end
    
    j = j+regionSize; %Increment vertical counter
    
    for k = size(in_img,1)-1:-regionSize:1 %Even rows
        halftone_region = floydHalftone(in_img(j:j+regionSize-1, k-regionSize+2:k));
        [x,y]=find(halftone_region==0);
        xPathRegion=ones(size(x));
        yPathRegion=ones(size(y));
        
        idxNN=knnsearch([x y],[0 size(in_img,1)]); %Use NN to top right corner as region path start
        
        if isempty(idxNN)
            xPathRegion=0;
            yPathRegion=0;
        else
            xPathRegion(1)=x(idxNN);
            yPathRegion(1)=y(idxNN);
            i=2;
            while size(x,1)>2
                idxNN=knnsearch([x y],[xPathRegion(i-1) yPathRegion(i-1)]); 
                xPathRegion(i)=x(idxNN);
                yPathRegion(i)=y(idxNN);
                x(idxNN)=[];
                y(idxNN)=[];                
                i=i+1;
            end
        end
        
        xPathRegion=xPathRegion+j;
        yPathRegion=k-yPathRegion;
        
        xPath=[xPath xPathRegion'];
        yPath=[yPath yPathRegion'];
        
        figure(3)
        plot(yPath,-xPath,'k');
        title('Pen Path');
        axis([0 size(in_img,2) -size(in_img,1) 0]);
        drawnow;
    end

end

    figure(3); %Plot total path
    plot(yPath,-xPath,'k');
    axis([0 size(in_img,2) -size(in_img,1) 0]);
    drawnow;
    
% Calculate relative displacement between neighboring path points for stepper motor commands and save to text file
xPath=-xPath;
yPath=yPath;

xPathRel=diff(xPath);
yPathRel=diff(yPath);

textFile=fopen('drawing.txt','w'); %Create new file called drawing.txt

for i=1:1:size(xPathRel,2) %Write alternativing X,Y waypoints on new lines
    fprintf(textFile, '%d\n', yPathRel(i));
    fprintf(textFile, '%d\n', xPathRel(i));
end

fclose(textFile); %Close the text file