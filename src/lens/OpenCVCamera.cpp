/*
 *  OpenCVCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 */

#include "OpenCVCamera.h"

lens::OpenCVCamera::OpenCVCamera() : m_capture(nullptr)
{ }

bool lens::OpenCVCamera::open(void)
{
  m_capture = shared_ptr<CvCapture>(cvCaptureFromCAM(CV_CAP_ANY),
	[] (CvCapture* ptr) { cvReleaseCapture(&ptr); } );

  return m_capture != nullptr;
}

bool lens::OpenCVCamera::close(void)
{
  m_capture.reset();
  return true;
}

int lens::OpenCVCamera::getWidth(void)
{
  if( nullptr == m_capture ) 
	{ return 0; }

  int width = cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_WIDTH);
  // OpenCV has problems with direct show and getting properties. 
  // Return a hardcoded value if it can't find one
  return width > 0 ? width : 640;
}

int lens::OpenCVCamera::getHeight(void)
{
  if( nullptr == m_capture ) 
	{ return 0; }

  int height = cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_HEIGHT);
  // TODO - Fix this
  // OpenCV has problems with direct show and getting properties. 
  // Return a hardcoded value if it can't find one
  return height > 0 ? height : 480;
}

IplImage* lens::OpenCVCamera::getFrame(void)
{
  return m_capture != nullptr ? cvQueryFrame(m_capture.get()) : nullptr;
}

std::string lens::OpenCVCamera::cameraName(void)
{
	return "OpenCV Camera Driver";
}
