CXX = /home/caphuong/linux_trainning/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
# CXX = g++

TARGET_EXEC := exe_project

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
# DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
# INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
# INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
# CPPFLAGS := $(INC_FLAGS) -MMD -MP

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ -lpthread -lcurl -lssl -lcrypto -l:libv4l2.a -l:libjpeg.a -ldl -lpaho-mqttpp3 -lpaho-mqtt3a -L./ -L/home/caphuong/my_rootfs/usr/local/lib/
# scp ./build/exe_project debian@192.168.15.2:/home/debian

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ -I./library/json/include -I./library/paho.mqtt.c/src -I./library/paho.mqtt.cpp/src -I/home/caphuong/my_rootfs/usr/local/include/


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)












































# EXEC = exe_project

# OBJS += main.o
# OBJS += romconfig.o
# OBJS += app_mqtt.o

#  # build the executable.
# ${EXEC}: ${OBJS}
# 	${CXX} -o ${EXEC} ${OBJS} -lpaho-mqttpp3 -lpaho-mqtt3as -lpthread
# 	mv ${EXEC} ./build
# 	mv ${OBJS} ./build
# 	echo Run cmd ./build/${EXEC} for run application.

# romconfig.o: romconfig.cpp romconfig.h
# 	${CXX} -c romconfig.cpp

# app_mqtt.o: app_mqtt.cpp app_mqtt.h
# 	${CXX} -c app_mqtt.cpp

# main.o: main.cpp romconfig.h app_mqtt.h
# 	${CXX} -c main.cpp

# clean:
# 	rm -f ${EXEC} ${OBJS}