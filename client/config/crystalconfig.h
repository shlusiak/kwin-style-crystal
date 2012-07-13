//////////////////////////////////////////////////////////////////////////////
// exampleconfig.h
// -------------------
// Config module for Example window decoration
// -------------------
// Copyright (c) 2003 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef EXAMPLECONFIG_H
#define EXAMPLECONFIG_H

#include <qobject.h>

#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8

class KConfig;
class ConfigDialog;

class ExampleConfig : public QObject
{
    Q_OBJECT
public:
    ExampleConfig(KConfig* config, QWidget* parent);
    ~ExampleConfig();
    
signals:
    void changed();

public slots:
    void load(KConfig*);
    void save(KConfig*);
    void defaults();
	void infoDialog();

protected slots:
    void selectionChanged(int);
	void selectionChanged(double);
    void colorChanged(const QColor&) { selectionChanged(0); }
	
	void hoverChanged(int);
	void refractionChanged(int);
	void transparencyChanged(int);

private:
    KConfig *config_;
    ConfigDialog *dialog_;
};

#endif // EXAMPLECONFIG_H
