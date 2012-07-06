/***************************************************************************
 *   Copyright (C) 2006 by Sascha Hlusiak                                  *
 *   Spam84@gmx.de                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/



#ifndef _CRYSTALBUTTON_INCLUDED_
#define _CRYSTALBUTTON_INCLUDED_

#include <qbutton.h>

class CrystalClient;
class ButtonImage;

class CrystalButton : public QButton
{
	Q_OBJECT

public:
	CrystalButton(CrystalClient *parent=0, const char *name=0,
			const QString &tip=NULL,
			ButtonType type=ButtonHelp,
			ButtonImage *vimage=NULL);
	virtual ~CrystalButton();

	void setBitmap(ButtonImage *newimage);
	QSize sizeHint() const;
	int lastMousePress() const { return lastmouse_; }
	void reset() { repaint(false); }
	void setFirstLast(bool vfirst,bool vlast) { first|=vfirst; last|=vlast; }
	void resetSize(bool FullSize);
private:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void drawButton(QPainter *painter);

	int buttonSizeH() const;
	int buttonSizeV() const;

private slots:
	void animate();

private:
	QTimer animation_timer;
	bool first,last;
	bool hover;
	float animation;
	CrystalClient *client_;
	ButtonType type_;
	ButtonImage *image;
	int lastmouse_;
};

#endif
