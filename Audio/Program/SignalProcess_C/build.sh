/usr/bin/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/home/highdimen/Downloads/Code/Algorithm/SignalProcess -B/home/highdimen/Downloads/Code/Algorithm/SignalProcess/build -G "Unix Makefiles"
/usr/bin/cmake --build /home/highdimen/Downloads/Code/Algorithm/SignalProcess/build --config Debug --target all -j 10 --

