do_cmake:
	rm -rf build && \
	mkdir build &&	\
	cd build &&	\
	cmake .. &&	\
	cmake --build . && \
	ctest --verbose