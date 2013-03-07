/*
 Filename:		JAICamera.h
 Author:		Nikolaus Karpinsky
 Date Created:	09/20/10
 Last Edited:	09/20/10
 
 Revision Log:
 09/20/10 - Nik Karpinsky - Original creation.
 */

#ifndef _JAI_CAMERA_H_
#define _JAI_CAMERA_H_
#define USE_JAI_CAMERA

#include "ICamera.h"
#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         "Width"
#define NODE_NAME_HEIGHT        "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_ACQSTART      "AcquisitionStart"

using namespace std;

namespace lens
{
    class JAICamera : public ICamera
	{
	  Q_OBJECT

	private:
		FACTORY_HANDLE	m_factory; // Factory Handle
		CAM_HANDLE		m_camera; // Camera Handle
		THRD_HANDLE     m_thread;      // Acquisition Thread Handle
		int8_t          m_cameraID[J_CAMERA_ID_SIZE];    // Camera ID string
		IplImage*		m_cameraImage;

    public:
	  JAICamera(void);
	  static std::string cameraName(void);
	  void streamCallBack(J_tIMAGE_INFO * pAqImageInfo);

	public slots:
	  bool open(void);
      bool close(void);
	  int getWidth(void);
      int getHeight(void);

	private:
	  bool _checkSuccess(J_STATUS_TYPE status);
	};
}

#endif	// _JAI_CAMERA_H_
