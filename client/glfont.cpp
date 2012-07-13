#include <qfont.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qrect.h>
#include "glfont.h"

#include "crystalclient.h"

inline int next_p2 ( int a )
{
        int rval=1;
        while(rval<a) rval<<=1;
        return rval;
}


GLFont::GLFont(QFont vfont)
	:font(vfont),metrics(vfont)
{
	for (int i=0;i<65536;i++)textures[i]=0;
	height=metrics.height();
	list_base=0;
}

GLFont::~GLFont()
{
	glDeleteLists(list_base,65536);
    for (int i=0;i<65536;i++)
		if (textures[i])glDeleteTextures(1,&textures[i]);
}

void GLFont::init(GLint mappingmode)
{
	list_base=glGenLists(65536);
	this->mappingmode=mappingmode;
	
    for (int i=0;i<65536;i++)
		if (textures[i])glDeleteTextures(1,&textures[i]);
}

void GLFont::checkText(QString text)
{
	glListBase(list_base);
	for (uint i=0;i<text.length();i++)if (!textures[text.at(i).unicode()])
	{
		QString chr=QString(text.at(i));
		ushort unicode=text.at(i).unicode();
		int th=next_p2(height);
		int w=metrics.width(chr);
		int tw=next_p2(w);
		QPixmap pix(tw,th,-1);
		QPainter painter;
		QImage img;
		QBitmap mask(tw,th);
		glGenTextures( 1,&textures[unicode]);

		glMatrixMode(GL_MODELVIEW);
		
		pix.fill(Qt::white);		// Fill pixmap with white
		mask.fill(Qt::color0);			// Fill mask bitmap with 0 (transparent)
		
		painter.begin(&mask);
		painter.setFont(font);
		painter.setPen(Qt::color1);		// And write our char as color 1 (opaq)
		painter.drawText(0,metrics.ascent(),chr,1);
		painter.end();
		
		pix.setMask(mask);				// Assign the mask to our fully white pixmap
		img=pix.convertToImage();
		
		img=CrystalFactory::convertToGLFormat(img);
		
		glBindTexture(GL_TEXTURE_2D, textures[unicode]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mappingmode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mappingmode);
		glTexImage2D( GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );
		
        glNewList(unicode,GL_COMPILE);

        glBindTexture(GL_TEXTURE_2D,textures[unicode]);
        glBegin(GL_QUADS);
        glTexCoord2d(0,1.0-(double)height/(double)th);
			glVertex2f(0,height);
        glTexCoord2d(0,1.0); 
			glVertex2f(0,0);
        glTexCoord2d((double)w/tw,1.0); 
			glVertex2f(w,0);
        glTexCoord2d((double)w/tw,1.0-(double)height/(double)th);
			glVertex2f(w,height);
        glEnd();
        glTranslatef(w,0,0);
        glEndList();
	}
}

void GLFont::renderText(double x,double y,QString text)
{
	if (text.isNull())return;
	checkText(text);
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glListBase(list_base);
	glTranslatef(x,y,0);
	for (uint i=0;i<text.length();i++)
	{
		glCallList(text.at(i).unicode());
	}
// 	glCallLists(text.length(),GL_UNSIGNED_SHORT,text.unicode());
    
	glPopMatrix();
	glPopAttrib();          
}

void GLFont::renderText(QRect r,Qt::AlignmentFlags align,QString text)
{
	if (text.isNull())return;
	double x=r.left(),y;
	y=(double)(r.top()+r.bottom()-metrics.ascent())/2.0;
	if (align==Qt::AlignLeft)x=r.left();
	if (align==Qt::AlignRight)x=r.right()-metrics.width(text);
	if (align==Qt::AlignHCenter)x=(double)(r.left()+r.right()-metrics.width(text))/2.0;	
	if (x<(double)r.left())x=(double)r.left();
	
	renderText(x,y,text);
}



