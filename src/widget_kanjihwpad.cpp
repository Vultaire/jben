/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: widget_kanjihwpad.cpp

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

#include "widget_kanjihwpad.h"
#include <cairomm/context.h>
#include <boost/format.hpp>
#include <sstream>
#include <ctime>
#include "string_utils.h"
#include "errorlog.h"
#include <glibmm/i18n.h>
#include <gdk/gdk.h>
#include "jben_defines.h"
#include <cstring>

#ifndef KPENGINE_DATADIR
	#ifdef __WIN32__
		#define KPENGINE_DATADIR JB_DATADIR "\\kpengine_data"
	#else
		#define KPENGINE_DATADIR JB_DATADIR "/kpengine_data"
	#endif
#endif

#ifdef __WIN32__
	/* NOT clearly documented.  To use read/write/open/close/etc, include io.h.
	   On Windows they're preceded with an underscore, but GLib adds
	   #defines for cross-platform convenience. */
	#include <io.h>
#endif

/**
 * A widget for accepting handwritten kanji.
 * This widget looks up kanji characters based on user input.  Updates to the
 * results can be captured by catching the button-release-event signal.
 * Button 1 means a line got added, button 3 means one was removed.  This
 * difference should be irrelevant outside the widget implementation; just
 * check for updates on any button-release-event signal.
 */
KanjiHWPad::KanjiHWPad() {
	/* The label line was added for Win32 builds since my dev version of GTK
	   doesn't handle labelless frames properly. */
	set_label(_("Draw Kanji (Right click erases)"));
	set_shadow_type(Gtk::SHADOW_IN);
	add(da);
	da.add_events(
#ifndef __WIN32__
				  Gdk::POINTER_MOTION_HINT_MASK |
#endif
				  Gdk::BUTTON1_MOTION_MASK |
				  Gdk::BUTTON_PRESS_MASK |
				  Gdk::BUTTON_RELEASE_MASK);
	da.signal_expose_event()
		.connect(sigc::mem_fun(*this, &KanjiHWPad::OnExpose));
	da.signal_motion_notify_event()
		.connect(sigc::mem_fun(*this, &KanjiHWPad::OnMotion));
	da.signal_button_press_event()
		.connect(sigc::mem_fun(*this, &KanjiHWPad::OnPress));
	da.signal_button_release_event()
		.connect(sigc::mem_fun(*this, &KanjiHWPad::OnRelease));

	show_all_children();
}

const std::vector<wchar_t>& KanjiHWPad::GetResults() {
	return results;
}

bool KanjiHWPad::OnExpose(GdkEventExpose* event) {
	Glib::RefPtr<Gdk::Window> rw = da.get_window();
	if(rw) {
		Cairo::RefPtr<Cairo::Context> rc = rw->create_cairo_context();
		/* Clip update region */
		rc->rectangle(event->area.x, event->area.y,
					  event->area.width, event->area.height);
		rc->clip();

		/* Set drawing style */
		rc->set_line_width(3);
		rc->set_line_join(Cairo::LINE_JOIN_ROUND);
		rc->set_line_cap(Cairo::LINE_CAP_ROUND);

		/* Fill background */
		rc->set_source_rgb(1,1,1);
		rc->paint();

		/* Create path from stored data */
		std::list< std::list< std::pair<int,int> > >::iterator itLine;
		std::list< std::pair<int,int> >::iterator itPoint;
		for(itLine = listLines.begin(); itLine != listLines.end(); itLine++) {
			if(itLine->size()<2) continue;
			itPoint = itLine->begin();
			rc->move_to(itPoint->first, itPoint->second);
			for(itPoint++; itPoint != itLine->end(); itPoint++) {
				rc->line_to(itPoint->first, itPoint->second);
			}
		}

		/* Draw lines */
		rc->set_source_rgb(0,0,0);
		rc->stroke();
	}
	return true;
}

bool KanjiHWPad::OnMotion(GdkEventMotion* event) {
	if(event->state & Gdk::BUTTON1_MASK) {
		if(pCurrentLine)
			pCurrentLine->push_back(std::pair<int,int>
				((int)event->x, (int)event->y));
		Update();
	}
#ifndef __WIN32__
	gdk_event_request_motions(event);
#endif
	return true;
}

bool KanjiHWPad::OnPress(GdkEventButton* event) {
	if(event->button==1) {
		/* Put a new line in the list and retrieve it. */
		listLines.push_back(std::list< std::pair<int,int> >());
		pCurrentLine = &listLines.back();
		pCurrentLine->push_back(std::pair<int,int>(event->x, event->y));
		Update();
	}
	return true;
}

bool KanjiHWPad::OnRelease(GdkEventButton* event) {
	switch(event->button) {
	case 1:
		if(pCurrentLine) {
			pCurrentLine->push_back(std::pair<int,int>(event->x, event->y));
			pCurrentLine = NULL;
		}
		Update();
		LookupChars();
		return false;
	case 3:
	    if(listLines.size()>0) {
			pCurrentLine = NULL;
			listLines.pop_back();
			Update();
			LookupChars();
			return false;
		}
	}
	return true;
}

