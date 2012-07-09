#Copyright 2007 Cory Cross

#    This symbol is covered under the GNU GPL, with the following exception.
#    See the file gpl.txt for the full license.
#As a special exception, if you create a design which uses this footprint,
#  and embed this footprint or unaltered portions of this footprint into the
#  design, this footprint does not by itself cause the resulting design to
#  be covered by the GNU General Public License. This exception does not
#  however invalidate any other reasons why the design itself might be
#  covered by the GNU General Public License. If you modify this
#  footprint, you may extend this exception to your version of the
#  footprint, but you are not obligated to do so. If you do not
#  wish to do so, delete this exception statement from your version.

Element["" "Terminal blocks with 200mil spacing" "CONN" "" 0 0 0 -35000 0 100 ""]
(
	Attribute("gedasymbols::url" "http://www.gedasymbols.org/user/cory_cross/footprints/TERMBLOCK200-V-10d.fp")
	Pin[-90000 0 10000 2400 11000 5600 "" "1" "square"]
	Pin[-70000 0 10000 2400 11000 5600 "" "2" ""]
	Pin[-50000 0 10000 2400 11000 5600 "" "3" ""]
	Pin[-30000 0 10000 2400 11000 5600 "" "4" ""]
	Pin[-10000 0 10000 2400 11000 5600 "" "5" ""]
	Pin[10000 0 10000 2400 11000 5600 "" "6" ""]
	Pin[30000 0 10000 2400 11000 5600 "" "7" ""]
	Pin[50000 0 10000 2400 11000 5600 "" "8" ""]
	Pin[70000 0 10000 2400 11000 5600 "" "9" ""]
	Pin[90000 0 10000 2400 11000 5600 "" "10" ""]
	ElementLine [-100000 30000 100000 30000 1000]
	ElementLine [-100000 -25000 100000 -25000 1000]
	ElementLine [-100000 30000 -100000 -25000 1000]
	ElementLine [100000 30000 100000 -25000 1000]
)
