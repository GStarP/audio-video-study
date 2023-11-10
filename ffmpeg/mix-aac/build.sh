mkdir -p dist
rm dist/mixed.aac
touch dist/mixed.aac
g++ mix_aac.cc -o ./dist/mix_aac -lavformat -lavcodec -lavutil -lavfilter
./dist/mix_aac
