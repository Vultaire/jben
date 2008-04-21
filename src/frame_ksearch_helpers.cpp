/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_ksearch_helpers.cpp

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

#include "frame_ksearch_helpers.h"

#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <glibmm/i18n.h>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include "kdict.h"

typedef std::pair<wchar_t, KInfo> kdict_pair;
typedef std::pair<std::string, SkipCode> SkipMiscode;

KSearchBox::KSearchBox(const Glib::ustring& label)
	: Frame(label), layout(false, 5) {
	set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	add(layout);
}

void KSearchBox::pack_start(Gtk::Widget& child,
							Gtk::PackOptions options, guint padding) {
	layout.pack_start(child, options, padding);
}

void KSearchBox::pack_end(Gtk::Widget& child,
						  Gtk::PackOptions options, guint padding) {
	layout.pack_end(child, options, padding);	
}



KSearchHW::KSearchHW()
	: KSearchBox(_("Handwriting")), leftLayout(false, 5), btnClear(_("Clear")) {
	khwp.set_size_request(-1, 75);
	btnClear.signal_clicked()
		.connect(sigc::mem_fun(*this, &KSearchHW::OnClear));

	buttons.pack_start(btnClear, Gtk::PACK_SHRINK);
	leftLayout.pack_start(khwp, Gtk::PACK_EXPAND_WIDGET);
	leftLayout.pack_start(buttons, Gtk::PACK_SHRINK);

	layout.pack_start(leftLayout, Gtk::PACK_EXPAND_WIDGET);
}

std::list<wchar_t> KSearchHW::Filter(std::list<wchar_t> srcList) {
	std::list<wchar_t> results;
	std::vector<wchar_t> vwc = khwp.GetResults();

	/* Append only the results which are also present in the source list. */
	foreach(wchar_t& wc, vwc) {
		if(find(srcList.begin(), srcList.end(), wc) != srcList.end())
			results.push_back(wc);
	}
	return results;
}

void KSearchHW::OnClear() {
	khwp.Clear();
}

KSearchStroke::KSearchStroke()
	: KSearchBox(_("Stroke Count")), leftLayout(false, 5),
	  strokeControls(false, 5),
	  allowCommonMiscounts(_("Include common stroke miscounts")),
	  miscountControls(false, 5),
	  allowMiscounts(_("Include stroke miscounts")), plusMinus(_("+/-")) {
	strokeCount.set_range(0, 40);
	strokeCount.set_digits(0);
	strokeCount.set_increments(1, 10);
	strokeCount.set_width_chars(2);
	strokeCount.set_text("0");
	strokeCount.set_activates_default(false);
	miscountAdjust.set_range(0,40);
	miscountAdjust.set_digits(0);
	miscountAdjust.set_increments(1, 1);
	miscountAdjust.set_width_chars(2);
	miscountAdjust.set_text("0");
	miscountAdjust.set_sensitive(false);

	allowMiscounts.signal_toggled()
		.connect(sigc::mem_fun(*this, &KSearchStroke::OnMiscountToggled));

	strokeControls.pack_start(strokeCount, Gtk::PACK_SHRINK);
	miscountControls.pack_start(allowMiscounts, Gtk::PACK_SHRINK);
	miscountControls.pack_start(plusMinus, Gtk::PACK_SHRINK);	
	miscountControls.pack_start(miscountAdjust, Gtk::PACK_SHRINK);
	leftLayout.pack_start(strokeControls, Gtk::PACK_SHRINK);
	leftLayout.pack_start(allowCommonMiscounts, Gtk::PACK_SHRINK);
	leftLayout.pack_start(miscountControls, Gtk::PACK_SHRINK);
	layout.pack_start(leftLayout, Gtk::PACK_EXPAND_WIDGET);
}

std::list<wchar_t> KSearchStroke::Filter(std::list<wchar_t> srcList) {
	std::list<wchar_t> results;

	int mcAdj = 0;
	if(allowMiscounts.get_active())
		mcAdj = miscountAdjust.get_value_as_int();
	bool commonMistrokes = allowCommonMiscounts.get_active();
	const KDict* kd = KDict::Get();
	const KInfo* ki;
	int sc = strokeCount.get_value_as_int();

	/* Append only the results which are also present in the source list. */
	foreach(wchar_t& wc, srcList) {
		ki = kd->GetEntry(wc);
		if(ki->strokeCount >= sc - mcAdj && ki->strokeCount <= sc + mcAdj)
			results.push_back(wc);
		else if(commonMistrokes) {
			/* I'm -not- going to apply the miscount adjust when searching
			   lists of common mistrokes - kind of defeats the purpose, I
			   think. */
			foreach(const int& miscount, ki->misstrokes) {
				if(miscount == sc) {
					results.push_back(wc);
					break;
				}
			}
		}
	}
	return results;
}

