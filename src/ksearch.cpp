#include "ksearch.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace std;
#include "kdict.h"
#include "string_utils.h"

typedef std::pair<std::string, SkipCode> SkipMiscode;
list<wchar_t> KSearchSKIP(const list<wchar_t>& l, int i1, int i2, int i3,
  bool miscodes, bool miscounts, int miscountAdjust) {
	list<wchar_t> results;
	if(i1<0 || i2<0 || i3<0) return results; /* Indexes are >= 0 only */

	list<string> ls;
	list<SkipCode> lsc;
	if(!miscounts) miscountAdjust = 0;
	const KDict* kd = KDict::Get();
	bool b;

	foreach(const wchar_t& wc, l) {
		lsc.clear();

		/* Add "official" skip code to list for checking. */
		const KInfo* ki = kd->GetEntry(wc);
		lsc.push_back(ki->qc_skip);

		/* Add miscodes to list, if miscodes are being searched. */
		if(miscodes) {
			foreach(const SkipMiscode& smc, ki->skipMisclass) {
				lsc.push_back(smc.second);
			}
		}

		/* Check all values, and append/break on matches. */
		foreach(SkipCode& sc, lsc) {
			if(!miscounts) {
				b = (i1 == 0 || sc.i1 == i1) &&
					(i2 == 0 || sc.i2 == i2) &&
					(i3 == 0 || sc.i3 == i3);
			} else {
				b = (i1 == 0 ||
					  (sc.i1 >= (i1-miscountAdjust) &&
					   sc.i1 <= (i1+miscountAdjust))) &&
					(i1 == 0 ||
					  (sc.i2 >= (i2-miscountAdjust) &&
					   sc.i2 <= (i2+miscountAdjust))) &&
					(i1 == 0 ||
					  (sc.i3 >= (i3-miscountAdjust) &&
					   sc.i3 <= (i3+miscountAdjust)));
			}

			if(b) {
				results.push_back(wc);
				break;
			}
		}
	}

	return results;
}

list<wchar_t> KSearchStrokeRange(const list<wchar_t>& l, int lowStroke,
  int highStroke, bool miscounts) {
	if(highStroke < lowStroke) {
		int temp = highStroke;
		highStroke = lowStroke;
		lowStroke = temp;
	}

	list<wchar_t> results;
	const KDict* kd = KDict::Get();
	foreach(const wchar_t& wc, l) {
		const KInfo* ki = kd->GetEntry(wc);
		/* Main comparison: official stroke count vs specified range */
		if(ki->strokeCount >= lowStroke && ki->strokeCount <= highStroke) {
			results.push_back(wc);
			continue;
		}
		/* Optional: search through list of misstroke values for a match */
		if(miscounts) {
			foreach(const int& mc, ki->misstrokes) {
				if(mc >= lowStroke && mc <= highStroke) {
					results.push_back(wc);
					break;
				}
			}
		}
	}
	return results;
}

#include <iostream>
list<wchar_t> KSearchMultirad(const list<wchar_t>& l, string radicals) {
	cerr << "KSearchMultirad is not yet implemented." << endl;
	/*KDict* kd = KDict::Get();*/
	list<wchar_t> results = l;
	return results;
}
