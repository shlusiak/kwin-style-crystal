#include <math.h>
#include "crystalclient.h"
#include "crystalbutton.h"
#include "glfont.h"
#include "imageholder.h"


namespace Render
{

inline const double max(const double a,const double b)
{ return (a<b)?b:a; }

inline const double max(const double a,const double b,const double c)
{ return max(a,max(b,c)); }

inline const double abs(const double x)
{ return (x<0)?(-x):(x); }

inline const double sqr(const double x)
{ return x*x; }



void drawBar(double x,double y,double w,double h)
{
	glBegin(GL_QUADS);
	glNormal3f( 0,0,-1);
    glTexCoord2f(x,y+h);		glVertex3f(x,y+h,   0);		// Bottom left
    glTexCoord2f(x+w,y+h);	glVertex3f(x+w,y+h, 0);		// Bottom right
    glTexCoord2f(x+w,y);		glVertex3f(x+w,y,   0);		// Top right
    glTexCoord2f(x,y);		glVertex3f(x,y,     0);		// Top left
	glEnd();
}

inline QColor blendColor(QColor color1,QColor color2,double balance)
{
	return QColor(color1.red()+(int)(balance*(color2.red()-color1.red())),
		color1.green()+(int)(balance*(color2.green()-color1.green())),
		color1.blue()+(int)(balance*(color2.blue()-color1.blue())));
}

void renderGlassVertex(double tx,double ty,const double x,const double y,const double z,const double angx,const double angy,const double ior)
//	ang: 0 is front side
{
	if (angx!=0.0)tx-=tan(angx-angx/ior)*(z);
	if (angy!=0.0)ty-=tan(angy-angy/ior)*(z);
		
	glTexCoord2f(tx,ty);		glVertex3f(x,y,0.0);
}

void renderGlassRect(const double x,const double y,const double w,const double h,const double ior,const double tesselation,const bool horizontal)
{
	const double width=horizontal?h:w;
	double x1,z1,ang;
	glBegin(GL_QUAD_STRIP);
	for (int i=0;i<=tesselation;i++)
	{
		ang=(double)i*M_PI/(double)tesselation-M_PI/2.0;
		x1=sin(ang)/2.0*width+width/2.0;
		z1=cos(ang)*width/2.0;
		
		if (horizontal)
		{
			renderGlassVertex(x,y+x1, 		x,y+x1,z1, 		0,ang,  ior);
			renderGlassVertex(x+w,y+x1,		x+w,y+x1,z1,	0,ang,  ior);
		}else{
			renderGlassVertex(x+x1,y, 		x+x1,y,z1, 		ang,0,  ior);
			renderGlassVertex(x+x1,y+h,		x+x1,y+h,z1,	ang,0,  ior);
		}
	}
	glEnd();
}

void renderLightedQuad(const double x1,const double y1,const double x2,const double y2,const double bright,const double alpha)
{
	double cc[4];
	glGetDoublev(GL_CURRENT_COLOR,cc);
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glBegin(GL_QUADS);
	glColor4f(bright,bright,bright,alpha);
	glVertex3f(x1,y1,0);
	glVertex3f(x2,y1,0);
	glVertex3f(x2,y2,0);
	glVertex3f(x1,y2,0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor4f(cc[0],cc[1],cc[2],cc[3]);
}

void renderFacettedGlassRect(const double x,const double y,const double w,const double h,const double ior,const double tesselation,const bool horizontal,const bool lighting)
{
	const double width=horizontal?h:w;
	double x1,z1,ang=0.0,ang1,ang2,x2,z2;
	double bright,alpha;

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	for (int i=0;i<tesselation;i++)
	{
		ang1=(double)i*M_PI/(double)tesselation-M_PI_2;
		x1=sin(ang1)/2.0*width+width/2.0;
		z1=cos(ang1)*width/2.0;
		ang2=(double)(i+1)*M_PI/(double)tesselation-M_PI_2;
		x2=sin(ang2)/2.0*width+width/2.0;
		z2=cos(ang2)*width/2.0;
		ang=(ang1+ang2)*0.5;
		bright=(ang1<0)?1.0:0.0;
		alpha=sqr(abs(ang1/M_PI_2)*(2.0-bright)*0.5);
		
		if (horizontal)
		{
			renderGlassVertex(x,y+x1, 		x,y+x1,z1, 		0,ang,  ior);
			renderGlassVertex(x+w,y+x1,		x+w,y+x1,z1,	0,ang,  ior);
		
			renderGlassVertex(x+w,y+x2,		x+w,y+x2,z2,	0,ang,  ior);
			renderGlassVertex(x,y+x2, 		x,y+x2,z2, 		0,ang,  ior);
			if (lighting)
			{
				glEnd();
				renderLightedQuad(x,y+x1,x+w,y+x2,bright,alpha);
				glBegin(GL_QUADS);
			}
		}else{
			renderGlassVertex(x+x1,y, 		x+x1,y,z1, 		ang,0,  ior);
			renderGlassVertex(x+x1,y+h,		x+x1,y+h,z1,	ang,0,  ior);
			
			renderGlassVertex(x+x2,y+h,		x+x2,y+h,z2,	ang,0,  ior);
			renderGlassVertex(x+x2,y, 		x+x2,y,z2, 		ang,0,  ior);
			
			if (lighting)
			{
				glEnd();
				renderLightedQuad(x+x1,y,x+x2,y+h,bright,alpha);
				glBegin(GL_QUADS);
			}
		}
	}
	glEnd();
}

}


void CrystalClient::renderLighting()
{
	const double light=1.0;
	const double lightalpha=0.18;
	const double dark=0.0;
	const double darkalpha=0.42;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glBegin(GL_QUADS);
	// Lighten top 
	glColor4f(light,light,light,lightalpha);	
	glVertex3f(width(),0,   0);		// Top right
	glVertex3f(0,0,     0);			// Top left

	glColor4f(light,light,light,0.00);	
	glVertex3f(isShade()?0:bl/2.0,bt/2.0,  0);		// Bottom left
	glVertex3f(width()-(isShade()?0:br/2.0),bt/2.0, 0);		// Bottom right

	glColor4f(dark,dark,dark,0.00);	
	glVertex3f(width()-(isShade()?0:br/2.0),bt/2.0, 0);		// Bottom right
	glVertex3f(isShade()?0:bl/2.0,bt/2.0,  0);		// Bottom left
	
	glColor4f(dark,dark,dark,darkalpha);	
	glVertex3f(isShade()?0:bl,bt,  0);		// Bottom left
	glVertex3f(width()-(isShade()?0:br),bt, 0);		// Bottom right
	glEnd();
    

	if (!isShade())
	{
		// Lighten bottom
		glBegin(GL_QUADS);
		glColor4f(light,light,light,lightalpha);
		glVertex3f(width()-br,height()-bb,   0);
		glVertex3f(bl,height()-bb,     0);

		glColor4f(light,light,light,0.00);
		glVertex3f(bl/2.0,height()-bb/2.0,  0);
		glVertex3f(width()-br/2.0,height()-bb/2.0, 0);

		glColor4f(dark,dark,dark,0.00);
		glVertex3f(width()-br/2.0,height()-bb/2.0, 0);
		glVertex3f(bl/2.0,height()-bb/2.0,  0);

		glColor4f(dark,dark,dark,darkalpha);
		glVertex3f(0,height(),  0);
		glVertex3f(width(),height(), 0);
		
		// Enlighten left side
		glColor4f(light,light,light,lightalpha);	
		glVertex3f(0,0,   0);
		glVertex3f(0,height(),     0);

		glColor4f(light,light,light,0.00);	
		glVertex3f(bl/2.0,height()-bb/2.0,  0);
		glVertex3f(bl/2.0,bt/2.0, 0);
			
		glColor4f(dark,dark,dark,0.00);	
    	glVertex3f(bl/2.0,bt/2.0, 0);
		glVertex3f(bl/2.0,height()-bb/2.0,  0);
	
		glColor4f(dark,dark,dark,darkalpha);	
    	glVertex3f(bl,height()-bb,  0);
		glVertex3f(bl,bt, 0);
			
		// Enlighten right side
		glColor4f(light,light,light,lightalpha);
		glVertex3f(width()-br,bt,   0);
		glVertex3f(width()-br,height()-bb,     0);

		glColor4f(light,light,light,0.0);	
		glVertex3f(width()-br/2.0,height()-bb/2.0,  0);
		glVertex3f(width()-br/2.0,bt/2.0, 0);

		glColor4f(dark,dark,dark,0.00);	
		glVertex3f(width()-br/2.0,bt/2.0, 0);
		glVertex3f(width()-br/2.0,height()-bb/2.0,  0);
	
		glColor4f(dark,dark,dark,darkalpha);	
		glVertex3f(width(),height(),  0);
		glVertex3f(width(),0, 0);
		glEnd();
	}
}


void CrystalClient::paintEvent(QPaintEvent*)
{
	using namespace Render;
	if (!CrystalFactory::initialized()) return;

	// This sets up the rendering state, if not already done, and attaches the glxcontext to the winId
	if (!::factory->makeCurrent(widget()->winId()))return;

	if (::factory->trackdesktop)
	if (::factory->useTransparency)
		::factory->image_holder->repaint(false); // If other desktop than the last, regrab the root image

	glViewport(0,0,(GLint)width(),(GLint)height());

	QPoint tl=widget()->mapToGlobal(QPoint(0,0));
	
	// Translate model matrix so that it fits to screen coordinates
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0,width(),height(),0,-1,1);

