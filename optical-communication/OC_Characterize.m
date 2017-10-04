% function OC_Characterize
%OC_Characterize.m
clear                   % clears variable memory
format compact          % removes extra line feeds in Command Window
global T_UNO R_UNO BR%
tb = [1,2,3,4,5,6,7,8]; % => tb_err = [0.9244 0.9487 0.9294 0.8988 0.7987 0 0 0]
% probability of error
tb_err = zeros(size(tb));

% PREPARE IMAGE
RGB=imread('small_smile.jpg'); % small image used to test
% RGB=imread('rick.jpg');      % 40x40 image
[Rows Columns Colors] = size(RGB);  % get dimensions
GS = uint8(zeros(Rows,Columns));    % cast as unsigned bytes
% convert colored image (RGB) to grayscale (GS)
for ir = 1:Rows
    for ic = 1:Columns
        % commonly used formula (used by Matlab)
            gray = 0.2989*RGB(ir,ic,1) +0.587*RGB(ir,ic,2) +0.114*RGB(ir,ic,3);
            GS(ir,ic) = uint8(gray); % unsigned bytes
    end
end
% display image
subplot(121),imshow(GS), figure(1);
axis 'image'
title('grayscale image')
drawnow;

% iterate through different tb
for i = 1:length(tb)
    % close any open ports
    sopen = instrfind;      % checks for already open serial ports
    if max(size(sopen))>0   % if there are open serial ports
        fclose(sopen);      % they are closed
    end
    disp('Connect to UNOs')
    connectT_UNO_prompt % connect to T_UNO function using prompt from T_UNO
    connectR_UNO_prompt % connect to R_UNO function using prompt from R_UNO

    TB = tb(i); % get current TB value of iteration
    disp(TB);   % display TB
    fprintf(T_UNO, '%d/n', double(TB)); % send TB to T_UNO
    fprintf(R_UNO, '%d/n', double(TB)); % send TB to R_UNO

    GSR = uint8(zeros(size(GS)));   % prepare grayscale image
    for ir=1:Rows
        disp(ir);                   % display ir to help track progress
        for ic = 1:Columns
            fprintf(T_UNO,'%d/n',double(GS(ir,ic))); % send to T_UNO
            val = fscanf(R_UNO,'%d');           % receive from R_UNO
            GSR(ir,ic) =  val;                  % put received val to GSR
        end
    end
    
    % display the received image (GSR)
    subplot(122),imshow(GSR);
    axis 'image'
    title('grayscale image from UNO')
    drawnow;
    
    % CALCULATE PROBABILITY OF ERROR
    err = 0;                            % initialize error counter to 0
    % iterate through rows & columns
    for ir=1:Rows
        for ic = 1:Columns
            if GS(ir,ic) ~= GSR(ir,ic)  % if receivd image doesn't match
                err=err+1;              % increment error
            end
        end
    end
    P_err = err/(ic*ir)                % calculate the probability
    tb_err(i) = P_err;                 % save that in tb_err for plotting
end

fclose(T_UNO) % closes serial connection to T_UNO
fclose(R_UNO) % closes serial connection to R_UNO

% PLOT TB against PE (probability of error)
figure(2)
plot(tb, tb_err, '-o')
xlabel('T_B') % x-axis label
ylabel('P_E') % y-axis label
grid on
% end

function connectT_UNO_prompt
% connects to UNO and waits for prompt (prompt is required)
global T_UNO R_UNO BR%
comNum = 'FD121' ; % T_UNO UNO
BR = 115200;
comStr = ['/dev/cu.usbmodem' comNum]; % concatenate strings
T_UNO = serial(comStr,'BaudRate',BR);  % must be same COM# and BR as Monitor
fopen(T_UNO) % opens serial connection to UNO - resets UNO
UNO_prompt= fscanf(T_UNO,'%c'); % fscanf waits until data terminated with new line occurs
disp(UNO_prompt)
end

function connectR_UNO_prompt
% connects to UNO and waits for prompt (prompt is required)
global T_UNO R_UNO BR %
comNum = 'FA131' ; % R_UNO UNO
comStr = ['/dev/cu.usbmodem' comNum]; % concatenate strings
R_UNO = serial(comStr,'BaudRate',BR);  % must be same COM# and BR as Monitor
fopen(R_UNO) % opens serial connection to UNO - resets UNO
UNO_prompt= fscanf(R_UNO,'%c'); % fscanf waits until data terminated with new line occurs
disp(UNO_prompt)
end