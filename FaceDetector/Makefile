all:VideoFrames.o DetectedResults.o AbstractDetector.o SensesDetector.o main.exe

CC=g++
CXX_FLAGS=-std=c++11

INCLUDE_DIR=D:/OpenCV/opencv/build3.4.5/install/include
LIB_DIR=D:/OpenCV/opencv/build3.4.5/install/x64/mingw/lib
LIBS=-llibopencv_highgui345 -llibopencv_core345 -llibopencv_video345 -llibopencv_videoio345 -llibopencv_face345 \
	 -llibopencv_rgbd345 -llibopencv_imgproc345 -llibopencv_tracking345 -llibopencv_objdetect345

VideoFrames.o:VideoFrames.h VideoFrames.cpp
	$(CC) $(CXX_FLAGS) -c $^ -I $(INCLUDE_DIR)
DetectedResults.o:DetectedResults.h DetectedResults.cpp
	$(CC) $(CXX_FLAGS) -c $^ -I $(INCLUDE_DIR)
AbstractDetector.o:AbstractDetector.h AbstractDetector.cpp
	$(CC) $(CXX_FLAGS) -c $^ -I $(INCLUDE_DIR)
SensesDetector.o:SensesDetector.h SensesDetector.cpp
	$(CC) $(CXX_FLAGS) -c $^ -I $(INCLUDE_DIR)
main.exe:main.cpp VideoFrames.o DetectedResults.o AbstractDetector.o SensesDetector.o
	$(CC) $(CXX_FLAGS) $^ -o $@ -I $(INCLUDE_DIR) -L $(LIB_DIR) $(LIBS)

.PHONY clean:
clean:
	del *.o
	del *.exe
	del *.gch