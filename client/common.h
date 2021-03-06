/***************************************************************************
 *   common.h                                                              *
 *   -----------------------                                               *
 *   Copyright (C) 2006-2011 by Sascha Hlusiak                             *
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


#ifndef COMMON_H_INCLUDED_
#define COMMON_H_INCLUDED_


struct WND_CONFIG
{
	int outlineMode,inlineMode;
	QColor frameColor,inlineColor;
	QPixmap overlay;
	int transparency;
	bool stretch_overlay;
}; 


enum ButtonImageTypes {
	ButtonImageMenu=0,
	ButtonImageHelp,
	ButtonImageMax,
	ButtonImageRestore,
	ButtonImageMin,
	ButtonImageClose, 
	ButtonImageSticky,
	ButtonImageUnSticky,
	ButtonImageShade,
	ButtonImageUnShade,
	ButtonImageBelow,
	ButtonImageUnBelow,
	ButtonImageAbove,
	ButtonImageUnAbove,
	ButtonImageCount
};


#endif