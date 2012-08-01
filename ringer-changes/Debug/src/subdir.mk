################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Bell.cpp \
../src/BellEntity.cpp \
../src/BellSounds.cpp \
../src/Controller.cpp \
../src/Display.cpp \
../src/MainWindow.cpp \
../src/Method.cpp \
../src/main.cpp 

OBJS += \
./src/Bell.o \
./src/BellEntity.o \
./src/BellSounds.o \
./src/Controller.o \
./src/Display.o \
./src/MainWindow.o \
./src/Method.o \
./src/main.o 

CPP_DEPS += \
./src/Bell.d \
./src/BellEntity.d \
./src/BellSounds.d \
./src/Controller.d \
./src/Display.d \
./src/MainWindow.d \
./src/Method.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/atkmm-1.6 -I/usr/include/giomm-2.4 -I/usr/lib/i386-linux-gnu/giomm-2.4/include -I/usr/include/pangomm-1.4 -I/usr/lib/i386-linux-gnu/pangomm-1.4/include -I/usr/include/gtk-3.0 -I/usr/include/cairomm-1.0 -I/usr/lib/i386-linux-gnu/cairomm-1.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gtk-3.0/unix-print -I/usr/include/atk-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/i386-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/i386-linux-gnu/sigc++-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gio-unix-2.0/ -I/usr/include/gtkmm-3.0 -I/usr/lib/gtkmm-3.0/include -I/usr/include/gdkmm-3.0 -I/usr/lib/gdkmm-3.0/include -I/usr/include/opencv -I../src -I/usr/include/OGRE -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


