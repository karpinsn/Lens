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

#include <QThread>
#include <FlyCapture2.h>
#include "Camera.h"

#define NODE_NAME_WIDTH         "Width"
#define NODE_NAME_HEIGHT        "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_ACQSTART      "AcquisitionStart"

using namespace std;

namespace lens
{
    class PointGreyCamera : public Camera, QThread
	{
	private:
		FlyCapture2::Camera			m_camera; // Camera Handle
		FlyCapture2::PGRGuid		m_cameraGUID; // Camera GUID
		FlyCapture2::BusManager		m_busManager;
		bool						m_running;
		FlyCapture2::Image			m_rawImage;
		IplImage*					m_convertedImage;
		

    public:
		PointGreyCamera(void);
		virtual void init(void);
		virtual void open(void);
		virtual void close(void);
		virtual float getWidth(void);
		virtual float getHeight(void);

		static std::string cameraName(void);

	protected:
		void run();
	};
}

#endif	// _POINT_GREY_CAMERA_H_
