/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of GCodeWorkShop.                                   *
 *                                                                         *
 *   GCodeWorkShop is free software; you can redistribute it and/or modify *
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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QFont>                // for QFont
#include <QObject>              // for Q_OBJECT
#include <QRegularExpression>   // for QRegularExpression
#include <QString>              // for QString
#include <QSyntaxHighlighter>   // for QSyntaxHighlighter
#include <QTextCharFormat>      // for QTextCharFormat
#include <QVector>              // for QVector

class QTextDocument;

#include <gcoderstyle.h>    // for HighlightColors


int autoDetectHighligthMode(const QString text);

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	Highlighter(QTextDocument* parent);

	void setHColors(const HighlightColors hColors, const QFont fnt);
	void setHighlightMode(int mode);

protected:
	void highlightBlock(const QString& text);
	void highlightBlockCommon(const QString& tx);

private:
	void highlightInside(const QString& tx, int pos, int maxlen);
	void loadRules();
	void highlightBlockSinuRule(const QString& text);
	void highlightBlockFanucRule(const QString& text);
	void highlightBlockOkumaRule(const QString& text);
	void highlightBlockSinuOldRule(const QString& text);
	void highlightBlockHeidRule(const QString& text);
	void highlightBlockHeidIsoRule(const QString& text);
	void highlightBlockToolTipsRule(const QString& text);
	void highlightBlockLinuxCNCRule(const QString& text);
	HighlightColors highlightColors;
	QFont font;
	int m_highlightMode;

	struct HighlightingRule {
		QRegularExpression pattern;
		QTextCharFormat format;
	};

	QVector<HighlightingRule> highlightRules;
	QVector<HighlightingRule> commentHighlightRules;

	struct ProgNameHighlightingRule {
		QRegularExpression pattern;
		QTextCharFormat format;
		int mode;
	};

	QVector<ProgNameHighlightingRule> progNameHighlightRules;

	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;

	QTextCharFormat keywordFormat;

	QTextCharFormat commentFormat;

	QTextCharFormat operatorFormat;
	QTextCharFormat progNameFormat;
	//QTextCharFormat adressFormat;
};

#endif // HIGHLIGHTER_H
