/*
 *  OpenCVCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 *
 */

#include "OpenCVCamera.h"

lens::OpenCVCamera::OpenCVCamera() : QThread()
{
  m_capture = nullptr;
}

lens::OpenCVCamera::~OpenCVCamera()
{
  if(nullptr != m_capture)
  {
	cvReleaseCapture(&m_capture);
  }
}

void lens::OpenCVCamera::init(void)
{
  m_capture = cvCaptureFromCAM(CV_CAP_ANY);
}

void lens::OpenCVCamera::open(void)
{
  m_running = true;
  this->start(QThread::TimeCriticalPriority);
}

void lens::OpenCVCamera::close(void)
{
  m_running = false;
}

float lens::OpenCVCamera::getWidth(void)
{
  int width = cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
  //  OpenCV has problems with direct show and getting properties. 
  //  Return a hardcoded value if it can't find one
  return width > 0 ? width : 640;
}

float lens::OpenCVCamera::getHeight(void)
{
  int height = cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
  //  OpenCV has problems with direct show and getting properties. 
  //  Return a hardcoded value if it can't find one
  return height > 0 ? height : 480;
}

IplImage* lens::OpenCVCamera::getFrame(void)
{
  return m_capture != nullptr ? cvQueryFrame(m_capture) : nullptr;
}

std::string lens::OpenCVCamera::cameraName(void)
{
	return "OpenCV Camera Driver";
}

void lens::OpenCVCamera::run()
{
  while(m_running)
  {
	if(nullptr != m_capture)
	{
	  IplImage* image = cvQueryFrame(m_capture);
	  notifyObservers(image);
	}
  }
}
