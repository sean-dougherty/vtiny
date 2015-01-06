src=$(wildcard src/*.cpp)
hdr=$(wildcard src/*.h)
obj=$(patsubst src/%.cpp, bld/%.o, ${src})
flags=-march=native -std=c++11 -g
libs=-lrt

bld/measure: ${obj} ${hdr}
	${CXX} -o $@ ${obj} ${libs}

bld/%.o: src/%.cpp
	@mkdir -p bld
	${CXX} -c ${flags} ${CXXFLAGS} -o $@ $<

.PHONY: clean
clean:
	@rm -rf bld