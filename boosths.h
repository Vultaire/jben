/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: boosths.h

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef boosths_h
#define boosths_h

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

using namespace std;
using namespace boost;
using namespace boost::multi_index;

template<typename value>
class BoostHS : public
	multi_index_container<
		value,
		indexed_by<
			hashed_unique<
				identity<value>
			>
		>
	> {
public:
	bool assign(const value& v);
private:
};

template<typename value>
bool BoostHS<value>::assign(const value& v) {
	pair<typename BoostHS<value>::iterator, bool> p = this->insert(v);
	if(!p.second) {
		if(!this->replace(p.first, v)) return false;
	}
	return true;
}

#endif
