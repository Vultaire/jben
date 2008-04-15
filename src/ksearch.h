#ifndef ksearch_h
#define ksearch_h

#include <list>
#include <string>

/**
 * List filter based on SKIP index code.
 * Keeps all characters matching the specified SKIP pattern.
 * If a 0 is specified for any of the indexes, the field is considered as a
 * wildcard and is not filtered upon.
 * For performance reasons, the indexes will be checked in reverse order.
 */
std::list<wchar_t> KSearchSKIP(const std::list<wchar_t>& l, int i1, int i2,
  int i3, bool miscodes=false, bool miscounts=false, int miscountAdjust=1);

/**
 * List filter based on stroke range.
 * Only keeps characters within the specified range.
 * If the ranges are invalid, then:
 * - If the low bound is higher than the high bound: the bounds are reversed,
 *   and the function proceeds.
 * - If a bound is outside the bounds of the list: the function just does as
 *   much as it can.
 */
std::list<wchar_t> KSearchStrokeRange(const std::list<wchar_t>& l,
  int lowStroke, int highStroke, bool miscounts=false);

/**
 * List filter based on a radical list.
 * Removes all characters from the list which do not have ALL the specified radicals.
 */
std::list<wchar_t> KSearchMultirad(const std::list<wchar_t>& l,
  std::string radicals);

#endif
