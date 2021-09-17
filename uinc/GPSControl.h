#ifndef _GPS_CONTROL_H_
#define _GPS_CONTROL_H_

#include "common.h"
#include "simcom_common.h"

typedef unsigned char boolean;

/** Represents a location. */
typedef struct {
    /** Represents latitude in degrees. */
    double          latitude;
    /** Represents longitude in degrees. */
    double          longitude;
    /** Represents altitude in meters above the WGS 84 reference
     * ellipsoid. */
    double          altitude;
    /** Represents speed in meters per second. */
    float           speed;
    /** Represents heading in degrees. */
    float           bearing;
    /** Represents expected accuracy in meters. */
    float           accuracy;
    /** Y-M-D H-M-S*/
    uint8_t  time[6];
} GpsInfo;
typedef void (*gps_ind_cb_fcn)(uint8_t simcom_event, void *gps_info);

boolean gps_init(gps_ind_cb_fcn gps_ind_cb);
void      gps_deinit();
boolean gps_xtra_enable();
boolean gps_xtra_disable();
boolean gps_coldstart();
boolean gps_hotstart();
boolean gps_stop();


#endif //_GPS_CONTROL_H_
