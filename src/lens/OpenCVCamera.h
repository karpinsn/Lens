/**
  @filename
  @author	  Nikolaus Karpinsky
  Date Created:	09/20/10
  Last Edited:	09/20/10
 */

#ifndef _OPEN_CV_CAMERA_H_
#define _OPEN_CV_CAMERA_H_

#include <memory>
#include <cv.h>
#include <highgui.h>

#include "ICamera.h"

using namespace std;

namespace lens
{
	class OpenCVCamera : public ICamera
	{
	  Q_OBJECT

	private:
	  shared_ptr<CvCapture> m_capture;

	public:
	  OpenCVCamera();
	  static std::string cameraName(void);

	public slots:
	  bool		open(void);
	  bool		close(void);
	  int		getWidth(void);
	  int		getHeight(void);
	  IplImage* getFrame(void);
	};
}

#endif	// _OPEN_CV_CAMERA_H_
