/*
 Filename:		ICCamera.h
 Author:		Nikolaus Karpinsky
 Date Created:	09/20/10
 Last Edited:	09/20/10
 
 Revision Log:
 09/20/10 - Nik Karpinsky - Original creation.
 */

#ifndef _IC_CAMERA_H_
#define _IC_CAMERA_H_

#define USE_IC_CAMERA
#include "ICamera.h"

using namespace std;

namespace lens
{
    class ICCamera : public ICamera
	{
	  Q_OBJECT

    private:
      //DShowLib::Grabber        m_grabber; // The instance of the Grabber class.

    public:
	  static std::string cameraName(void);

	public slots:
	  bool		open(void);
	  bool		close(void);
	  int		getWidth(void);
	  int		getHeight(void);
	  IplImage* getFrame(void);
	};
}

#endif	// _IC_CAMERA_H_
