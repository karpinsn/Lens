/**
@filename
@author	  Nikolaus Karpinsky
Date Created:	09/20/10
Last Edited:	09/20/10
*/

#ifndef _FILE_CAMERA_H_
#define _FILE_CAMERA_H_

#define USE_FILE_CAMERA

#include <qtimer.h>
#include <QFileDialog>

#include <cv.h>
#include <highgui.h>

#include "ICamera.h"

using namespace std;

namespace lens
{
  class FileCamera : public ICamera
  {
	Q_OBJECT
	friend class FileCameraWorker;

  private:
	shared_ptr<CvCapture>		  m_capture;
	string						  m_currentFileName;

  public:
	FileCamera();
	static std::string cameraName(void);

  public slots:
	bool	  open(void);
	bool	  close(void);
	int		  getWidth(void);
	int		  getHeight(void);
	IplImage* getFrame(void);
  };
}

#endif	// _FILE_CAMERA_H_
