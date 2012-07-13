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
	height=metrics.height();
	list_base=0;
}

GLFont::~GLFont()
{
	glDeleteLists(list_base,255);
    glDeleteTextures(255,&textures[0]);
}

void GLFont::init(GLint mappingmode)
{
	QPixmap pix(1,next_p2(height),-1);
	QPainter painter;
	QImage img;
	QBitmap mask(1,next_p2(height));
	char text[2]={'-','\0'};

	list_base=glGenLists(255);
	glGenTextures( 255, &textures[0] );
	glMatrixMode(GL_MODELVIEW);
	int th=next_p2(height);
	for (int i=0;i<255;i++)
	{
		text[0]=i;
		int w=metrics.width(text);
		int tw=next_p2(w);
		if (tw!=pix.width()) 
		{
			pix.resize(tw,th);
			pix.fill(Qt::white);		// Fill pixmap with white
			mask.resize(tw,th);
		}
		mask.fill(Qt::color0);			// Fill mask bitmap with 0 (transparent)
		
		painter.begin(&mask);
		painter.setFont(font);
		painter.setPen(Qt::color1);		// And write our char as color 1 (opaq)
		painter.drawText(0,metrics.ascent(),text,1);
		painter.end();
		
		pix.setMask(mask);				// Assign the mask to our fully white pixmap
		img=pix.convertToImage();
		
		img=CrystalFactory::convertToGLFormat(img);
		
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mappingmode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mappingmode);
		glTexImage2D( GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );
		
        glNewList(list_base+i,GL_COMPILE);

        glBindTexture(GL_TEXTURE_2D,textures[i]);
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

void GLFont::renderChar(double x,double y,int chr)
{
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
 	glEnable(GL_BLEND);
 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      

	glListBase(list_base);

	glTranslatef(x,y,0);

	glCallList(chr);
    
	glPopMatrix();
	glPopAttrib();          
}


void GLFont::renderText(double x,double y,const char* str)
{
	if (str==NULL)return;
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glListBase(list_base);
	glTranslatef(x,y,0);
	glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);
    
	glPopMatrix();
	glPopAttrib();          
}

void GLFont::renderText(QRect r,Qt::AlignmentFlags align,const char *text)
{
	if (text==NULL)return;
	double x=r.left(),y;
	y=(double)(r.top()+r.bottom()-metrics.ascent())/2.0;
	if (align==Qt::AlignLeft)x=r.left();
	if (align==Qt::AlignRight)x=r.right()-metrics.width(text);
	if (align==Qt::AlignHCenter)x=(double)(r.left()+r.right()-metrics.width(text))/2.0;	
	if (x<(double)r.left())x=(double)r.left();
	
	renderText(x,y,text);
}



