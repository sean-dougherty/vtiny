src=$(wildcard src/*.cpp)
hdr=$(wildcard src/*.h)
flags=-march=native -std=c++11 -g
libs=-lrt

bld/main: ${src} ${hdr}
	@mkdir -p bld
	${CXX} ${flags} ${CXXFLAGS} -o $@ ${src} ${libs}

.PHONY: clean
clean:
	@rm -rf bld