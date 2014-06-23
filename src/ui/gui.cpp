#include "ui.gps.h"

/*------------------------------------
-- Function: gui
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   
--              gui(int argc, char** argv, gpsdata* gpsd)
--                  int argc: size of argv
--                  char** argv: arguments passed to program (in main), required for Qt
--                  gpsdata* gpsd: pointer to data struct for storing data read from gps device
--
-- RETURNS: int. Qt exit status.
--
-- NOTES:
-- Called from main. Creates a Qt window object (GpsWindow),
-- sets its viewing parameters, and executes the application.
--------------------------------------*/
int gui(int argc, char** argv, gpsdata* gpsd)
{
        // create application and gui window
        QApplication gps(argc, argv);
        GpsWindow window(gpsd);

        window.resize(WINDOWWIDTH, WINDOWHEIGHT);
        window.move(50, 50);
        window.setWindowTitle("My[Qt]Gps");
        window.show();

        return gps.exec();
}
