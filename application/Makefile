# CXX = /home/caphuong/linux_trainning/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
# CXX = g++
EXEC = appadvance

OBJS += main.o
OBJS += romconfig.o
 # build the executable.
${EXEC}: ${OBJS}
	${CXX} -o ${EXEC} ${OBJS}

romconfig.o: romconfig.cpp romconfig.h
	${CXX} -c romconfig.cpp

main.o: main.cpp romconfig.h
	${CXX} -c main.cpp

clean:
	rm -f ${EXEC} ${OBJS}