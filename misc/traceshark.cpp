// SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause)
/*
 * Traceshark - a visualizer for visualizing ftrace and perf traces
 * Copyright (C) 2021  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 *
 *  a) This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of the
 *     License, or (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with this library; if not, write to the Free
 *     Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *     MA 02110-1301 USA
 *
 * Alternatively,
 *
 *  b) Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *     1. Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *     2. Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *     NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <regex.h>

#include <QObject>

#include "misc/traceshark.h"
#include "vtl/compiler.h"

namespace TShark {

#undef TSHARK_LOGIC_ITEM_
#define TSHARK_LOGIC_ITEM_(a) vtl_str(a)
	const char * const logic_names[] = {
		TSHARK_LOGIC_DEFS_,
		nullptr
	};
#undef TSHARK_LOGIC_ITEM_

	QString translateRegexError(int ecode)
	{
		QObject q;
		QString emsg;
		switch (ecode) {
		case REG_BADBR:
			emsg = q.tr("Invalid use of back reference operator.");
			break;
		case REG_BADPAT:
			emsg = q.tr("Invalid use of pattern operators.");
			break;
		case REG_BADRPT:
			emsg = q.tr("Invalid use of repetition operators.");
			break;
		case REG_EBRACE:
			emsg = q.tr("Un-matched brace interval operators.");
			break;
		case REG_EBRACK:
			emsg = q.tr("Un-matched bracket list operators.");
			break;
		case REG_ECOLLATE:
			emsg = q.tr("Invalid collating element.");
			break;
		case REG_ECTYPE:
			emsg = q.tr("Unknown character class name.");
			break;
		case REG_EEND:
			emsg = q.tr("Nonspecific error.");
			break;
		case REG_EESCAPE:
			emsg = q.tr("Trailing backslash.");
			break;
		case REG_EPAREN:
			emsg = q.tr("Un-matched parenthesis group operators.");
			break;
		case REG_ERANGE:
			emsg = q.tr("Invalid use of the range operator.");
			break;
		case REG_ESIZE:
			emsg = q.tr("Pattern buffer too large.");
			break;
		case REG_ESPACE:
			emsg = q.tr("The regex routines ran out of memory.");
			break;
		case REG_ESUBREG:
			emsg = q.tr("Invalid back reference to a subexpression.");
			break;
		default:
			emsg = q.tr("Unknown error.");
			break;
		}
		return emsg;
	}
}
