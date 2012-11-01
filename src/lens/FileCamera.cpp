/*
 *  FileCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 */

#include "FileCamera.h"

lens::FileCamera::FileCamera()
{
  m_worker = shared_ptr<FileCameraWorker>(new FileCameraWorker(*this));
}

void lens::FileCamera::init(void)
{
  QString file = QFileDialog::getOpenFileName(nullptr, "Select a movie to open", "/", "Movies (*.avi)");

  //	If we dont have a currently selected item then selected the first in the list
  if (nullptr != file && !file.isEmpty())
  {
    m_currentFileName = file.toLocal8Bit().constData();
	m_capture = shared_ptr<CvCapture>(cvCaptureFromAVI(m_currentFileName.c_str()), [](CvCapture* ptr){ cvReleaseCapture(&ptr); });
  }
}

void lens::FileCamera::open(void)
{
  //  Create the thread and its timer
  m_thread = new QThread(this);
  m_timer = new QTimer(0);
  m_timer->setInterval(1000/540);

  //  Connect the thread and its timer 
  connect(m_thread, SIGNAL(started()), m_timer, SLOT(start()));
  connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
  connect(m_thread, SIGNAL(finished()), m_timer, SLOT(stop()));
  connect(m_timer, SIGNAL(timeout()), m_worker.get(), SLOT(getFrame()));

  //  Move ourselves and the timer to the newly created thread
  m_timer->moveToThread(m_thread);
  m_worker->moveToThread(m_thread);

  //  Finally start up the thread
  m_thread->start();
}

void lens::FileCamera::close(void)
{
  m_thread->quit();
  m_thread->wait();
}

float lens::FileCamera::getWidth(void)
{
  if(nullptr != m_capture)
  {
	return cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_WIDTH);
  }

  return 0.0f;
}

float lens::FileCamera::getHeight(void)
{
  if(nullptr != m_capture)
  {
	return cvGetCaptureProperty(m_capture.get(), CV_CAP_PROP_FRAME_HEIGHT);
  }

  return 0.0f;
}

std::string lens::FileCamera::cameraName(void)
{
  return "File Based Camera Driver";
}

void lens::FileCameraWorker::getFrame()
{
  if(nullptr != m_parent.m_capture)
  {
	IplImage* image = cvQueryFrame(m_parent.m_capture.get());
    if(nullptr != image)
    {
      m_parent.notifyObservers(image);
    }
    else
    {
	  // Create a new capture from the same file (loop the file)
      m_parent.m_capture = shared_ptr<CvCapture>(cvCaptureFromAVI(m_parent.m_currentFileName.c_str()), [](CvCapture* ptr){ cvReleaseCapture(&ptr); });
    }
  }
}
