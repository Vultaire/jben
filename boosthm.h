/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: boosthm.h

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

#ifndef boosthm_h
#define boosthm_h

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

using namespace std;
using namespace boost;
using namespace boost::multi_index;

template<typename key, typename value>
class BoostHM : public
	multi_index_container<
		pair<key, value>,
		indexed_by<
			hashed_unique<
				member<
					pair<key, value>,
					key,
					&pair<key, value>::first
				>
			>
		>
	> {
public:
	const value& operator[](const key& k);
	const value& operator[](const key& k) const;
	bool assign(const key& k, const value& v);
private:
};

/* NOTE: These two operator[] overloads do not handle non-existant keys!
   Use with caution! */
template<typename key, typename value>
const value& BoostHM<key,value>::operator[](const key& k) {
	return this->find(k)->second;
}

template<typename key, typename value>
const value& BoostHM<key,value>::operator[](const key& k) const {
	return this->find(k)->second;
}

template<typename key, typename value>
bool BoostHM<key,value>::assign(const key& k, const value& v) {
	pair<key,value> newEntry(k,v);

	pair<typename BoostHM<key,value>::iterator, bool> p = this->insert(newEntry);
	if(!p.second) {
		if(!this->replace(p.first, newEntry)) return false;
	}
	return true;
}

#endif
