/**
 @filename		PhantomCamera.h
 @author		Nikolaus Karpinsky

 Provides a unified Lens interface to phantom cameras
 */

#ifndef _PHANTOM_CAMERA_H_
#define _PHANTOM_CAMERA_H_
#define USE_PHANTOM_CAMERA

#ifdef _WIN32
	#include <windows.h>	//	Needed by windows for typedefs in phcon.h
#endif

#include <iostream>
#include <phcon.h>			//	Phantom camera includes

#include "ICamera.h"

using namespace std;

namespace lens
{
    class PhantomCamera : public ICamera
	{
	Q_OBJECT
	
	private:
		IplImage*		m_cameraImage;

        unsigned int    m_cameraNumber;

		//	Bitmap Info structure used by the camera
		BYTE			CineBMI[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
		LPBITMAPINFO    m_CineBMI;

		//	Acquisition parameters of the camera
		PACQUIPARAMS	m_params;

    public:
      PhantomCamera(void);
	  ~PhantomCamera();

      static std::string cameraName(void);

	public slots:
	  bool 	open(void);
      bool 	close(void);
	  int 	getWidth(void);
      int	getHeight(void);
	  IplImage* getFrame(void);
	  
	  // Phantom specific slots
	  void setFrameRate(int fps);
	  void setTrigger(unsigned int triggerMode);
	};
}

#endif	// _PHANTOM_CAMERA_H_
