/usr/bin/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/home/highdimen/Algorithm/signal -B/home/highdimen/Algorithm/signal/build -G "Unix Makefiles"
/usr/bin/cmake --build /home/highdimen/Algorithm/signal/build --config Debug --target all -j 10 --

