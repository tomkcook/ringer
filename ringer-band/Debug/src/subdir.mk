################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BandWindow.cpp \
../src/Figure.cpp \
../src/KinectInterface.cpp \
../src/OgreWidget.cpp \
../src/SkeletonPositions.cpp \
../src/main.cpp 

OBJS += \
./src/BandWindow.o \
./src/Figure.o \
./src/KinectInterface.o \
./src/OgreWidget.o \
./src/SkeletonPositions.o \
./src/main.o 

CPP_DEPS += \
./src/BandWindow.d \
./src/Figure.d \
./src/KinectInterface.d \
./src/OgreWidget.d \
./src/SkeletonPositions.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/OGRE -I/usr/include/ni -I/usr/include/gtkmm-2.4 -I/usr/lib/i386-linux-gnu/gtkmm-2.4/include -I/usr/include/atkmm-1.6 -I/usr/include/giomm-2.4 -I/usr/lib/i386-linux-gnu/giomm-2.4/include -I/usr/include/pangomm-1.4 -I/usr/lib/i386-linux-gnu/pangomm-1.4/include -I/usr/include/gtk-2.0 -I/usr/include/gtk-unix-print-2.0 -I/usr/include/gdkmm-2.4 -I/usr/lib/i386-linux-gnu/gdkmm-2.4/include -I/usr/include/atk-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/i386-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/i386-linux-gnu/sigc++-2.0/include -I/usr/include/cairomm-1.0 -I/usr/lib/i386-linux-gnu/cairomm-1.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/lib/i386-linux-gnu/gtk-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0/ -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


