/*
Filename:		Camera.h
Author:			Nikolaus Karpinsky
Date Created:	09/20/10
Last Edited:	09/20/10

Revision Log:
09/20/10 - Nik Karpinsky - Original creation.
*/

#ifndef _LENS_CAMERA_H_
#define _LENS_CAMERA_H_

#include <string>
#include <vector>

#include <cv.h>
#include <highgui.h>

#include <QObject>
#include <QMetaType>

namespace lens
{
  class ICamera : public QObject
  {
	Q_OBJECT

  public slots:
	virtual bool	  open(void)      = 0;
	virtual bool	  close(void)	  = 0;
	virtual int		  getWidth(void)  = 0;
	virtual int		  getHeight(void) = 0;
	virtual IplImage* getFrame(void)  = 0;
  };
}

Q_DECLARE_METATYPE(lens::ICamera*)

#endif	// _LENS_CAMERA_H_