	// Translates texture coordinates to fit the screen coordinates
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	if (::factory->useTransparency)
	{
		glScaled(1.0/::factory->image_holder->screenwidth(),-1.0/::factory->image_holder->screenheight(),1);
		glTranslated(tl.x(),tl.y(),0);

		glEnable(GL_TEXTURE_2D);
		::factory->image_holder->activateTexture();
	}
	glMatrixMode(GL_MODELVIEW);
// 	glClear(GL_COLOR_BUFFER_BIT);	// For debugging

	QRect r=titlebar_->geometry();

//	double myanimation=sin(animation*M_PI/2.0);
	double myanimation=animation;
	
	QColor color=blendColor(::factory->inactiveColor,::factory->activeColor,myanimation);
	
	borders(bl,br,bt,bb);

	
	// tint the title bar
	glColorQ(color);
	
	if (::factory->useRefraction)
	{
		const double ior=(::factory->iorActive-::factory->iorInactive)*myanimation+(::factory->iorInactive);
		if (1)
		{
			renderGlassRect(0,0,width(),bt,ior,7,true);
			renderGlassRect(0,bt,bl,height()-bb-bt,ior/1.2,5,false);
			renderGlassRect(width()-br,bt,br,height()-bb-bt,ior/1.2,5,false);
			renderGlassRect(0,height()-bb,width(),bb,ior/1.2,5,true);
			
			glDisable(GL_TEXTURE_2D);
			if (::factory->useLighting)renderLighting();
		}else{
			const bool lighting=::factory->useLighting;
			renderFacettedGlassRect(0,0,width(),bt,ior,5,true,lighting);
			renderFacettedGlassRect(0,bt,bl,height()-bb-bt,ior/1.2,4,false,lighting);
			renderFacettedGlassRect(width()-br,bt,br,height()-bb-bt,ior/1.2,4,false,lighting);
			renderFacettedGlassRect(0,height()-bb,width(),bb,ior/1.2,4,true,lighting);
			
 			glDisable(GL_TEXTURE_2D);
		}
	}else{
		drawBar(0,0,width(),bt);
		// Left
		drawBar(0,bt,bl,height()-bb-bt);
		// Right
		drawBar(width()-br,bt,br,height()-bb-bt);
		// Bottom
		drawBar(0,height()-bb,width(),bb);
	
		glDisable(GL_TEXTURE_2D);
		if (::factory->useLighting)renderLighting();
	}
	
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	
	
