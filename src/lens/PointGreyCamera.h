/*
Filename:		PointGreyCamera.h
Author:		Nikolaus Karpinsky
Date Created:	09/11/12
Last Edited:	09/11/12

Revision Log:
09/11/12 - Nik Karpinsky - Original creation.
*/

#ifndef _POINT_GREY_CAMERA_H_
#define _POINT_GREY_CAMERA_H_
#define USE_POINT_GREY_CAMERA

#include <memory>

#include <QThread>
#include <FlyCapture2.h>
#include "ICamera.h"

#define NODE_NAME_WIDTH         "Width"
#define NODE_NAME_HEIGHT        "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_ACQSTART      "AcquisitionStart"

using namespace std;

namespace lens
{
  class PointGreyCamera : public ICamera
  {
	Q_OBJECT

  private:
	//	Constants used by PointGrey to specify registers and
	//	needed values on their camera
	const unsigned int c_cameraPower;
	const unsigned int c_cameraPowerValue;

	FlyCapture2::Camera				m_camera; // Camera Handle
	FlyCapture2::PGRGuid			m_cameraGUID; // Camera GUID
	FlyCapture2::BusManager			m_busManager; // Bus manager for all cameras

	shared_ptr<FlyCapture2::Image>	m_rawImage;
	shared_ptr<FlyCapture2::Image>  m_converterImage;
	shared_ptr<IplImage>			m_convertedImage;

	unsigned int m_serialNumber;		// No serial number is represented as 0
	unsigned int m_previousFrameNumber;
	unsigned int m_frameSetCount;

  public:
	PointGreyCamera(const unsigned int serialNumber = 0);

	static string cameraName(void);

  public slots:
	bool	  open(void);
	bool	  close(void);
	int		  getWidth(void);
	int		  getHeight(void);
	IplImage* getFrame(void);

   /**
	* Sets the serial number of the camera to use. This function must be called
	* before connecting and powering up the camera: i.e. open() as it is used
	* to specify which camera should be loaded
	*/
	void setSerialNumber(const unsigned int serialNumber);

   /**
	* Sets the trigger on the camera to be an external trigger.
	* Before calling this function the camera must be connected
	* and powered up: i.e. open() should have been called.
	*/
	void setExternalTrigger(void);

   /**
	* Sets the grab mode on the camera to buffer frames
	* so that we dont drop any (hopefully). Before calling
	* this function the camera must be connected and powered up: i.e.
	* open() should have been called.
	*/
	void setBufferedGrab(int buffers);

   /**
	* Sets the format of the camera to use Format7, which is a custom
	* sized output format. Specifying the width, height, and offsets will
	* determine where the images is grabbed from the CCD.
	*/
	bool setFormat7(int width, int height, int offsetX, int offsetY);

   /**
	* Sets the gain to the specified value.
	*/
	bool setGain(float gain);

   /**
	* Sets the frame set count, which is the number of frames belonging to a set. 
	* If a frame has to be dropped due to processing, it will drop the number of frames
	* in a set, so that correct ordering can be maintained.
	*/
	bool setFrameSetCount( unsigned int frameSetCount );

  private:
   /**
	* Checks the passed in error to see if it is an error. If it
	*	is an error then it will log the error and return false.
	* If there is no error then it will just return true.
	*
	*	@params error - Error to be checking / logging
	*	@returns True or false whether or not there was an error
	*/
	bool _checkLogError(FlyCapture2::Error error);
  };
}

#endif	// _POINT_GREY_CAMERA_H_
