%(26M) https://drive.google.com/open?id=1qdgl0qNxkdzr4n9GpSP5r9st_bNyvESF
filename = 'stereo-88k-24b.flac';

%(204M) https://drive.google.com/open?id=1k6Ucql79cOoexC1bcYLdOudxFQ4OCwSd
%filename = 'MAGNIFICAT_16bits_96kHz.flac';

[y, Fs] = audioread(filename);
nBits=24;
channal = 1;
playerObj = audioplayer(y(:, channal), Fs, nBits);
get(playerObj);
play(playerObj);
plot(y(:, channal));

%stop(playerObj);