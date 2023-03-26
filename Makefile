BUILD_TYPE = Debug
# BUILD_TYPE = Relese

do_cmake_linux:
	rm -rf build && \
	mkdir build &&	\
	cd build &&	\
	cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} && \
	cmake --build . && \
	ctest --verbose