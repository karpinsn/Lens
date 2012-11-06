/*
 *  PointGreyCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2012 ISU. All rights reserved.
 *
 */

#include "PointGreyCamera.h"

lens::PointGreyCamera::PointGreyCamera(void) : QThread(),
  //  Magic numbers from the point grey camera library
  c_cameraPower(0x610), c_cameraPowerValue(0x80000000)
{
	m_convertedImage = NULL;
}

void lens::PointGreyCamera::init(void)
{
	FlyCapture2::Error error;
	
	unsigned int numCameras;
	error = m_busManager.GetNumOfCameras(&numCameras);
	if(error != FlyCapture2::PGRERROR_OK)
	{
		//	Could not get the number of attached cameras
		return;
	}

	if(numCameras < 1)
	{
		//	No Point Grey cameras detected. 
		return;
	}

	error = m_busManager.GetCameraFromIndex(0, &m_cameraGUID);
	if(error != FlyCapture2::PGRERROR_OK)
	{
		return;
	}
}

void lens::PointGreyCamera::open(void)
{
	FlyCapture2::Error error;
	error = m_camera.Connect(&m_cameraGUID);
	if(error != FlyCapture2::PGRERROR_OK)
	{
		return;
	}

	FlyCapture2::CameraInfo camInfo;
	error = m_camera.GetCameraInfo(&camInfo);
	if(error != FlyCapture2::PGRERROR_OK)
	{
		return;
	}

	// Power on the camera
	error  = m_camera.WriteRegister( c_cameraPower, c_cameraPowerValue );
	//	Wait for the camera to power up
	unsigned int currentPowerValue = 0;
	do
	{
	  if (error != FlyCapture2::PGRERROR_OK)
	  {
	  	return;
	  }
	  error = m_camera.ReadRegister(c_cameraPower, &currentPowerValue);
	} while ((currentPowerValue & c_cameraPowerValue) == 0);


	_setExternalTrigger();

	error = m_camera.StartCapture();
	this->start();
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
	return 1280.0f;
}

float lens::PointGreyCamera::getHeight(void)
{
	return 1024.0f;
}

std::string lens::PointGreyCamera::cameraName(void)
{
	return "Point Grey Camera Driver";
}

void lens::PointGreyCamera::run()
{
  //  Used for grabbing the image from the camera
  FlyCapture2::Image rawImage;

  //  Used to convert image to an IplImage for our observers
  shared_ptr<IplImage> convertedImage(
	cvCreateImage(cvSize(getWidth(), getHeight()), IPL_DEPTH_8U, 3),
	[](IplImage* ptr) { cvReleaseImage(&ptr); });

  FlyCapture2::Image converterImage(
	reinterpret_cast<unsigned char*>(convertedImage->imageData), 
	convertedImage->imageSize);

  FlyCapture2::Error error;
  while(m_running)
  {
	  error = m_camera.RetrieveBuffer(&rawImage);
	  error = rawImage.Convert(FlyCapture2::PIXEL_FORMAT_RGB, &converterImage);
	  notifyObservers(convertedImage.get());
  }
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