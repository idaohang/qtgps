#include "dev.gps.h"
#include "gen.gps.h"

#define LOCAL_HOST "127.0.0.1"

extern void* read_loop_ter(void*);
extern bool  ui_mode(int, char**);
extern  int  gui(int, char**, gpsdata*);

/*------------------------------------
-- Function: main
-- Date: November 12 2013
-- Designer: Sam Youssef
-- Programmer: Sam Youssef
--
-- INTERFACE:   int main(int argc, char** argv)
--                  int argc: size of argv
--                  char** argv: array of arguments passed to program
--
-- RETURNS: int. exit status
--
-- NOTES:
-- This is the entry point for the application. Here we instantiate the necessary structures
-- that will hold receiver information and the data received. We open the gps receiver device
-- and initiate a stream.
-- 
-- If the user selects terminal mode, a new thread is generated which polls the device and prints
-- information.
-- 
-- If gui mode is selected (by default), main makes a call to gui(argc, argv, gpsd) and waits for 
-- it to complete.
--
-- Finally, main closes the stream and the device, and cleans up any allocated memory.
--------------------------------------*/
int main(int argc, char** argv) 
{       
            
    // init structs for reading from device
    static source_t source;
    static gpsdata* gpsd;
    
    unsigned int flags = WATCH_ENABLE;
    int errno = 1;    
    
    
    bool abort = false;
    struct thread_args_t thread_args = { &gpsd, &abort };
    pthread_t thread;

    source.server = (char*)LOCAL_HOST;
    source.port = (char*)DEFAULT_GPSD_PORT;
            
    gpsd = (gpsdata*)malloc(sizeof(gpsdata));    
    
    if((errno = gps_open(source.server, source.port, gpsd)) != 0) 
    {
        fprintf(stderr, "%s\n", gps_errstr(errno)); // failed to open file
        return 1;
    }

    if( source.device != NULL )
        flags = flags | WATCH_DEVICE | PACKET_SET;
                
    if((errno = gps_stream(gpsd, flags, source.device)) != 0) 
    {
        fprintf(stderr, "%s\n",gps_errstr(errno));
        return 1;
    }

    // go to ------> gui mode, set by default
    if( ui_mode(argc,argv) ) gui(argc, argv, gpsd);
    
    
    // go to ------> terminal mode, use "-t"
    else
    {               
        int ch;     
        pthread_create(&thread, NULL, read_loop_ter, (void*) &thread_args); // create thread

        while( ! abort ) {              

            if( (ch = getchar()) == 'q') 
            {
                abort = true;
                break;          
            }
        }   

        pthread_join(thread, NULL);
    }
        
        
    // clean up
    gps_stream(gpsd, WATCH_DISABLE, NULL);
    gps_close(gpsd);    
    free(gpsd); 
         
    return 0;
}

