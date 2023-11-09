mkdir -p dist
g++ show_metadata.cc -o ./dist/show_metadata -lavformat -lavcodec -lavutil
./dist/show_metadata
