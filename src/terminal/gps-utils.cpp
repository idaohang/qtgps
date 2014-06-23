#include "dev.gps.h"
#include "gen.gps.h"

extern void print(gpsdata*);

/*------------------------------------
-- Function: read_loop_ter
-- Date:    November 12 2013
-- Designer:    Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   void* read_loop_ter(void* thread_args)
--                  void* thread_args: pointer to void (which is later cast to thread_args_t*)
--
-- RETURNS: void*
--
-- NOTES:
-- This is a threaded function which initiates a loop that polls the gps receiver.
-- We pass in a pointer to a thread_args_t struct. This struct holds a pointer to a gpsdata object and a pointer to an 
-- "abort" variable that is set to true when the user quits.
--
-- The loop checks too see if abort is set to true. If false, the loop waits for an acknowledgment from the receiver and 
-- reads the received data into the gpsd struct, after which it calls print. If true, it exits the loop.
-- 
-- Main sets abort to true in response to user input. We set input to unbuffered by making a call to system("/bin/stty raw"), this is 
-- so that the user only needs to push the [q] key in order to exit without also needing to press [enter].
-- We set it back by calling system("/bin/stty cooked") before printing anything to stdout.
--------------------------------------*/
void* read_loop_ter(void* thread_args) 
{    

    thread_args_t* th = (thread_args_t*) thread_args;
    
    gpsdata* gpsd = *(th->gpsd);
    
    
    fprintf(stdout, "\n\n\tmygps: awaiting data ...\n");
    while( true ) { 
        
        system("/bin/stty raw");
        
        bool abort = *(th->abort);        
        
        if( abort )
            break;
        
        if( ! gps_waiting(gpsd, 2000000) ) {
            fprintf(stderr, "\tmygps: GPS timeout");
            break;
        }
        
        if(gps_read( gpsd ) == -1) {
            fprintf(stderr, "\targs:socket error 4");
            break;          
        }
        
        system("/bin/stty cooked");
        print(gpsd);

    }
    
    system("/bin/stty cooked");
    fprintf(stdout, "\n\n\n\r");
    
    return th;
}



