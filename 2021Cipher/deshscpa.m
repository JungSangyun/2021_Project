function corr_coef = hscpa(trace_filename,plaintext)
% TraceNum * 8byte
%%%%%%%%% For Test AES %%%%%%%
st{1} = [14	4	13	1	2	15	11	8	3	10	6	12	5	9	0	7;...
         0  15	7	4	14	2	13	1	10	6	12	11	9	5	3	8;...
         4	1	14	8	13	6	2	11	15	12	9	7	3	10	5	0;...
         15	12	8	2	4	9	1	7	5	11	3	14	10	0	6	13];
st{2} = [15	1	8	14	6	11	3	4	9	7	2	13	12	0	5	10;...
    	3	13	4	7	15	2	8	14	12	0	1	10	6	9	11	5;...
		0	14	7	11	10	4	13	1	5	8	12	6	9	3	2	15;...
		13	8	10	1	3	15	4	2	11	6	7	12	0	5	14	9];
st{3} = [10	0	9	14	6	3	15	5	1	13	12	7	11	4	2	8;...
		13	7	0	9	3	4	6	10	2	8	5	14	12	11	15	1;...
		13	6	4	9	8	15	3	0	11	1	2	12	5	10	14	7;...
		1	10	13	0	6	9	8	7	4	15	14	3	11	5	2	12];
st{4} = [7	13	14	3	0	6	9	10	1	2	8	5	11	12	4	15;...
		13	8	11	5	6	15	0	3	4	7	2	12	1	10	14	9;...
		10	6	9	0	12	11	7	13	15	1	3	14	5	2	8	4;...
		3	15	0	6	10	1	13	8	9	4	5	11	12	7	2	14];
st{5} = [2	12	4	1	7	10	11	6	8	5	3	15	13	0	14	9;...
		14	11	2	12	4	7	13	1	5	0	15	10	3	9	8	6;...
		4	2	1	11	10	13	7	8	15	9	12	5	6	3	0	14;...
		11	8	12	7	1	14	2	13	6	15	0	9	10	4	5	3];
st{6} = [12	1	10	15	9	2	6	8	0	13	3	4	14	7	5	11;...
		10	15	4	2	7	12	9	5	6	1	13	14	0	11	3	8;...
		9	14	15	5	2	8	12	3	7	0	4	10	1	13	11	6;...
		4	3	2	12	9	5	15	10	11	14	1	7	6	0	8	13];
st{7} = [4	11	2	14	15	0	8	13	3	12	9	7	5	10	6	1;...
		13	0	11	7	4	9	1	10	14	3	5	12	2	15	8	6;...
		1	4	11	13	12	3	7	14	10	15	6	8	0	5	9	2;...
		6	11	13	8	1	4	10	7	9	5	0	15	14	2	3	12];
st{8} = [13	2	8	4	6	15	11	1	10	9	3	14	5	0	12	7;...
		1	15	13	8	10	3	7	4	12	5	6	11	0	14	9	2;...
		7	11	4	1	9	12	14	2	0	6	10	13	15	3	5	8;...
		2	1	14	7	4	10	8	13	15	12	9	0	3	5	6	11];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% trNum : Set 
% trLength : Correlation-Coefficient WindowSize
trNum = 14000;
trLength = 50002;

% Open plaintext file and Get intermediates
%fid = fopen(plaintext_filename,'r');
%fseek(fid,128,'bof');

target_index = 1;

%for i=1:trNum
%    tmp_ = fread(fid,16,'uint8');
%    plaintext(i) = tmp_(target_index);
%end

%fclose(fid);

% index 1 -> 1 ; 2 -> 2 ; ... ; 256 -> 0 

range = 64; % key guess range : 0~255

%plaintext = sum( dec2bin(double(plaintext)).' == '1' );
zero_check = (double(plaintext == 0))*range;
index_plaintext = plaintext + zero_check;

count=zeros(range,1);
store=zeros(trLength,range);
Sxx = zeros(trLength,1);
Sx = zeros(trLength,1);

% Load trace file
%filename = sprintf(filename_format,0);
%compare = ReadLeCroyBinaryWaveform(filename);

% Conditional Leakage Averaging for each possible x
% Leakage Sx, Sxx processing
for i=1:trNum
    i
    filename = sprintf(trace_filename,i-1);
    trace = ReadLeCroyBinaryWaveform(filename);
    trace = trace.y;
    %trace = trace(startpoint:endpoint-1);
    %trace = trace - mean(trace);
    %trace = trace/10;
    store(:,index_plaintext(i)) = store(:,index_plaintext(i))+trace; 
    %count(index_plaintext(i)) = count(index_plaintext(i))+1;
    Sxx = Sxx + trace.*trace;
    Sx = Sx + trace;
end

%for x=1:range
%    store(:,x) = store(:,x)/count(x);
%end

%fclose(fid);

% Hypotheses Sy and Syy processing
%for index_key=1:range
%    % Make Your Own Model_Function
%    % tmp = model_function(key,plaintext,trNum);
%    %%%%%%%%% For Test AES %%%%%%%
%    key = bitand(index_key,255)
%    tmp = bitxor(key,plaintext);
%    for i=1:trNum
%        tmp(i) = sbox((tmp(i))+1);
%    end
%    tmp = sum( dec2bin(tmp).' == '1' );
%    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Syy = Syy + tmp.*tmp;
%    Sy = Sy + tmp;
%end

t1 = dec2bin(plaintext(:,1) , 8);
t2 = dec2bin(plaintext(:,2) , 8);
t3 = dec2bin(plaintext(:,3) , 8);
t4 = dec2bin(plaintext(:,4) , 8);
t5 = dec2bin(plaintext(:,5) , 8);
t6 = dec2bin(plaintext(:,6) , 8);
t7 = dec2bin(plaintext(:,7) , 8);
t8 = dec2bin(plaintext(:,8) , 8);
mer = horzcat(t1,t2,t3,t4,t5,t6,t7,t8);

temp  = fInitalPermutation(mer ,trNum);

tempR = temp(:,[33:64]);
Value48 = fFExpansionPermutation(tempR ,trNum);
block1 = bin2dec(Value48(1:6));



test = zeros(7, trNum);

% Hypotheses Sy and Syy processing and Correlations Processing
for index_key=1:range
    i
    Syy = zeros(range,1);
    Sy = zeros(range,1);
    Sxy = zeros(trLength,1);
    for index_x=1:range
        % Make Your Own Model_Function
        %Sxy = Sxy + (model_function(key,x,1)*store(:,x));
        %%%%%%%%% For Test AES %%%%%%%
        key = bitand(index_key,63);
        x = bitand(index_x,63);
        tmp = bitxor(key,x);
        tmp = horzcat( tmp, test);
        tmp = dec2bin(tmp , 6);
        tmp = fFSBoxSubstitution(tmp , trNum);
        tmp = sum((tmp(:,[1:4])).' == '1' );
        Sxy = Sxy + tmp*store(:,index_x);
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    end

    % Make Your Own Model_Function
    % tmp = model_function(key,plaintext,trNum);
    %%%%%%%%% For Test AES %%%%%%%

    key = bitand(index_key,63);
    tmpy = bitxor(key,block1);
    tmpy = fFSBoxSubstitution(tmpy , trNum);
    tmpy = sum((tmpy(:,[1:4])).' == '1' );
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    Syy = sum(tmpy.*tmpy);
    Sy = sum(tmpy);
    a = trNum*Sxy-Sx*Sy;
    b = trNum*Sxx-(Sx.*Sx);
    c = trNum*Syy-Sy*Sy;
    corr_coef(:,index_key) = a./sqrt(b*c);
end

end
