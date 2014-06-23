#ifndef DCGPS_H
#define DCGPS_H
#include <gps.h>

#define MAX_BF 100

typedef struct fixsource_t
{
    char *spec;
    char *server;
    char *port;
    char *device;
} source_t;


typedef struct gps_data_t gpsdata;


struct thread_args_t {

    gpsdata** gpsd;
    bool* abort;

};
#endif
