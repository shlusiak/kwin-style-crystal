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

	void init();
	void renderChar(double x,double y,int chr);
	void renderText(double x,double y,const char *text);
	void renderText(QRect r,Qt::AlignmentFlags align,const char *text);
	
	int height;
private:
	QFont font;
	QFontMetrics metrics;
	GLuint textures[255];
	GLuint list_base;

};




#endif
