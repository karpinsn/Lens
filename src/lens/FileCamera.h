/**
@filename
@author	  Nikolaus Karpinsky
Date Created:	09/20/10
Last Edited:	09/20/10
*/

#ifndef _FILE_CAMERA_H_
#define _FILE_CAMERA_H_

#include <QThread>
#include <qtimer.h>
#include <QFileDialog>

#include <cv.h>
#include <highgui.h>

#include "Camera.h"

using namespace std;

namespace lens
{

  /**
  * Worker class that is used to notify observers from a background thread that is
  * executing a timer to give precise frame timing. Somewhat convoluted but it is
  * needed so that we create and delete on the same thread as well as offload 
  * this from the UI thread.
  */
  class FileCamera;
  class FileCameraWorker : public QObject
  {
	Q_OBJECT
    private:
	  FileCamera& m_parent;

    public:
	  FileCameraWorker(FileCamera& camera) : m_parent(camera) { };

	public slots:
	  /**
	  * Slot that is called by the timer function in the thread asking for another image
	  */
	  void getFrame();
  };

  /**
  * Actual FileCamera that other classes will use to get a camera that simply 
  * reads from a file
  */
  class FileCamera : public QObject, public Camera
  {
	Q_OBJECT
	friend class FileCameraWorker;

  private:
	shared_ptr<CvCapture>		  m_capture;
	//	Worker that will receive timer signals and then act on them
	shared_ptr<FileCameraWorker>  m_worker;
	string						  m_currentFileName;

	//	Thread and timer that the processing will be done on
	QTimer*						  m_timer;
	QThread*					  m_thread;

  public:
	FileCamera();

	virtual void  init(void);
	virtual void  open(void);
	virtual void  close(void);
	virtual float getWidth(void);
	virtual float getHeight(void);

	static std::string cameraName(void);
  };
}

#endif	// _FILE_CAMERA_H_
