/*
 * $Id: embedtool.cpp,v 1.5 2002/12/02 12:00:18 garbanzo Exp $
 *
 * Crystal KWin embed too
 *
 * Copyright (C) 2002 Fredrik H�lund <fredrik@kde.org>
 * Copyright (C) 2008 Sascha Hlusiak <contact@saschahlusiak.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the license, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
 /* Taken from nVIDIA KWin sources and modified by Sascha Hlusiak, 2006, 2008 */

#include <qimage.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlist.h>

#include <iostream>


struct EmbedImage {
	QString string;
	int width;
	int height;
	bool alpha;
	QString name;
};

class nVIDIAEmbedder {
public:
	nVIDIAEmbedder();
	~nVIDIAEmbedder();
	
	void embed( const char * );
	void writeIndex();
	
private:
	QFile *file;
	QTextStream stream;
};

nVIDIAEmbedder::nVIDIAEmbedder()
{
	QDateTime date( QDateTime::currentDateTime() );
	QString datestring( date.toString() );
	
//	file = new QFile( "tiles.h" );
	file = new QFile();
	file->open( stdout, QIODevice::WriteOnly | QIODevice::Truncate );

	stream.setDevice( file );
}

nVIDIAEmbedder::~nVIDIAEmbedder()
{
	file->close();
	delete file;
}

void nVIDIAEmbedder::embed( const char *name )
{
	QFileInfo   fileinfo( name );
	QString     basename( fileinfo.baseName() );
	QString     codename( basename );
	QImage      image( name );
	
	codename = codename.replace( QRegExp("[^a-zA-Z0-9]"), "_" );
	
	stream << "static const QRgb " << codename << "_data[] = {" << endl << "\t";
	stream.setFieldAlignment( QTextStream::AlignRight );
	stream.setIntegerBase(16);
	stream.setPadChar( '0' );
	
	int pixels = image.width() * image.height();
	quint32 *data = reinterpret_cast<quint32*>( image.bits() );
	bool hasAlpha = false;

	
	for ( int i = 0, j = 0; i < pixels; i++ ) {
		if ( qAlpha( *data ) && qAlpha( *data ) != 0xff )
			hasAlpha = true;
		
		stream << "0x" << qSetFieldWidth(8) << *(data++) << qSetFieldWidth(0);
		
		if ( i != pixels-1 ) {
			stream << ',';
		
			if ( j++ > 4 ) {
				j = 0;
				stream << endl << "\t";
			} else
				stream << ' ';
		}
	}

	stream.reset();
	
	stream << endl << "}; // " << codename << "_data" << endl << endl;
}

int main( int argv, char **argc )
{
	if ( argv < 2 ) {
		printf("embedtool [file1] [file2] [file3] > output.h\n");
		
		return 1;
	}

	nVIDIAEmbedder embedder;

	for ( int i = 1; i < argv; i++ )
	{
// 		std::cerr << argc[i] << std::endl;
		embedder.embed( argc[i] );
	}
	
	return 0;
}

// vim: set noet ts=4 sw=4:

