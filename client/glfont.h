#ifndef _GL_FONT_
#define _GL_FONT_

#include <GL/gl.h>

class QFont;
class QFontMetrics;
class QRect;



class GLFont
{
public:
	GLFont(QFont vfont);
	~GLFont();

	void init(bool antialiase);
//	void renderChar(double x,double y,int chr);
	void renderText(double x,double y,QString text,bool fade,double maxw);
	void renderText(QRect r,Qt::AlignmentFlags align,QString text,bool fade);
	
	int height;
private:
	QFont font;
	QFontMetrics metrics;
	GLuint textures[65536];		// Damn unicode
	GLuint list_base;
	bool aa;
		
	void checkText(QString text);
	QImage adjustAlphaBuffer(QImage img);
};




#endif
