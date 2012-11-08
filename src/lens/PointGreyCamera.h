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
	class PointGreyCamera;
	class PointGreyCameraWorker : public QObject
	{
	  Q_OBJECT
	  private:
		PointGreyCamera& m_parent;
		bool m_running;

	  public:
		PointGreyCameraWorker(PointGreyCamera& camera) : 
			m_parent(camera), m_running(true) { };
		void stop(void);

	  public slots:
		void getFrame(void);

	  signals:
		void finished(void);
	};

    class PointGreyCamera : public QObject, public Camera
	{
	  Q_OBJECT
	  friend class PointGreyCameraWorker;

	private:
		//	Constants used by PointGrey to specify registers and
		//	needed values on their camera
	  	const unsigned int c_cameraPower;
		const unsigned int c_cameraPowerValue;

		FlyCapture2::Camera				m_camera; // Camera Handle
		FlyCapture2::PGRGuid			m_cameraGUID; // Camera GUID
		FlyCapture2::BusManager			m_busManager; // Bus manager for all cameras
		bool							m_running;	  // Thread stuff	

		QThread* m_thread;

    public:
		PointGreyCamera(void);
		virtual void init(void);
		virtual void open(void);
		virtual void close(void);
		virtual float getWidth(void);
		virtual float getHeight(void);

		static std::string cameraName(void);

	private:
	  /**
	  *	Sets the trigger on the camera to be an external trigger.
	  * Before calling this function the camera must be connected
	  * and powered up: i.e. init() should have been called and open()
	  * should have been called. This maybe called from open()
	  */
	  void _setExternalTrigger(void);

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
