#ifndef _LENS_FRAME_GRABBER_H_
#define _LENS_FRAME_GRABBER_H_

#include <string>
#include <vector>

#include <cv.h>
#include <highgui.h>

#include <QObject>
#include <QThread>

#include "ICamera.h"

namespace lens
{
  class ICameraObserver
  {
  public:
    virtual void newFrame(IplImage* frame) = 0;
  };

  // Can't make this private otherwise QT MOC cannot compile it :(
  class FrameGrabberWorker : public QObject
  {
	Q_OBJECT
	friend class FrameGrabber;

  private:
	std::vector<ICameraObserver*> m_observers;
	bool						  m_canRun;
	ICamera&					  m_camera;

  public:
	FrameGrabberWorker(ICamera& camera) : m_camera(camera), m_canRun(true){ };

  public slots:
	void grabFrames(void);

  signals:
	void stopped(void);
  };

  class FrameGrabber : public QObject
  {
	Q_OBJECT

  private:
	QThread*			m_frameGrabberThread;
	FrameGrabberWorker* m_worker;

  public:
	FrameGrabber(ICamera& camera);
	void open(void);
	void close(void);

	void addObserver(ICameraObserver *observer);
  };
}

#endif	// _LENS_FRAME_GRABBER_H_
