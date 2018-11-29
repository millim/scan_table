cmake_minimum_required(VERSION 2.6)
project(aiReader)
add_compile_options(-std=c++11)
# add_compile_options(-lstdc++)
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
add_definitions("-Wno-error=narrowing  -w")
set(CMAKE_MACOSX_RPATH 1)
# set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
# find_library (LIBSTDCXX NAMES stdc++)
# add_compile_options(-stdlib=libstdc++)
set(OpenCV_DIR /usr/local/Cellar/opencv/3.4.1_2/share/OpenCV)
find_package(OpenCV 3 REQUIRED)

include_directories(
/usr/local/include/
/usr/local/Cellar/jsoncpp/
/usr/local/Cellar/opencv/3.4.1_2/include/
/usr/local/Cellar/opencv/3.4.1_2/lib
/usr/local/Cellar/jsoncpp/1.8.4/include/json/
/usr/local/Cellar/jsoncpp/1.8.4/include/
/usr/local/Cellar/opencv/3.4.1_2/include/opencv/
/opt/X11/include/
)
link_directories(
  /usr/local/lib
  /opt/X11/lib
  /usr/local/Cellar/jsoncpp/1.8.4/lib
)

add_executable(Demo demo.cpp)
target_link_libraries(Demo 
-lX11
-lpthread
${OpenCV_LIBS}
jsoncpp
)


