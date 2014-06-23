#include "dev.gps.h"
#include <time.h>
#include <cstdio>


/*------------------------------------
-- Function: print
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE: void print(gpsdata* g)
--              gpsdata* g: the data structure containing what's read from the gps device; to be printed
--
-- RETURNS: void.
--
-- NOTES:
-- Prints the values read from the gps device to console. Formats the timestamp before printing.
--------------------------------------*/
void print(gpsdata* g)
{
    int  nsats = g->satellites_visible;
    char tbuf[MAX_BF + 1] = { 0 };

    for(int i = 0; i < nsats; ++i) {
        printf("\nPRN: %d\tElevation: %d\tAzimuth: %d\tSNR: %.2f\tUsed: %s", g->PRN[i], g->elevation[i], g->azimuth[i], g->ss[i], (g->used[i] ? "Y" : "N"));
    }
    
    unix_to_iso8601(g->fix.time, tbuf, sizeof(tbuf));
    printf("\n\nLatitude: %.2f\nLongitude: %.2f\n%s\n\n", g->fix.latitude, g->fix.longitude, tbuf);
    
    printf("\n------------------------------------- press [q] to quit \n\n\n");
}
