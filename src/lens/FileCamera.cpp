/*
 *  FileCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 */

#include "FileCamera.h"

lens::FileCamera::FileCamera()
{ }

bool lens::FileCamera::open(void)
{
  QString file = QFileDialog::getOpenFileName(nullptr, "Select a movie to open", "/", "Movie (*.avi *.mp4)");

  //	If we dont have a currently selected item then selected the first in the list
  if (nullptr != file && !file.isEmpty())
  {
    m_currentFileName = file.toLocal8Bit().constData();
	m_capture = shared_ptr<CvCapture>(cvCaptureFromAVI(m_currentFileName.c_str()), [](CvCapture* ptr){ cvReleaseCapture(&ptr); });
  }

  return nullptr != m_capture;
}

bool lens::FileCamera::close(void)
{
  return true;
}

int lens::FileCamera::getWidth(void)
{
  if(nullptr == m_capture)
	{ return 0; }
	
  return cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_WIDTH);
}

int lens::FileCamera::getHeight(void)
{
  if(nullptr == m_capture)
	{ return 0; }

  return cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_HEIGHT);
}

std::string lens::FileCamera::cameraName(void)
{
  return "File Based Camera Driver";
}

IplImage* lens::FileCamera::getFrame(void)
{
  if( nullptr == m_capture )
	{ return nullptr; }
  
  IplImage* image = cvQueryFrame(m_capture.get());
  if(nullptr != image)
	{ return image; }

  // If here, we have hit the end of the file. Reloop and send the image
  // Create a new capture from the same file (loop the file)
  m_capture = shared_ptr<CvCapture>(cvCaptureFromAVI(m_currentFileName.c_str()), [](CvCapture* ptr){ cvReleaseCapture(&ptr); });
  return cvQueryFrame(m_capture.get());
}