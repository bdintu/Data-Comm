%download link https://goo.gl/XhKvsx
filename = 'MAGNIFICAT_16bits_96kHz.flac';
[y, Fs] = audioread(filename);
nBits=24;
playerObj = audioplayer(y(:, 1), Fs, nBits);
get(playerObj);
play(playerObj);
%plot(y(:, 1));
%stop(playerObj);