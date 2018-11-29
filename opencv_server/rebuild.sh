if [ -f "./cmake_install.cmake" ]
then
  echo "删除cmake_install.cmake"
  rm ./cmake_install.cmake
fi
if [ -f "./CMakeCache.txt" ]
then
  echo "删除CMakeCache.txt"
  rm ./CMakeCache.txt
fi
if [ -f "./Makefile" ]
then
  echo "删除Makefile"
  rm ./Makefile
fi

cmake ./
make
echo "执行结束<---------"