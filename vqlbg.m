function r = vqlbg(d,k)
e = .01;
r = mean(d, 2);
dpr = 10000;
for i = 1:log2(k)
    r = [r*(1+e), r*(1-e)];
    while (1 == 1)
        z = disteu(d, r);
        [m,ind] = min(z, [], 2);
        t = 0;
        for j = 1:2^i
            r(:, j) = mean(d(:, find(ind == j)), 2); %#ok<FNDSB>
            x = disteu(d(:, find(ind == j)), r(:, j)); %#ok<FNDSB>
            for q = 1:length(x)
                t = t + x(q);
            end
        end
        if (((dpr - t)/t) < e)
            break;
        else
            dpr = t;
        end
    end
end
end
