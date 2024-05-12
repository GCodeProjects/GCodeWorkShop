/*
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QChar>    // for operator==, QChar, operator!=, operator<=, operator>=
#include <QString>  // for QString

#include "utils-spaces.h"


int Utils::removeSpaces(QString &tx, std::function<bool(int)> interrupt)
{
	enum {
		NORMAL_FLOW,
		AFTER_APOSTROPHE,
		AFTER_PARETHESIS,
		AFTER_SEMICOLON
	} state = NORMAL_FLOW;

	QString updatedText;
	int pos = 0;
	bool wasLetter = false;
	bool skipChar;
	bool openAddress = false;
	int replaced = 0;
	QChar currentChar = QChar();

	while (pos < tx.length()) {
		if (interrupt(pos)) {
			return 0;
		}

		currentChar = tx.at(pos++);
		skipChar = false;

		switch (state) {
		case AFTER_APOSTROPHE:
			if (currentChar == '\'' || currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		case AFTER_PARETHESIS:
			if (currentChar == ')' || currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		case AFTER_SEMICOLON:
			if (currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		default:
			if (currentChar == '\'') {
				state = AFTER_APOSTROPHE;
			} else if (currentChar == '(') {
				state = AFTER_PARETHESIS;
			} else if (currentChar == ';') {
				state = AFTER_SEMICOLON;
			} else {
				// Some spaces are not removed to avoid merging words.
				// If there are letters before and after spaces, then one space is left.
				if (currentChar == ' ' || currentChar == '\t') {
					skipChar = true;
					replaced ++;

					if (wasLetter) {
						openAddress = true;
					}
				} else if ((currentChar >= 'A' && currentChar <= 'Z') ||
				           (currentChar >= 'a' && currentChar <= 'z') ||
				           currentChar == '#') {
					if (openAddress) {
						updatedText.append(' ');
					}

					wasLetter = true;
					openAddress = false;
				} else {
					wasLetter = false;
					openAddress = false;
				}
			}
		}

		if (!skipChar) {
			updatedText.append(currentChar);
		}
	}

	if (replaced) {
		tx = updatedText;
	}

	return replaced;
}

int Utils::insertSpaces(QString &tx, std::function<bool(int)> interrupt)
{
	enum {
		NORMAL_FLOW,
		AFTER_APOSTROPHE,
		AFTER_PARETHESIS,
		AFTER_SEMICOLON
	} state = NORMAL_FLOW;

	int pos = 0;
	QChar currentChar = QChar();
	QChar previosChar = QChar();
	int replaced = 0;

	QString updatedText;

	while (pos < tx.length()) {
		if (interrupt(pos)) {
			return 0;
		}

		previosChar = currentChar;
		currentChar = tx.at(pos++);
		bool insert = false;

		switch (state) {
		case AFTER_APOSTROPHE:
			if (currentChar == '\'' || currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		case AFTER_PARETHESIS:
			if (currentChar == ')' || currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		case AFTER_SEMICOLON:
			if (currentChar == '\n') {
				state = NORMAL_FLOW;
			}

			break;

		default:
			if (currentChar == '\'') {
				state = AFTER_APOSTROPHE;

				if (previosChar != ' ' && previosChar != '\n') {
					insert = true;
				}
			} else if (currentChar == '(') {
				state = AFTER_PARETHESIS;

				if (previosChar != ' ' && previosChar != '\n') {
					insert = true;
				}
			} else if (currentChar == ';') {
				state = AFTER_SEMICOLON;

				if (previosChar != ' ' && previosChar != '\n') {
					insert = true;
				}
			} else if (previosChar == ' ' || currentChar == ' ') {
				// do nothing
			} else if (previosChar == '\n') {
				// do nothing
			} else if (currentChar == '#' && previosChar != '#' && previosChar != '['
			           && previosChar != '-' && previosChar != '+' && previosChar != '*'
			           && previosChar != '/' && previosChar != '=') {
				insert = true;
			} else if ((previosChar >= 'A' && previosChar <= 'Z') ||
			           (previosChar >= 'a' && previosChar <= 'z') ||
			           previosChar == ',') {
				// do nothing
			} else if ((currentChar >= 'A' && currentChar <= 'Z') ||
			           (currentChar >= 'A' && currentChar <= 'Z') ||
			           currentChar == ',') {
				insert = true;
			}
		}

		if (insert) {
			replaced++;
			updatedText.append(' ');
		}

		updatedText.append(currentChar);
	}

	if (replaced) {
		tx = updatedText;
	}

	return replaced;
}
