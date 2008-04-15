/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: kdict_classes.cpp

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "kdict_classes.h"
#include "string_utils.h"
#include <boost/format.hpp>

SkipCode::SkipCode() {
	i1 = i2 = i3 = 0;
}

SkipCode::SkipCode(const string& skipStr) {
	list<string> ls = StrTokenize<char>(skipStr, "-");
	i1 = atoi(ls.front().c_str()); ls.pop_front();
	i2 = atoi(ls.front().c_str()); ls.pop_front();
	i3 = atoi(ls.front().c_str());
}

string SkipCode::str() const {
	return (boost::format("%d-%d-%d") % i1 % i2 % i3).str();
}

bool SkipCode::is_set() const {
	return (i1 && i2 && i3);
}

KInfo::KInfo() {
	radical = radicalNelson = (unsigned char) 0;
	grade = strokeCount = freq = 0;
}
