/*
 *  PhantomCamera.cpp
 */

#include "PhantomCamera.h"

lens::PhantomCamera::PhantomCamera(void)
{
	m_CineBMI = (PBITMAPINFO)&CineBMI;
	m_params = new ACQUIPARAMS;
	m_cameraImage = NULL;
    m_cameraNumber = 0;
}

lens::PhantomCamera::~PhantomCamera()
{
	delete m_params;
}

bool lens::PhantomCamera::open(void)
{
  int registrationStatus = PhLVRegisterClientEx("/home/karpinsn/tmp", NULL, PHCONHEADERVERSION);

  if(registrationStatus < 0)
	{ return false; } // Failed to register with Phantom camera

  float width = getWidth();
  float height = getHeight();
  m_cameraImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
  setFrameRate(36);
  setTrigger(SYNC_EXTERNAL);
  
  return true;
}

bool lens::PhantomCamera::close(void)
{
  PhLVUnregisterClient();
  
  return true;
}

int lens::PhantomCamera::getWidth(void)
{
  PhGetCineParams ( m_cameraNumber, CINE_DEFAULT, m_params, m_CineBMI);
  return m_params->ImWidth;
}

int lens::PhantomCamera::getHeight(void)
{
  PhGetCineParams ( m_cameraNumber, CINE_DEFAULT, m_params, m_CineBMI);
  return m_params->ImHeight;
}

IplImage* lens::PhantomCamera::getFrame(void)
{
  IMRANGE range;
  range.First = 0;
  range.Cnt = 1;
  
  int cineNumber = CINE_CURRENT;
  PhGetImage(m_cameraNumber, &cineNumber, &range, 0, (PBYTE)m_cameraImage->imageData);
	
  return m_cameraImage;
}

void lens::PhantomCamera::setFrameRate(int fps)
{
  PhGetCineParams ( m_cameraNumber, CINE_DEFAULT, m_params, m_CineBMI);
  m_params->FrameRate = fps;	//	Set the framerate
  PhSetCineParams( m_cameraNumber, CINE_DEFAULT, m_params);
}

void lens::PhantomCamera::setTrigger(unsigned int triggerMode)
{
  PhGetCineParams ( m_cameraNumber, CINE_DEFAULT, m_params, m_CineBMI);
  m_params->SyncImaging = triggerMode;	//	Set the trigger mode
  PhSetCineParams( m_cameraNumber, CINE_DEFAULT, m_params);
}

std::string lens::PhantomCamera::cameraName(void)
{
    return "Phantom Camera Driver";
}
