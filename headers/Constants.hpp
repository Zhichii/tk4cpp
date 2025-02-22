/* Symbolic constants for Tk. */

#ifndef TK4CPP_CONSTANTS
#define TK4CPP_CONSTANTS

namespace tki {

	namespace constants {

		// Booleans
		const static bool NO = 0;
		const static bool FALSE = 0;
		const static bool OFF = 0;
		const static bool YES = 1;
		const static bool TRUE = 1;
		const static bool ON = 1;

		// -anchor and -sticky
		const static char N[] = "n";
		const static char S[] = "s";
		const static char W[] = "w";
		const static char E[] = "e";
		const static char NW[] = "nw";
		const static char SW[] = "sw";
		const static char NE[] = "ne";
		const static char SE[] = "se";
		const static char NS[] = "ns";
		const static char EW[] = "ew";
		const static char NSEW[] = "nsew";

		// -fill
		const static char NONE[] = "none";
		const static char X[] = "x";
		const static char Y[] = "y";
		const static char BOTH[] = "both";

		// -side
		const static char LEFT[] = "left";
		const static char TOP[] = "top";
		const static char RIGHT[] = "right";
		const static char BOTTOM[] = "bottom";

		// -relief
		const static char RAISED[] = "raised";
		const static char SUNKEN[] = "sunken";
		const static char FLAT[] = "flat";
		const static char RIDGE[] = "ridge";
		const static char GROOVE[] = "groove";
		const static char SOLID[] = "solid";

		// -orient
		const static char HORIZONTAL[] = "horizontal";
		const static char VERTICAL[] = "vertical";

		// -tabs
		const static char NUMERIC[] = "numeric";

		// -warp
		const static char CHAR[] = "char";
		const static char WORD[] = "word";

		// -align
		const static char BASELINE[] = "baseline";

		// -bordermore
		const static char INSIDE[] = "inside";
		const static char OUTSIDE[] = "outside";

		// Special tags, marks and insert positions
		const static char SEL[] = "sel";
		const static char SEL_FIRST[] = "sel_first";
		const static char SEL_LAST[] = "sel_last";
		const static char END[] = "end";
		const static char INSERT[] = "insert";
		const static char CURRENT[] = "current";
		const static char ANCHOR[] = "anchor";
		const static char ALL[] = "all"; // e.g. Canvas.delete(ALL);

		// Text widget and button states
		const static char NORMAL[] = "normal";
		const static char DISABLED[] = "disabled";
		const static char ACTIVE[] = "active";
		// Canvas state
		const static char HIDDEN[] = "hidden";

		// Menu item types
		const static char CASCADE[] = "cascade";
		const static char CHECKBUTTON[] = "checkbutton";
		const static char COMMAND[] = "command";
		const static char RADIOBUTTON[] = "radiobutton";
		const static char SEPARATOR[] = "separator";

		// Selection modes for list boxes
		const static char SINGLE[] = "single";
		const static char BROWSE[] = "browse";
		const static char MULTIPLE[] = "multiple";
		const static char EXTENDED[] = "extended";

		// Activestyle for list boxes
		// const static char NONE[]="none"; is also valid
		const static char DOTBOX[] = "dotbox";
		const static char UNDERLINE[] = "underline";

		// Various canvas styles
		const static char PIESLICE[] = "pieslice";
		const static char CHORD[] = "chord";
		const static char ARC[] = "arc";
		const static char FIRST[] = "first";
		const static char LAST[] = "last";
		const static char BUTT[] = "butt";
		const static char PROJECTING[] = "projecting";
		const static char ROUND[] = "round";
		const static char BEVEL[] = "bevel";
		const static char MITER[] = "miter";

		// Arguments to xview/yview
		const static char MOVETO[] = "moveto";
		const static char SCROLL[] = "scroll";
		const static char UNITS[] = "units";
		const static char PAGES[] = "pages";

	}
}

#endif