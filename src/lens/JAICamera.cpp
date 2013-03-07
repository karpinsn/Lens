/*
 *  JAICamera.cpp
 *
 *  Created by Nikolaus Karpinsky on 9/20/10.
 *  Copyright 2010 ISU. All rights reserved.
 *
 */

#include "JAICamera.h"

lens::JAICamera::JAICamera(void)
{
	m_cameraImage = NULL;
}

bool lens::JAICamera::open(void)
{
  	uint32_t  cameraCount;
  	int64_t	  int64Val;
	SIZE	  ViewSize;
	int64_t	  pixelFormat;
	bool8_t	  hasChanged = false;

	if(!_checkSuccess(J_Factory_Open("", &m_factory))) 
	  { return false; } // Unable to open the factory.
	
	// Update camera list
    if (!_checkSuccess(J_Factory_UpdateCameraList(m_factory, &hasChanged)))
	  { return false; } // Could not update the camera list

    // Get the number of Cameras
    if (!_checkSuccess(J_Factory_GetNumOfCameras(m_factory, &cameraCount)))
	  { return false; } // Could not determine the number of cameras
  
    if (cameraCount == 0)
	  { return false; } // There are no cameras
      
    // Get camera ID
    uint32_t tSize = (uint32_t)sizeof(m_cameraID);
    if (!_checkSuccess(J_Factory_GetCameraIDByIndex(m_factory, 0, m_cameraID, &tSize)))
	  { return false; } // Could not get a camera ID

    // Open camera
    if (!_checkSuccess(J_Camera_Open(m_factory, m_cameraID, &m_camera)))
	  { return false; } //	Could not open the camera
	
	// Get Width from the camera
    J_Camera_GetValueInt64(m_camera, NODE_NAME_WIDTH, &int64Val);
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

    // Get Height from the camera
    J_Camera_GetValueInt64(m_camera, NODE_NAME_HEIGHT, &int64Val);
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

    // Get pixelformat from the camera
    J_Camera_GetValueInt64(m_camera, NODE_NAME_PIXELFORMAT, &int64Val);
    pixelFormat = int64Val;

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(pixelFormat);

	// Open stream
    if (!_checkSuccess(J_Image_OpenStream(m_camera, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&lens::JAICamera::streamCallBack), &m_thread, (ViewSize.cx*ViewSize.cy*bpp)/8))) 
	  { return false; } //	Could not open the stream

    // Start Acquision
    J_Camera_ExecuteCommand(m_camera, NODE_NAME_ACQSTART);
}

bool lens::JAICamera::close(void)
{
  if (m_camera)
  {
	// Close camera
	if(!_checkSuccess(J_Camera_Close(m_camera))) 
	  { return false; }
	else
	  { m_camera = nullptr; }
  }
  return true;
}

int lens::JAICamera::getWidth(void)
{
	int64_t width;
	J_Camera_GetValueInt64(m_camera, NODE_NAME_WIDTH, &width);
	return width;
}

int lens::JAICamera::getHeight(void)
{
	int64_t height;
	J_Camera_GetValueInt64(m_camera, NODE_NAME_HEIGHT, &height);
	return height;
}

//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
/* TODO - Old Lens
void lens::JAICamera::streamCallBack(J_tIMAGE_INFO * pAqImageInfo)
{
    // We only want to create the OpenCV Image object once and we want to get the correct size from the Acquisition Info structure
    if (m_cameraImage == NULL)
    {
        // Create the Image:
        // We assume this is a 8-bit monochrome image in this sample
        m_cameraImage = cvCreateImage(cvSize(pAqImageInfo->iSizeX,pAqImageInfo->iSizeY), IPL_DEPTH_8U, 1);
    }

	// Copy the data from the Acquisition engine image buffer into the OpenCV Image obejct
    memcpy(m_cameraImage->imageData, pAqImageInfo->pImageBuffer, m_cameraImage->imageSize);

	notifyObservers(m_cameraImage);
}*/

std::string lens::JAICamera::cameraName(void)
{
	return "JAI Camera Driver";
}

bool lens::JAICamera::_checkSuccess(J_STATUS_TYPE status)
{
  return J_ST_SUCCESS == status;
}