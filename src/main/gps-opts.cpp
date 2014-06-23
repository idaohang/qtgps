#include <cstring>

/*--------------------------------------
-- Function: ui_mode
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   bool ui_mode(int argc, char** argv)
--                  int argc: size of argv
--                  char** argv: array of arguments passed to program
--
-- RETURNS: bool. False if -t (terminal mode) is requested, true otherwise.
--
-- NOTES:
-- Sets the program's mode: gui vs. terminal. Checks if "-t" flag is set, which changes the program
-- to run in the terminal.
--------------------------------------*/
bool ui_mode(int argc, char** argv)
{
    for( int i = 0; i < argc; ++i ) 
        if( strcmp(argv[i], "-t") == 0 ) return false;
    return true;
}



