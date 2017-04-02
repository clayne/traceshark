/*
 * Traceshark - a visualizer for visualizing ftrace and perf traces
 * Copyright (C) 2015, 2016  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CURSOR_H
#define CURSOR_H

#include "qcp/qcp.h"

class Cursor : public QCPItemLine
{
	Q_OBJECT
public:
	Cursor(QCustomPlot *parent, const QColor &color);
	double getPosition();
	void setPosition(double pos);
	void setColor(const QColor &color);
private:
	double position;
};

#endif /* CURSOR_H */
