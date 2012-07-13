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

	void init(GLint mappingmode);
//	void renderChar(double x,double y,int chr);
	void renderText(double x,double y,QString text);
	void renderText(QRect r,Qt::AlignmentFlags align,QString text);
	
	int height;
private:
	QFont font;
	QFontMetrics metrics;
	GLuint textures[65535];		// Damn unicode
	GLuint list_base;
	GLint mappingmode;
		
	void checkText(QString text);
};




#endif
