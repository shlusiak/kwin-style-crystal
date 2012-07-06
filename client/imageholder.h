#ifndef _IMAGEHOLDER_INCLUDED_
#define _IMAGEHOLDER_INCLUDED_



#include <kwinmodule.h>
#include "myrootpixmap.h"

class QImageHolder:public QObject
{
	Q_OBJECT
public:
	QImageHolder();
	virtual ~QImageHolder();
	
	void Init();
	QPixmap *image(bool active) { Init(); return active?img_active:img_inactive; }
	void repaint(bool force);

private:
	bool initialized;
	KMyRootPixmap *rootpixmap;
	QPixmap *img_active,*img_inactive;
	
public slots:
	void BackgroundUpdated(const QImage *);
	void handleDesktopChanged(int desk);
	void CheckSanity();
	
signals:
	void repaintNeeded();
};


#endif