void KanjiHWPad::Clear() {
	pCurrentLine = NULL;
	listLines.clear();
	results.clear();
	Update();
}

void KanjiHWPad::Update() {
	gdk_window_invalidate_rect(da.get_window()->gobj(), NULL, false);
}

void KanjiHWPad::LookupChars() {
	/* Make our call to jben_kpengine(.exe) and get candidate chars */
	results.clear();
	if(listLines.size()>0) {
		/* Here we do a Gtk-style async process call... ug-ly!  */
		const char exename[] = "jben_kpengine";
		const char args[] = "-d";

		/* Prep command line data */
		char **argv;
		argv = new char*[4];
		argv[0] = new char[strlen(exename)+1];
		argv[1] = new char[strlen(args)   +1];
		argv[2] = new char[strlen(KPENGINE_DATADIR)+1];
		argv[3] = NULL;
		strcpy(argv[0], exename);
		strcpy(argv[1], args);
		strcpy(argv[2], KPENGINE_DATADIR);

		/* Prep data for stdin */
		std::ostringstream oss;
		for(std::list< std::list< std::pair<int,int> > >::iterator
				li=listLines.begin(); li!=listLines.end(); li++) {
			for(std::list< std::pair<int,int> >::iterator
					pi=li->begin(); pi!=li->end(); pi++) {
				oss << boost::format("%d %d ") % pi->first % pi->second;
			}
			oss << '\n';
		}
		oss << '\n';

		/* Call kpengine */
		GPid child_pid;
		gint standard_in, standard_out, standard_err;
		GError *error;
		gboolean result
			= g_spawn_async_with_pipes
			(NULL, /* working dir */
			 argv,
			 NULL, /* environment - default to parent */
			 (GSpawnFlags) (G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
			 NULL, /* child_setup func and user_data, which are */
			 NULL, /*   not useful for Windows builds */
			 &child_pid,
			 &standard_in, &standard_out, &standard_err, &error);
		if(result) {
			ssize_t sz;

			/* Write to our engine */
			sz = write(standard_in, oss.str().c_str(), oss.str().length());
			int result = close(standard_in);
			if(result) {
				el.Push(EL_Error,
						(boost::format(_("%s:%d: An error occurred while "
										 "closing the output stream to "
										 "kpengine."))
						 % __FILE__ % __LINE__).str());
			}

			/* Wait up to 3 seconds to retrieve data */
			std::ostringstream ossOut, ossErr;
			char buffer[80];
			memset((void*)buffer, 0, 80);
			time_t st = std::time(NULL);
			while(std::time(NULL) - st < 3) {
				sz = read(standard_out, buffer, 79);
				if(sz) {
					buffer[sz]=0;
					ossOut << buffer;
				}
				if(!sz) {
					sz = read(standard_err, buffer, 79);
					if(sz) {
						buffer[sz]=0;
						ossErr << buffer;
					}
				}
				if(!sz) {
					/* Nothing came in to either stream this time. */
					/* Check for newline on each stream; that signals a full
					   return message. */
					if(ossOut.str().find('\n')!=std::string::npos) break;
					if(ossErr.str().find('\n')!=std::string::npos) break;
				}
			}

			/* cleanup our child process's stuff */
			g_spawn_close_pid(child_pid);
			close(standard_out);
			close(standard_err);

			if(ossErr.str().empty() && (!ossOut.str().empty())) {
				/* Create new wchar_t list */
				/* First: trim the response string */
				std::string data = ossOut.str().substr(1); /* skip the "k" */
				size_t pos = data.find('\n');
				int temp;
				if(pos!=std::string::npos) data = data.substr(0,pos);
				/* Tokenize */
				std::list<std::string> l = StrTokenize<char>(data, " ");
				for(std::list<std::string>::iterator
						it = l.begin(); it != l.end(); it++) {
					if(!it->empty()) {
						temp = atoi(it->c_str());
						if(temp > 0xFFFF || temp < 32) {
							el.Push(EL_Error,
									(boost::format(
										_("%s:%d: kpengine returned an out-of-"
										  "range character (0x%X).  The "
										  "character has been dropped."))
									 % __FILE__ % __LINE__ % temp).str());
						} else
							results.push_back((wchar_t)temp);
					}
				}
			}
		} else {
			ostringstream oss;
			if (error->domain == G_SPAWN_ERROR) {
				el.Push(EL_Error,
					(boost::format(_("%s:%d: G_SPAWN_ERROR, code = %d, message = \"%s\""))
					 % __FILE__ % __LINE__
					 % error->code % error->message).str());
			} else {
				el.Push(EL_Error,
					(boost::format(_("%s:%d: Bad call to kpengine!  Domain = %d, code = %d, message = \"%s\""))
					 % __FILE__ % __LINE__
					 % error->domain % error->code % error->message).str());
			}
		}

		/* cleanup */
		for(int i=0;i<3;i++)
			delete[] argv[i];
		delete[] argv;
	}
}