	if (!caption().isNull() && ::factory->gl_font)
	{   // Render caption 
		QRect r=titlebar_->geometry();

		glEnable(GL_SCISSOR_TEST);
		glScissor(r.left(),height()-bt,r.width(),bt);

		r.moveBy(-1,-1);
		QColor color1=options()->color(KDecoration::ColorFont, false);
		QColor color2=options()->color(KDecoration::ColorFont, true);
		QColor color=blendColor(color1,color2,myanimation);

		if (::factory->textshadow)
		{	// First draw shadow
			if (max(color.red(),color.green(),color.blue())>75)
			{	// Only if color is bright enough draw a dark shadow to improve readability
				glColor4f(0.0,0.0,0.0,0.5);
				r.moveBy(1,1);
				::factory->gl_font->renderText(r,CrystalFactory::titleAlign(),caption());
				r.moveBy(-2,0);
				::factory->gl_font->renderText(r,CrystalFactory::titleAlign(),caption());
				r.moveBy(1,-1);
			}
		}

		glColorQ(color,0.7+myanimation*0.3);
		::factory->gl_font->renderText(r,CrystalFactory::titleAlign(),caption());
		glDisable(GL_SCISSOR_TEST);
	}

	double buttonFade=(::factory->fadeInactiveButtons?0.4+0.6*myanimation:1.0);
	for (int i=0;i<ButtonTypeCount;i++)if (button[i])
		button[i]->drawButton(buttonFade);
	
// 	glXWaitGL();
	// Swap buffers and be happy
	glXSwapBuffers( qt_xdisplay(),widget()->winId() );
	::factory->makeCurrent(0);
}

