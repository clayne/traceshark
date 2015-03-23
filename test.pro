#
#
#  Traceshark - a visualizer for visualizing ftrace traces
#  Copyright (C) 2014-2015  Viktor Rosendahl
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

HEADERS       = argnode.h
HEADERS      += cpunode.h
HEADERS      += eventnode.h
HEADERS      += ftraceparser.h
HEADERS      += grammarnode.h
HEADERS      += grammarroot.h
HEADERS      += mainwindow.h
HEADERS      += namepidnode.h
HEADERS      += mempool.h
HEADERS      += timenode.h
HEADERS      += traceevent.h
HEADERS      += tracefile.h
HEADERS      += traceline.h
HEADERS      += traceshark.h
HEADERS      += tstring.h

SOURCES       = argnode.cpp
SOURCES      += cpunode.cpp
SOURCES      += eventnode.cpp
SOURCES      += ftraceparser.cpp
SOURCES      += grammarnode.cpp
SOURCES      += grammarroot.cpp
SOURCES      += main.cpp
SOURCES      += mainwindow.cpp
SOURCES      += mempool.cpp
SOURCES      += namepidnode.cpp
SOURCES      += timenode.cpp
SOURCES      += tracefile.cpp

QT           += core
QT           += widgets

QMAKE_CXXFLAGS_RELEASE += -Wall -g
QMAKE_CFLAGS_RELEASE += -pedantic -Wall -std=c99
QMAKE_LFLAGS_RELEASE =
CONFIG += DEBUG
