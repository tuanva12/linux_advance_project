# CXX = /home/caphuong/linux_trainning/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
# CXX = g++
EXEC = exe_project

OBJS += main.o
OBJS += romconfig.o
OBJS += app_mqtt.o

 # build the executable.
${EXEC}: ${OBJS}
	${CXX} -o ${EXEC} ${OBJS} -lpaho-mqttpp3 -lpaho-mqtt3as -lpthread
	mv ${EXEC} ./build
	mv ${OBJS} ./build
	echo Run cmd ./build/${EXEC} for run application.

romconfig.o: romconfig.cpp romconfig.h
	${CXX} -c romconfig.cpp

app_mqtt.o: app_mqtt.cpp app_mqtt.h
	${CXX} -c app_mqtt.cpp

main.o: main.cpp romconfig.h app_mqtt.h
	${CXX} -c main.cpp

clean:
	rm -f ${EXEC} ${OBJS}