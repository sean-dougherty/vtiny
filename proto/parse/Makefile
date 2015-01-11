llvm_home=/usr/lib/llvm-3.4/
llvm_include=${llvm_home}/include/
llvm_lib=${llvm_home}/lib

main: main.cpp
	clang++ -std=c++11 -g -o main -I${llvm_include} -L${llvm_lib} main.cpp -lclang