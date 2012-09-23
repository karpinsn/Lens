/*
 *  PointGreyCamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2012 ISU. All rights reserved.
 *
 */

#include "PointGreyCamera.h"

lens::PointGreyCamera::PointGreyCamera(void) : QThread()
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
	m_camera.Connect();
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

	error = m_camera.StartCapture();
	this->start();
}

void lens::PointGreyCamera::close(void)
{
	m_running = false;
	FlyCapture2::Error error = m_camera.StopCapture();

	if( error != FlyCapture2::PGRERROR_OK )
    {
		//	Failed to stop the camera
		return;
	}
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
	FlyCapture2::Error error;
	while(m_running)
	{
		error = m_camera.RetrieveBuffer( &m_rawImage );
		//notifyObservers(m_convertedImage);
	}
}