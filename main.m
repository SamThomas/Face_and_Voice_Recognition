function main(testdir,code_number)
% Speaker Recognition: Testing Stage
%
% Input:
%       testdir : string name of directory contains all test sound files
%       n       : number of test files in testdir
%       code    : codebooks of all trained speakers
%
% Note:
%       Sound files in testdir is supposed to be: 
%               s1.wav, s2.wav, ..., sn.wav
%
% Example:
%       >> test('C:\data\test\', 8, code);


if code_number == 1 
load gareth_code code;
pathWriter = "resultat_gareth";
end
if code_number == 2
 load damien_code code;
pathWriter = "resultat_damien";
end
if code_number == 3
 load alexandre_code code;
pathWriter = "resultat_alexandre";
end
if code_number == 4
 load samuel_code code ;
pathWriter = "resultat_samuel";
end

%for k = 1:n                     % read test sound file of each speaker


    file = sprintf('%s.wav', testdir);
    [s, fs] = wavread(file);      
        
    v = mfcc(s, fs);            % Compute MFCC's

    distmin = inf;
    k1 = 0;

    
   
    for l = 1:length(code)      % each trained codebook, compute distortion
        d = disteu(v, code{l}); 
        dist = sum(min(d,[],2)) / size(d,1);
      
        if dist < distmin
            distmin = dist;
            k1 = l;
	    
        end      
    end

    %Ecriture dans fichier txt

    fileTowrite = fopen(pathWriter,'w');
    fprintf(fileTowrite,"%f\n",distmin);    
    
    
    
%end
