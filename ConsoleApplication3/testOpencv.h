#pragma once


#include <iostream>
#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
class testOpencv {
#ifndef __debug


private:
	
public:
	
	testOpencv();
	//转发调用字符串
	char* paststr(char* leftImage, char* rightImage, char* templateImage);
	
	~testOpencv();
#endif // !__debug
};




