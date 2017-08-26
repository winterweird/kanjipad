// NOTE: I'm not sure it makes sense to have a header like this, but screw it.

#ifndef KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER
#define KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER

#define WCHAR_EQ(a,b) (a.d[0] == b.d[0] && a.d[1] == b.d[1])

// used for defining the size of a global array variable
#define MAX_GUESSES 10

#define BUFLEN 256

#endif /* KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER */
