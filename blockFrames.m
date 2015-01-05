function M3 = blockFrames(s, fs, m, n)
l = length(s);
nbFrame = floor((l - n) / m) + 1;
for i = 1:n
    for j = 1:nbFrame
        M(i, j) = s(((j - 1) * m) + i); %#ok<AGROW>
    end
end
h = hamming(n);
M2 = diag(h) * M;
for i = 1:nbFrame
    M3(:, i) = fft(M2(:, i)); %#ok<AGROW>
end
end
