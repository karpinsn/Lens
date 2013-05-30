/*
 *  PointGreyCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2012 ISU. All rights reserved.
 *
 */

#include "PointGreyCamera.h"

lens::PointGreyCamera::PointGreyCamera(unsigned int serialNumber) :
  c_cameraPower(0x610), c_cameraPowerValue(0x80000000), m_frameSetCount(1), m_previousFrameNumber(0), m_serialNumber(serialNumber)
{ }

bool lens::PointGreyCamera::open(void)
{	
	unsigned int numCameras;
	if(!_checkLogError(m_busManager.GetNumOfCameras(&numCameras)))
		return false;	// Could not get the number of attached cameras

	if(numCameras < 1)
	  { return false; } //	No Point Grey cameras detected. 

	// If we have a serial number (not equal to 0) then use the serial number, else just grab the first one
	if(m_serialNumber != 0)
	{
	  if(!_checkLogError(m_busManager.GetCameraFromSerialNumber(m_serialNumber, &m_cameraGUID)))
		{ return false; }
	}
	else
	{
	  if(!_checkLogError(m_busManager.GetCameraFromIndex(0, &m_cameraGUID)))
		{ return false; }
	}
	
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

	// Set the frame counter on so that we can check what frame we are on
	FlyCapture2::EmbeddedImageInfo imageInfo;
	imageInfo.frameCounter.onOff = true;
	if(!_checkLogError(m_camera.SetEmbeddedImageInfo(&imageInfo)))
	  { return false;	}

	if(!_checkLogError(m_camera.StartCapture()))
	  { return false; }

	m_rawImage = make_shared<FlyCapture2::Image>();

	m_convertedImage = shared_ptr<IplImage>(
	  cvCreateImage(cvSize(getWidth(), getHeight()), IPL_DEPTH_8U, 3),
	  [](IplImage* ptr) { cvReleaseImage(&ptr); });

	m_converterImage = make_shared<FlyCapture2::Image>(
	  reinterpret_cast<unsigned char*>(m_convertedImage->imageData), 
	  m_convertedImage->imageSize);

	return true;
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

  // Make sure that we haven't dropped an image.
  unsigned int currentFrameNumber = m_rawImage->GetMetadata().embeddedFrameCounter;
  // So long as the difference is not an entire set, drop frames
  while( (currentFrameNumber - ( m_previousFrameNumber + 1 ) ) % m_frameSetCount )
  {
	cout << "Need to drop a Frame!" << " " << (currentFrameNumber - (m_previousFrameNumber + 1) ) % m_frameSetCount << " " << m_previousFrameNumber << " " << currentFrameNumber << endl;
	m_camera.RetrieveBuffer(m_rawImage.get());
	currentFrameNumber = m_rawImage->GetMetadata().embeddedFrameCounter;
  }

  m_previousFrameNumber = currentFrameNumber;

  // Convert and return!
  m_rawImage->Convert(FlyCapture2::PIXEL_FORMAT_RGB, m_converterImage.get());
  return m_convertedImage.get();
}

void lens::PointGreyCamera::setSerialNumber(const unsigned int serialNumber)
{
  m_serialNumber = serialNumber;
}

void lens::PointGreyCamera::setExternalTrigger(void)
{
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

void lens::PointGreyCamera::setBufferedGrab(int buffers)
{
    // Setup buffer grab mode
    FlyCapture2::FC2Config config;

    if(!_checkLogError(m_camera.GetConfiguration(&config)))
	  { return; }

	config.grabMode		= FlyCapture2::BUFFER_FRAMES;
	config.grabTimeout	= FlyCapture2::TIMEOUT_INFINITE;
	config.numBuffers	= buffers;

    if(!_checkLogError(m_camera.SetConfiguration(&config)))
	  { return; }
}

bool lens::PointGreyCamera::setFormat7(int width, int height, int offsetX, int offsetY)
{
  FlyCapture2::Format7ImageSettings settings;
  settings.mode = FlyCapture2::MODE_0;
  settings.width = width;
  settings.height = height;
  settings.offsetX = offsetX;
  settings.offsetY = offsetY;
	settings.pixelFormat = FlyCapture2::PIXEL_FORMAT_RAW8;

  bool valid;
  FlyCapture2::Format7PacketInfo packetInfo;

  if(!_checkLogError(m_camera.ValidateFormat7Settings(&settings, &valid, &packetInfo)))
	{ return false; }

  if(!valid)
	{ return false; }

	// Have to stop the capture before we can set this 
	// configuration since we are changing the image size
	if(!_checkLogError(m_camera.StopCapture()))
		{ return false; }

  if(!_checkLogError(m_camera.SetFormat7Configuration(
	&settings, packetInfo.recommendedBytesPerPacket)))
	{ return false; }

	// Dont forget to start the capture again
	if(!_checkLogError(m_camera.StartCapture()))
		{ return false; }

  return true;
}

bool lens::PointGreyCamera::setGain(float gain)
{
  FlyCapture2::Property gainProperty;
	gainProperty.type = FlyCapture2::GAIN;

  if(!_checkLogError(m_camera.GetProperty(&gainProperty)))
	{ return false; }

  gainProperty.absValue = gain;
  if(!_checkLogError(m_camera.SetProperty(&gainProperty)))
  { return false; }

  return true;
}

bool lens::PointGreyCamera::setFrameSetCount( unsigned int frameSetCount )
{
  if(frameSetCount < 1)
	{ return false;	}

  m_frameSetCount = frameSetCount;
  return true;
}

bool lens::PointGreyCamera::_checkLogError(FlyCapture2::Error error)
{
  if (error != FlyCapture2::PGRERROR_OK)
	{ return false; } // Log our error and return false

  //  No error, return true
  return true;
}
