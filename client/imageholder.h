#ifndef _IMAGEHOLDER_INCLUDED_
#define _IMAGEHOLDER_INCLUDED_



// #include <kwinmodule.h>
#include "myrootpixmap.h"
#include <GL/glx.h>
#include <qimage.h>


class QImageHolder:public QObject
{
	Q_OBJECT
public:
	QImageHolder();
	virtual ~QImageHolder();
	
	void Init();
	void repaint(bool force);
	GLuint texture() { return mytexture; }
	void activateTexture();
	
	double screenwidth() { return vscreenwidth; }
	double screenheight() { return vscreenheight; }
private:
	bool initialized;
	KMyRootPixmap *rootpixmap;
	double vscreenwidth,vscreenheight;
	
	GLuint mytexture;	
	
public slots:
	void BackgroundUpdated(const QImage *);
	void handleDesktopChanged(int desk);
	void CheckSanity();
signals:
	void repaintNeeded();
};


#endif
