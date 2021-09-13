#ifndef _LOCATION_INTERFACE_H_
#define _LOCATION_INTERFACE_H_

#include "gps.h"
#include "gps_extended_c.h"

#define TRUE 1
#define FALSE 0

#define XTRA_DATA_BUF_LEN 200000
#define XTRA_DATA_FILE_NAME "localxtra.bin"
#define XTRA_DEFAULT_SERVER1 "http://xtrapath1.izatcloud.net/xtra3grc.bin"
#define XTRA_DEFAULT_SERVER2 "http://xtrapath2.izatcloud.net/xtra3grc.bin"
#define XTRA_DEFAULT_SERVER3 "http://xtrapath3.izatcloud.net/xtra3grc.bin"
#define XTRA_USER_AGENT_STRING "LE/1.2.3/OEM/Model/Board/Carrier" // Test user agent string
#define XTRA_NTP_DEFAULT_SERVER1 "time.izatcloud.net"

#define S_DEVICE_PATH "/dev/smd8"
#define LOC_MAX_FILE_PATH_LEN 50

typedef unsigned char boolean;
typedef unsigned char uint8;

typedef void (* garden_location_cb_t) (GpsLocation* location);
typedef void (* garden_status_cb_t) (GpsStatus *status);
typedef void (* garden_sv_status_cb_t) (GpsSvStatus* sv_info);
typedef void (* garden_nmea_cb_t)(int64_t timestamp, const char* nmea, int length);
typedef void (* garden_set_cap_cb_t)(uint32_t capabilities);
typedef void (* garden_request_utc_time_cb_t)();
typedef void (* garden_xtra_report_server_cb_t)(const char* server1, const char* server2, const char* server3);
typedef void (* garden_xtra_download_request_cb_t)();
typedef void (* garden_agps_status_cb_t)(AGpsStatus* status);
typedef void (* garden_agps_ext_status_cb_t)(AGpsExtStatus* status);
typedef void (* garden_ni_notify_cb_t)(GpsNiNotification *notification);

typedef void (* garden_acquire_wakelock_cb_t)();
typedef void (* garden_release_wakelock_cb_t)();
typedef pthread_t (* garden_create_thread_cb_t)(const char* name,
                                             void (*start)(void *),
                                             void* arg);

typedef struct {
    garden_location_cb_t                garden_location_cb;
    garden_status_cb_t                  garden_status_cb;
    garden_sv_status_cb_t               garden_sv_status_cb;
    garden_nmea_cb_t                    garden_nmea_cb;
    garden_set_cap_cb_t                 garden_set_cap_cb;
    garden_request_utc_time_cb_t        garden_request_utc_time_cb;
    garden_xtra_report_server_cb_t      garden_xtra_report_server_cb;
    garden_xtra_download_request_cb_t   garden_xtra_download_request_cb;
    garden_agps_ext_status_cb_t         garden_agps_ext_status_cb;
    garden_ni_notify_cb_t               garden_ni_notify_cb;
    garden_acquire_wakelock_cb_t        garden_acquire_wakelock_cb;
    garden_release_wakelock_cb_t        garden_release_wakelock_cb;
    garden_create_thread_cb_t           garden_create_thread_cb;
} garden_ext_cb_t;

typedef struct location_c_Interface {
    int (*location_init) (garden_ext_cb_t *pcb);
    int (*location_set_position_mode) (uint32_t mode, uint32_t recurrence, uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time);
    int (*location_start_navigation)(void);
    int (*location_stop_navigation)(void);
    int (*location_close)(void);
    int (*location_delete_aiding_data)(uint32_t flags);
    int (*location_inject_time)(int64_t time, int64_t timeReference, int uncertainty);
    int (*location_inject_location)(double latitude, double longitude, float accuracy);
    int (*location_xtra_inject_data)(char *data, int length);
} LocationInterface;

typedef LocationInterface * loc_test_interface_func_t();
LocationInterface const* get_location_client_interface();


void test_garden_location_cb (GpsLocation * location);
void test_garden_status_cb (GpsStatus * status);
void test_garden_sv_status_cb (GpsSvStatus * sv_info);
void test_garden_nmea_cb (GpsUtcTime timestamp, const char *nmea, int length);
void test_garden_set_capabilities_cb (uint32_t capabilities);
void test_garden_xtra_time_req_cb ();
void test_garden_xtra_report_server_cb(const char* server1, const char* server2, const char* server3);
void test_garden_xtra_download_req_cb ();
void test_agps_status_cb (AGpsExtStatus * status);
void test_garden_ni_notify_cb(GpsNiNotification *notification);

void test_garden_acquire_wakelock_cb ();
void test_garden_release_wakelock_cb ();
pthread_t test_garden_create_thread_cb (const char *name, void (*start) (void *),
                                     void *arg);

void test_xtra_client_data_callback(char * data,int length);
void test_xtra_client_time_callback(int64_t utcTime, int64_t timeReference, int uncertainty);
void garden_print(const char *fmt, ...);
#endif //_GPS_CONTROL_H_

