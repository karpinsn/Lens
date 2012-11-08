/*
 *  PointGreyCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2012 ISU. All rights reserved.
 *
 */

#include "PointGreyCamera.h"

lens::PointGreyCamera::PointGreyCamera(void) :
  //  Magic numbers from the point grey camera library
  c_cameraPower(0x610), c_cameraPowerValue(0x80000000)
{ }

void lens::PointGreyCamera::init(void)
{	
	unsigned int numCameras;
	if(!_checkLogError(m_busManager.GetNumOfCameras(&numCameras)))
		return; //	Could not get the number of attached cameras

	if(numCameras < 1)
	{
		//	No Point Grey cameras detected. 
		return;
	}

	if(!_checkLogError(m_busManager.GetCameraFromIndex(0, &m_cameraGUID)))
	  return;
}

void lens::PointGreyCamera::open(void)
{
	if(!_checkLogError(m_camera.Connect(&m_cameraGUID)))
	  return;
	
	FlyCapture2::CameraInfo camInfo;
	if(!_checkLogError(m_camera.GetCameraInfo(&camInfo)))
	  return;

	// Power on the camera
	if(!_checkLogError(m_camera.WriteRegister( c_cameraPower, c_cameraPowerValue )))
	  return;
	
	//	Wait for the camera to power up
	unsigned int currentPowerValue = 0;
	do
	{
	  if(!_checkLogError(m_camera.ReadRegister(c_cameraPower, &currentPowerValue)))
		return;

	} while ((currentPowerValue & c_cameraPowerValue) == 0);


	_setExternalTrigger();

	if(!_checkLogError(m_camera.StartCapture()))
	  return;

	m_thread = new QThread(this);
	PointGreyCameraWorker* worker = new PointGreyCameraWorker(*this);
	worker->moveToThread(m_thread);

	connect(m_thread, SIGNAL(started()), worker, SLOT(getFrame()));
	connect(worker, SIGNAL(finished()), m_thread, SLOT(quit()));
	connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
	connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));

	m_thread->start();
}

void lens::PointGreyCamera::close(void)
{
	m_running = false;
	
	if(!_checkLogError(m_camera.StopCapture()))
	  return; //	Failed to stop the camera

	if(!_checkLogError(m_camera.Disconnect()))
	  return; // Failed to disconnect from the camera
}

float lens::PointGreyCamera::getWidth(void)
{
	return 800.0f;
}

float lens::PointGreyCamera::getHeight(void)
{
	return 600.0f;
}

std::string lens::PointGreyCamera::cameraName(void)
{
	return "Point Grey Camera Driver";
}

void lens::PointGreyCameraWorker::getFrame(void)
{
  //  Used for grabbing the image from the camera
  FlyCapture2::Image rawImage;

  //  Used to convert image to an IplImage for our observers
  shared_ptr<IplImage> convertedImage(
	cvCreateImage(cvSize(m_parent.getWidth(), m_parent.getHeight()), IPL_DEPTH_8U, 3),
	[](IplImage* ptr) { cvReleaseImage(&ptr); });

  FlyCapture2::Image converterImage(
	reinterpret_cast<unsigned char*>(convertedImage->imageData), 
	convertedImage->imageSize);

  FlyCapture2::Error error;
  while(m_running)
  {
	  error = m_parent.m_camera.RetrieveBuffer(&rawImage);
	  error = rawImage.Convert(FlyCapture2::PIXEL_FORMAT_RGB, &converterImage);
	  m_parent.notifyObservers(convertedImage.get());
  }

  emit finished();
}

void lens::PointGreyCameraWorker::stop()
{
  m_running = false;
}

void lens::PointGreyCamera::_setExternalTrigger(void)
{
	FlyCapture2::Error error;
	//	Setup external trigger
	FlyCapture2::TriggerMode triggerMode;

	if(!_checkLogError(m_camera.GetTriggerMode(&triggerMode)))
	  return;
	
	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;
	triggerMode.source = 0;

	if(!_checkLogError(m_camera.SetTriggerMode(&triggerMode)))
	  return;
}

bool lens::PointGreyCamera::_checkLogError(FlyCapture2::Error error)
{
  if (error != FlyCapture2::PGRERROR_OK)
  {
	// Log our error and return false
	return false;
  }
  //  No error, return true
  return true;
}