void KSearchStroke::OnMiscountToggled() {
	miscountAdjust.set_sensitive(allowMiscounts.get_active());
}

KSearchSKIP::KSearchSKIP()
	: KSearchBox(_("SKIP search")), leftLayout(false, 5),
	  skipControls(false, 5), sep1(_("-")), sep2(_("-")),
	  allowMiscodes(_("Include SKIP miscodes")), miscountControls(false, 5),
	  allowMiscounts(_("Include stroke miscounts")), plusMinus(_("+/-")) {

	/* Based on KANJIDIC2, the max values for these fields are as follows:
	   index1: 4 (by SKIP definition; should never change)
	   index2: 22
	   index3: 28

	   HOWEVER, let's not hard-code them.  Set upper bounds to 30 on
	   index2/index3, and if we really need a specific upper bound, have it
	   determined on dictionary load time later. */

	index1.set_range(0, 4);
	index1.set_digits(0);
	index1.set_increments(1, 1);
	index1.set_width_chars(1);
	index1.set_text("0");
	index1.set_activates_default(false);
	index2.set_range(0, 30);
	index2.set_digits(0);
	index2.set_increments(1, 10);
	index2.set_width_chars(2);
	index2.set_text("0");
	index2.set_activates_default(false);
	index3.set_range(0, 30);
	index3.set_digits(0);
	index3.set_increments(1, 10);
	index3.set_width_chars(2);
	index3.set_text("0");
	index3.set_activates_default(false);
	miscountAdjust.set_range(0,30);
	miscountAdjust.set_digits(0);
	miscountAdjust.set_increments(1, 1);
	miscountAdjust.set_width_chars(2);
	miscountAdjust.set_text("0");
	miscountAdjust.set_sensitive(false);
	allowMiscounts.signal_toggled()
		.connect(sigc::mem_fun(*this, &KSearchSKIP::OnMiscountToggled));

	skipControls.pack_start(index1, Gtk::PACK_SHRINK);
	skipControls.pack_start(sep1, Gtk::PACK_SHRINK);
	skipControls.pack_start(index2, Gtk::PACK_SHRINK);
	skipControls.pack_start(sep2, Gtk::PACK_SHRINK);
	skipControls.pack_start(index3, Gtk::PACK_SHRINK);
	miscountControls.pack_start(allowMiscounts, Gtk::PACK_SHRINK);
	miscountControls.pack_start(plusMinus, Gtk::PACK_SHRINK);
	miscountControls.pack_start(miscountAdjust, Gtk::PACK_SHRINK);
	leftLayout.pack_start(skipControls, Gtk::PACK_SHRINK);
	leftLayout.pack_start(allowMiscodes, Gtk::PACK_SHRINK);
	leftLayout.pack_start(miscountControls, Gtk::PACK_SHRINK);
	layout.pack_start(leftLayout, Gtk::PACK_EXPAND_WIDGET);
}

std::list<wchar_t> KSearchSKIP::Filter(std::list<wchar_t> srcList) {
	std::list<wchar_t> results;

	int mcAdj = 0;
	if(allowMiscounts.get_active())
		mcAdj = miscountAdjust.get_value_as_int();
	bool miscodes = allowMiscodes.get_active();
	const KDict* kd = KDict::Get();
	const KInfo* ki;
	int i1 = index1.get_value_as_int();
	int i2 = index2.get_value_as_int();
	int i3 = index3.get_value_as_int();

	/* Append only the results which are also present in the source list. */
	foreach(wchar_t& wc, srcList) {
		ki = kd->GetEntry(wc);
		if((i1 == 0 || ki->qc_skip.i1 == i1) &&
		   (i2 == 0 ||
			(ki->qc_skip.i2 >= i2 - mcAdj && ki->qc_skip.i2 <= i2 + mcAdj)) &&
		   (i3 == 0 ||
			(ki->qc_skip.i3 >= i3 - mcAdj && ki->qc_skip.i3 <= i3 + mcAdj)))
			results.push_back(wc);
		else if(miscodes) {
			foreach(const SkipMiscode& code, ki->skipMisclass) {
				if((i1 == 0 || code.second.i1 == i1) &&
				   (i2 == 0 ||
					(code.second.i2 >= i2 - mcAdj &&
					 code.second.i2 <= i2 + mcAdj)) &&
				   (i3 == 0 ||
					(code.second.i3 >= i3 - mcAdj &&
					 code.second.i3 <= i3 + mcAdj))) {
					results.push_back(wc);
					break;
				}
			}
		}
	}
	return results;
}

void KSearchSKIP::OnMiscountToggled() {
	miscountAdjust.set_sensitive(allowMiscounts.get_active());
}
