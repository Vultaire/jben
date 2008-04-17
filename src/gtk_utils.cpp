/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: gtk_utils.cpp

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

#include "gtk_utils.h"
#include "string_utils.h"
#include <stack>
#include <list>
using namespace std;

void SetTextBuf(Glib::RefPtr<Gtk::TextBuffer> rpBuf, std::string& text) {
	size_t si, ei=0;
	string s;
	stack<string> fontStack;
	list< Glib::RefPtr<Gtk::TextTag> > tagList;
	string fontName = "en"; /* Current font tag to apply */
	int bold = 0;  /* Count of bold tags, nestable */
	bool handled; /* Tracks whether <> args were eaten by the parser or not */

	/* Iterate through all tags
	 * Supported tags: b, font, image
	 * font affects ranges and can be nested, like in html.
	 * image is an immediate, unnested tag.
	 * b is an HTML-style bold tag.
	 */

	rpBuf->set_text("");
	Gtk::TextBuffer::iterator it = rpBuf->get_iter_at_offset(0);
	Glib::RefPtr<Gtk::TextBuffer::TagTable> rpTable = rpBuf->get_tag_table();
	/* Insert any text before the first tag */
	si = text.find('<',0);
	if(si!=string::npos && si!=0)
		it = rpBuf->insert_with_tag(it, text.substr(0, si), fontName);
	while(si!=string::npos) {
		/* Get tag and process */
		ei = text.find('>', si+1);
		if(ei == string::npos) break;
		list<string> tag = StrTokenize<char>
			(text.substr(si+1, ei-(si+1)), " ");
		s = tag.front();
		handled = true;
		if(s=="font") {
			tag.pop_front();
			fontName = tag.front();
			fontStack.push(fontName);
		} else if(s=="/font") {
			if(!fontStack.empty()) fontStack.pop();
			if(!fontStack.empty())
				fontName=fontStack.top();
			else
				fontName="en";
		} else if(s=="b") {
			bold++;
		} else if(s=="/b") {
			bold--;
		} else if(s=="img") {
			tag.pop_front();
			it = rpBuf->insert_pixbuf(it,
				Gdk::Pixbuf::create_from_file(tag.front()));
		} else handled = false;
		/* Update our tag list */
		tagList.clear();
		tagList.push_back(rpTable->lookup(fontName));
		if(bold>0) tagList.push_back(rpTable->lookup("bold"));

		if(!handled) {
			/* Include the text for any tags which could not be properly
			   handled. */
			it = rpBuf->insert_with_tags(it, text.substr(si, ei+1 - si), tagList);
		}
		/* Find next tag */
		si = text.find('<',ei+1);
		/* Insert any text before the next tag */
		if(si!=string::npos && si!=ei+1) {
			it = rpBuf->insert_with_tags(it, text.substr(ei+1, si-(ei+1)), tagList);
		}
	}
	/* Insert any text after the last tag */
	/*
	break conditions: search for < = not found, ei points to last >
	or: search for > failed, si points to last < (which should be included)
	ALSO: loop skip condition: si = npos, ei never set past 0.
	*/
	if(si==string::npos) {
		/* If ei=0, it was never actually set, and so we want to start from index 0.
		   In any other case, start from ei+1. */
		rpBuf->insert_with_tag(it, text.substr(ei + (ei == 0 ? 0 : 1)), fontName);
	} else if(ei==string::npos) {
		rpBuf->insert_with_tag(it, text.substr(si), fontName);
	}
}
