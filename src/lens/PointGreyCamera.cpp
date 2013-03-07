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

bool lens::PointGreyCamera::open(void)
{	
	unsigned int numCameras;
	if(!_checkLogError(m_busManager.GetNumOfCameras(&numCameras)))
		return false;	// Could not get the number of attached cameras

	if(numCameras < 1)
	  { return false; } //	No Point Grey cameras detected. 

	if(!_checkLogError(m_busManager.GetCameraFromIndex(0, &m_cameraGUID)))
	  { return false; }

	if(!_checkLogError(m_camera.Connect(&m_cameraGUID)))
	  { return false; }

	FlyCapture2::CameraInfo camInfo;
	if(!_checkLogError(m_camera.GetCameraInfo(&camInfo)))
	  { return false; }

	// Power on the camera
	if(!_checkLogError(m_camera.WriteRegister( c_cameraPower, c_cameraPowerValue )))
	  { return false; }
	
	//	Wait for the camera to power up
	unsigned int currentPowerValue = 0;
	do
	{
	  if(!_checkLogError(m_camera.ReadRegister(c_cameraPower, &currentPowerValue)))
		{ return false; }

	} while ((currentPowerValue & c_cameraPowerValue) == 0);


	_setExternalTrigger();
	_setGrabMode();

	if(!_checkLogError(m_camera.StartCapture()))
	  { return false; }

	m_rawImage = make_shared<FlyCapture2::Image>();

	m_convertedImage = shared_ptr<IplImage>(
	  cvCreateImage(cvSize(getWidth(), getHeight()), IPL_DEPTH_8U, 3),
	  [](IplImage* ptr) { cvReleaseImage(&ptr); });

	m_converterImage = make_shared<FlyCapture2::Image>(
	  reinterpret_cast<unsigned char*>(m_convertedImage->imageData), 
	  m_convertedImage->imageSize);
}

bool lens::PointGreyCamera::close(void)
{
	if(!_checkLogError(m_camera.StopCapture()))
	  { return false; } //	Failed to stop the camera

	if(!_checkLogError(m_camera.Disconnect()))
	  { return false; } // Failed to disconnect from the camera

	return true;
}

int lens::PointGreyCamera::getWidth(void)
{
  FlyCapture2::Format7ImageSettings cameraImageSettings;
  unsigned int packetSize = 0;
  float packetPercentage = 0;

  if(!_checkLogError(m_camera.GetFormat7Configuration(&cameraImageSettings, &packetSize, &packetPercentage)))
	{ return 0; }

  return cameraImageSettings.width;
}

int lens::PointGreyCamera::getHeight(void)
{
  FlyCapture2::Format7ImageSettings cameraImageSettings;
  unsigned int packetSize = 0;
  float packetPercentage = 0;

  if(!_checkLogError(m_camera.GetFormat7Configuration(&cameraImageSettings, &packetSize, &packetPercentage)))
	{ return 0; }

  return cameraImageSettings.height;
}

std::string lens::PointGreyCamera::cameraName(void)
{
	return "Point Grey Camera Driver";
}

IplImage* lens::PointGreyCamera::getFrame(void)
{
  m_camera.RetrieveBuffer(m_rawImage.get());
  m_rawImage->Convert(FlyCapture2::PIXEL_FORMAT_RGB, m_converterImage.get());

  return m_convertedImage.get();
}

void lens::PointGreyCamera::_setExternalTrigger(void)
{
	FlyCapture2::Error error;
	//	Setup external trigger
	FlyCapture2::TriggerMode triggerMode;

	if(!_checkLogError(m_camera.GetTriggerMode(&triggerMode)))
	  { return; }
	
	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;
	triggerMode.source = 0;

	if(!_checkLogError(m_camera.SetTriggerMode(&triggerMode)))
	  { return; }
}

void lens::PointGreyCamera::_setGrabMode(void)
{
    FlyCapture2::Error error;

    // Setup buffer grab mode
    FlyCapture2::FC2Config config;

    if(!_checkLogError(m_camera.GetConfiguration(&config)))
	  { return; }

	config.grabMode		= FlyCapture2::BUFFER_FRAMES;
	config.grabTimeout	= FlyCapture2::TIMEOUT_INFINITE;
	config.numBuffers	= 10; // TODO: Not sure if I have to allocate for these

    if(!_checkLogError(m_camera.SetConfiguration(&config)))
	  { return; }
}

bool lens::PointGreyCamera::_checkLogError(FlyCapture2::Error error)
{
  if (error != FlyCapture2::PGRERROR_OK)
	{ return false; } // Log our error and return false

  //  No error, return true
  return true;
}
