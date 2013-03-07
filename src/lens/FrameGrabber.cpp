#include "FrameGrabber.h"

lens::FrameGrabber::FrameGrabber(ICamera& camera)
{
  m_frameGrabberThread = new QThread(this);
  m_worker = new FrameGrabberWorker(camera);
  m_worker->moveToThread(m_frameGrabberThread);

  // Wire up our signals and slots so that we grab frames till we are stopped
  // Once stopped, call deleteLater to clean up our memory
  connect(m_frameGrabberThread, SIGNAL(started()), m_worker, SLOT(grabFrames()));
  connect(m_worker, SIGNAL(stopped()), m_frameGrabberThread, SLOT(quit()));
  connect(m_frameGrabberThread, SIGNAL(finished()), m_frameGrabberThread, SLOT(deleteLater()));
  connect(m_worker, SIGNAL(stopped()), m_worker, SLOT(deleteLater()));
}

void lens::FrameGrabber::open(void)
{
  m_frameGrabberThread->start(QThread::TimeCriticalPriority);
}

void lens::FrameGrabber::close(void)
{
  // Let the worker know that he cannot work anymore. This should
  // escape the main frame grabbing loop and close down the thread
  m_worker->m_canRun = false;
}

void lens::FrameGrabber::addObserver(ICameraObserver *observer)
{
  m_worker->m_observers.push_back(observer);
}

// ------------------ FrameGrabberWorker stuff -----------------------
void lens::FrameGrabberWorker::grabFrames(void)
{
  // While we can run, continously grab frames and send them to the observers
  while(m_canRun)
  {
	auto frame = m_camera.getFrame();

	for(auto itr = m_observers.begin(); itr < m_observers.end(); ++itr)
	{
	  (*itr)->newFrame(frame);
	}
  }

  emit stopped();
}