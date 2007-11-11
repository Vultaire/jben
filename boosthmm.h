/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: boosthmm.h

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

#ifndef boosthmm_h
#define boosthmm_h

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;
using namespace boost;
using namespace boost::multi_index;

template<typename key, typename value>
class BoostHMM : public
	multi_index_container<
		pair<key, value>,
		indexed_by<
			hashed_non_unique<
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

template<typename key, typename value>
const value& BoostHMM<key,value>::operator[](const key& k) {
	return this->find(k)->second;
}

template<typename key, typename value>
const value& BoostHMM<key,value>::operator[](const key& k) const {
	return this->find(k)->second;
}

/* Assign will only assign if the pair does not match an existing one. */
template<typename key, typename value>
bool BoostHMM<key,value>::assign(const key& k, const value& v) {
	pair<key,value> newEntry(k,v);
	bool bOk=false;

	typename BoostHMM<key,value>::iterator i = this->find(k);
	if(i==this->end()) bOk=true;  /* key does not exist, so we know we should
	                                be able to insert this value. */

	/* If a key existed, we need to check if there's an exact match.
	   Only proceed if no exact match is found. */
	if(!bOk) {
		for(; i!=this->end(); i++) {
			/* I'm assuming the container holds values
			   with the same key in a hashed index TOGETHER.  Hopefully this
			   is a valid assumption. */
			if(i->first!=k) {
				i = this->end(); /* Makes sure we exit on a GOOD note. */
				break;
			}
			/* If we get here, then the key and value
			   both match.  BREAK. */
			if(i->second==v) {
			    break;
			}
		}
		if(i==this->end()) bOk = true;
	}

	/* If no match exists, proceed with the insertion. */
	if(bOk) {
		pair<typename BoostHMM<key,value>::iterator, bool> p = this->insert(newEntry);
		if(!p.second) {
#ifdef DEBUG
			cerr << "Error in hash multimap!  This should never happen!" << endl;
#endif
			return false;
		}
	}

	return bOk;
}

#endif
