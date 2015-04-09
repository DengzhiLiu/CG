#ifndef _CDIB_H_
#define _CDIB_H_

#pragma once
#include "Definitions.h"

class CDIB
{
public:
	int width, height;
	u_char *image;

public:
	CDIB(void);
	~CDIB(void);
	bool readBMP(char *filename);
	void buildMap();
};

#endif