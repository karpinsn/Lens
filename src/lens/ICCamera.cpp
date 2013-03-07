/*
 *  ICCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 *
 */

#include "ICCamera.h"

bool lens::ICCamera::open(void)
{
  return false;
}

bool lens::ICCamera::close(void)
{
  return false;
}

int lens::ICCamera::getWidth(void)
{
  return 0;
}

int lens::ICCamera::getHeight(void)
{
  return 0;
}

IplImage* lens::ICCamera::getFrame(void)
{
  return nullptr;
}

std::string lens::ICCamera::cameraName(void)
{
	return "IC Camera Driver";
}
