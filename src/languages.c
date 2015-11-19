#include <pebble.h>
#include "languages.h"

/*
LANGUAGE CODES

english 0
french 1
spanish 2
german 3

*/

// Days of the week
const char* dayNames[4][7] = {
	{"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"},
	{"DIM", "LUN", "MAR", "MER", "JEU", "VEN", "SAM"},
	{"DO", "LU", "MA", "MI", "JU", "VI", "SA"},
	{"SO", "MO", "DI", "MI", "DO", "FR", "SA"},
};

// Months of the year
const char* monthNames[4][12] = {
	{"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"},
	{"JAN", "FEV", "MAR", "AVR", "MAI", "JUI", "JUL", "AOU", "SEP", "OCT", "NOV", "DEC"},
	{"ENE", "FEB", "MAR", "ABR", "MAY", "JUN", "JUL", "AGO", "SET", "OCT", "NOV", "DIC"},
	{"JAN", "FEB", "MÄR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEZ"}
};

// Codes for fetching weather
char* langCodes[4] = {
	"en",
	"fr",
	"es",
	"de"
};