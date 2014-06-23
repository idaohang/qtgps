#include "ui.gps.h"

/*------------------------------------
-- Method: Thread::Thread
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- Interface:   
--              Thread::Thread(gpsdata* gpsd)
--                  gpsdata* gpsd : pointer to struct used to hold data read from gps device
--
-- NOTES:
-- Constructor for Thread class. Points local gpsd pointer ( this->gpsd ) 
-- to memory location of original gpsd structure.
--------------------------------------*/
Thread::Thread(gpsdata* gpsd)
{
    this->gpsd = gpsd;
}

/*------------------------------------
-- Method: Thread::run
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   
--              void Thread::run(void)
--
-- RETURNS: 
--              void
--
-- NOTES:
-- Executes main thread loop. 
--------------------------------------*/
void Thread::run()
{
    exec();
}

/*------------------------------------
-- Method: Thread::exec
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   
--              int Thread::exec(void)
-- RETURNS:
--            int. On termination.
--
-- NOTES:
-- Enters loop which polls gps device. Reads gps data into gpsd structure.
-- On success, emits a signal which is captured by GpsWindow. In turn, GpsWindow
-- displays the data. 
--
-- Is aborted when Thread class receives the appropriate disconnect signal
-- from GpsWindow, either on program close or via the user clicking 
-- disconnect.
--------------------------------------*/
int Thread::exec()
{   
    bool ab = abort = false;
    while( !ab  )
    {   
        mutex.lock();
        ab = abort;
        mutex.unlock();
        
        if( ! gps_waiting(gpsd, 2000000) ) {
            fprintf(stderr, "\tmygps: GPS timeout\n");
            break;
        }
                    
        if(gps_read(gpsd) == -1) {
            fprintf(stderr, "\targs:socket error 4\n");
            break;
        }
        
        emit(read_true());
        
    }
    return 1;
}
