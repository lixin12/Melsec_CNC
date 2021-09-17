#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <sys/poll.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <dlfcn.h> 

#include "GPSControl.h"
#include "xtra_system_interface.h"
#include "location_interface.h"

gps_ind_cb_fcn test_gps_ind_cb;

static boolean is_xtra_use = FALSE;

// Required by Xtra Client init function
XtraClientDataCallbacksType myXtraClientDataCallback = {
    test_xtra_client_data_callback
};

// Required by Xtra Client function
XtraClientTimeCallbacksType myXtraClientTimeCallback = {
    test_xtra_client_time_callback
};

static LocationInterface * location_test_interface_ptr = NULL;
static XtraClientInterfaceType *pXtraClientInterface = NULL;
static void *loc_lib_handle;
static void *xtra_lib_handle;

// Xtra data file
FILE * g_xtra_data_file;

XtraClientConfigType g_xtra_client_config = {
    XTRA_NTP_DEFAULT_SERVER1,
    XTRA_DEFAULT_SERVER1,
    XTRA_DEFAULT_SERVER2,
    XTRA_DEFAULT_SERVER3,
    XTRA_USER_AGENT_STRING
};

static garden_ext_cb_t my_garden_cb = {
    &test_garden_location_cb,
    &test_garden_status_cb,
    &test_garden_sv_status_cb,
    &test_garden_nmea_cb,
    &test_garden_set_capabilities_cb,
    &test_garden_xtra_time_req_cb,
    &test_garden_xtra_report_server_cb,
    &test_garden_xtra_download_req_cb,
    &test_agps_status_cb,
    &test_garden_ni_notify_cb,
    &test_garden_acquire_wakelock_cb,
    &test_garden_release_wakelock_cb,
    &test_garden_create_thread_cb
};

void garden_cleanup()
{
    char *error;
    if(location_test_interface_ptr != NULL) {
        if(loc_lib_handle) {
            int ret = dlclose(loc_lib_handle);
            if (ret)
            {
                garden_print("Failed to unload library - %d", ret);
                if ((error = dlerror()) != NULL )
                {
                    garden_print("    dlerror - %s",error);
                }
            }
        }
        location_test_interface_ptr = NULL;
    }

    if(pXtraClientInterface != NULL) {
        if(xtra_lib_handle) {
            int ret = dlclose(xtra_lib_handle);
            if (ret)
            {
                garden_print("Failed to unload library - %d", ret);
                if ((error = dlerror()) != NULL )
                {
                    garden_print("    dlerror - %s",error);
                }
            }
        }
        pXtraClientInterface = NULL;
    }
}

void garden_print(const char *fmt, ...)
{
    va_list ap;
    char buf[1024];
    va_start(ap, fmt);
    vsnprintf(buf, 1024, fmt, ap);
    va_end(ap);
    fprintf(stderr,"GARDEN: %s\n",buf);
}

typedef void (*ThreadStart) (void *);
struct tcreatorData {
    ThreadStart pfnThreadStart;
    void* arg;
};

void *my_thread_fn (void *tcd)
{
    struct tcreatorData* local_tcd = (struct tcreatorData*)tcd;
    if (NULL != local_tcd) {
        local_tcd->pfnThreadStart (local_tcd->arg);
        free(local_tcd);
    }
    return NULL;
}

// location-xtra library calls this function back when data is ready
void test_xtra_client_data_callback(char * data,int length) {

    g_xtra_data_file = fopen(XTRA_DATA_FILE_NAME,"w");

    if(g_xtra_data_file != NULL)
    {
        fwrite(data,1,length,g_xtra_data_file);
        // close file
        fclose(g_xtra_data_file);
    }
	garden_print("## test_xtra_client_data_callback data_length=%d ##", length);
	location_test_interface_ptr->location_xtra_inject_data(data, length);

}

// location-xtra library calls this function back when time is ready
void test_xtra_client_time_callback(int64_t utcTime, int64_t timeReference, int uncertainty) {
	garden_print("## test_xtra_client_time_callback ##");
	location_test_interface_ptr->location_inject_time(utcTime, timeReference, uncertainty);
}

/*timestamp tranfer*/
void transformToDate(int64_t timeStamp ,uint8 *dateArry)
{  
    int64_t low ,high ,mid ,t;  
    int64_t year ,month ,day ,hour ,minute ,second ,milliSecond;  
    int64_t daySum[] = {0 ,31 ,59 ,90 ,120 ,151 ,181 ,212 ,243 ,273 ,304 ,334 ,365};  
    int64_t milOfDay = 24 * 3600 * 1000;  
    int64_t milOfHour = 3600 * 1000;  
   
    if(timeStamp > 315537897599999) {  
        timeStamp = 315537897599999;  
    }  
   
    low = 1;  
    high = 9999;  
   
    while(low <= high)  
    {  
        mid = (low+high)>>1;  
        t = ((mid-1) * 365 + (mid-1)/4 - (mid-1)/100 + (mid-1)/400) * milOfDay;  
   
        if(t == timeStamp)  
        {  
            low = mid + 1;  
            break;  
        }  
        else if(t < timeStamp)  
            low = mid + 1;  
        else  
            high = mid - 1;  
    }  
    year = low-1;  
    timeStamp -= ((year-1) * 365 + (year-1)/4 - (year-1)/100 + (year-1)/400) * milOfDay;  
   
    int isLeapYear = ((year&3) == 0 && year%100!=0) || year%400 == 0;  
   
    for(month = 1 ;(daySum[month] + ((isLeapYear && month > 1) ? 1 : 0)) * milOfDay <= timeStamp && month < 13 ;month ++) {  
        if(isLeapYear && month > 1)  
            ++daySum[month];  
    }  
    timeStamp -= daySum[month-1] * milOfDay;  
   
    day = timeStamp / milOfDay;  
    timeStamp -= day * milOfDay;  
   
    hour = timeStamp / milOfHour;  
    timeStamp -= hour * milOfHour;  
   
    minute = timeStamp / 60000;  
    timeStamp -= minute * 60000;  
   
    second = timeStamp / 1000;  
    milliSecond = timeStamp % 1000;  
   
    dateArry[0] = (uint8)(year+1970-1-2000);  
    dateArry[1] = (uint8)month;  
    dateArry[2] = (uint8)day+1;  
    dateArry[3] = (uint8)hour;  
    dateArry[4] = (uint8)minute;  
    dateArry[5] = (uint8)second;  
}

void test_garden_location_cb (GpsLocation * location) {
	GpsInfo gps_info;

	transformToDate(location->timestamp, gps_info.time);
	gps_info.accuracy = location->accuracy;
	gps_info.bearing = location->bearing;
	gps_info.speed = location->speed;
	gps_info.altitude = location->altitude;
	gps_info.longitude = location->longitude;
	gps_info.latitude = location->latitude;
	test_gps_ind_cb(SIMCOM_EVENT_LOC_IND, (void*)&gps_info);
    //garden_print("\nLATITUDE:  %f\nLONGITUDE: %f\nALTITUDE:  %f\nSPEED:     %f\nTIME:      %s",
    //             location->latitude, location->longitude, location->altitude,location->speed, time);
}


void test_garden_status_cb (GpsStatus * status) {
    if(status->status == GPS_STATUS_ENGINE_ON)
    {
    	garden_print("## gps engine on ##");
    }

    if(status->status == GPS_STATUS_ENGINE_OFF)
    {
    	garden_print("## gps engine off  ##");
    }
}

void test_garden_sv_status_cb (GpsSvStatus * sv_info)
{

}

void test_garden_nmea_cb (GpsUtcTime timestamp, const char *nmea, int length)
{
	char tmp[300] = {0};
    if(nmea != NULL) {
		memcpy(tmp, nmea, length);
		test_gps_ind_cb(SIMCOM_EVENT_NMEA_IND, (void *)tmp);
		//garden_print(tmp);
	}
}

void test_garden_set_capabilities_cb (uint32_t capabilities) {
}

void test_garden_acquire_wakelock_cb () {
}

void test_garden_release_wakelock_cb () {
}

pthread_t test_garden_create_thread_cb (const char *name, void (*start) (void *),
                                void *arg)
{
    garden_print("## gps_create_thread ##:");
    pthread_t thread_id = -1;
    garden_print ("%s", name);

    struct tcreatorData* tcd = (struct tcreatorData*)malloc(sizeof(*tcd));

    if (NULL != tcd) {
        tcd->pfnThreadStart = start;
        tcd->arg = arg;

        if (0 > pthread_create (&thread_id, NULL, my_thread_fn, (void*)tcd)) {
            garden_print ("error creating thread");
            free(tcd);
        } else {
            garden_print ("created thread");
        }
    }
    return thread_id;
}

void test_garden_xtra_download_req_cb () {
    if(is_xtra_use == FALSE) {
        return;
    }
    garden_print ("## gps_xtra_download_request ##:");
    // Pass on the data request to Xtra Client
    if (pXtraClientInterface != NULL) {
	    pXtraClientInterface->onDataRequest();
    }
}

void test_garden_xtra_time_req_cb () {

    garden_print ("## gps_request_utc_time ##:");
    // Pass on the time request to Xtra Client
    if (pXtraClientInterface != NULL) {
	    pXtraClientInterface->onTimeRequest();
    }
}

void test_garden_xtra_report_server_cb(const char* server1, const char* server2, const char* server3) {
    garden_print ("## report_xtra_server_callback ##:");
    //XtraClientInterfaceType const * pXtraClientInterface = get_xtra_client_interface();
    if (pXtraClientInterface != NULL)
    {
        pXtraClientInterface->onXTRAServerReported(g_xtra_client_config.xtra_server_url[0], g_xtra_client_config.xtra_server_url[1], g_xtra_client_config.xtra_server_url[2]);
    }
}

void test_agps_status_cb (AGpsExtStatus * status) {
}

void test_garden_ni_notify_cb(GpsNiNotification *notification) {
}

static boolean location_init()
{
	int rc = 0;
	char *error;
	char *lib_name;
	char *sym_name = (char *)"get_location_client_interface";
	char *print_msg;
	xtra_test_interface_func_t *xtra_create;
	loc_test_interface_func_t *loc_create;

	/*GPS*/
	if (location_test_interface_ptr != NULL) {
		garden_print("Location Already Init");
		return FALSE;
	}

    lib_name = (char *)"libloc_hal_test_shim_extended.so.1";
	
    loc_lib_handle = dlopen((const char *)lib_name, RTLD_NOW);
    if(!loc_lib_handle) {
        garden_print("Could not open Library - %s",dlerror());
        garden_cleanup();
        return FALSE;
    }

    loc_create = (loc_test_interface_func_t*)dlsym(loc_lib_handle,(const char *)sym_name);
    if( (error = dlerror()) != NULL) {
        garden_print("Could not retrieve symbol - %s",error);
        garden_cleanup();
        return FALSE;
    }

    if(loc_create != NULL) {
        location_test_interface_ptr = (*loc_create)();
    }

  if (location_test_interface_ptr != NULL)
  {
    rc = location_test_interface_ptr->location_init(&my_garden_cb);

    if(rc == -1) {
        garden_print(" Could not Initialize, Cannot proceed ");
        return FALSE;
    }
  }

    // Initialize XTRA client
	char *xtra_name = (char *)"get_xtra_client_interface";
	if (pXtraClientInterface != NULL) {
		garden_print("XTRA Already Init");
		return FALSE;
	}

    lib_name = (char *)"libloc_xtra.so.1";
	sym_name = (char *)"get_xtra_client_interface";
	
    xtra_lib_handle = dlopen((const char *)lib_name, RTLD_NOW);
    if(!xtra_lib_handle) {
        garden_print("Could not open Library - %s",dlerror());
        return FALSE;
    }

    xtra_create = (xtra_test_interface_func_t*)dlsym(xtra_lib_handle,(const char *)sym_name);
    if( (error = dlerror()) != NULL) {
        garden_print("Could not retrieve symbol - %s",error);
        garden_cleanup();
        return FALSE;
    }

    if(xtra_create != NULL) {
        pXtraClientInterface = (*xtra_create)();
    }

    if(pXtraClientInterface != NULL) {
        pXtraClientInterface->init(&myXtraClientDataCallback, &myXtraClientTimeCallback, &g_xtra_client_config);
    }
	return TRUE;

}

static boolean location_close()
{
	if (location_test_interface_ptr == NULL) {
		garden_print("Location Already Close");
		return FALSE;
	}
	location_test_interface_ptr->location_close();
    // Stop XTRA client
    //XtraClientInterfaceType const* pXtraClientInterface = get_xtra_client_interface();
    if(pXtraClientInterface != NULL) {
        pXtraClientInterface->stop();
    }
	garden_cleanup();
	return TRUE;
}

static boolean location_xtra_enable()
{
    int s_fd = -1;
    int count;
    char command[50];
    char readbuf[50];
	 int flag = 1;
    
	s_fd = open (S_DEVICE_PATH, O_RDWR);
	if(s_fd < 0)
	{
		printf("[%s] open device fail.\n", __func__);
		return -1;
	}

    /*enable xtra*/
	memset(command, 0, sizeof(command));
	sprintf(command, "AT+CGPSXE=%d\r\n", flag);
	count = write (s_fd, command, strlen(command));
	if(count != strlen(command))
	{
		printf("[%s] write fail.\n", __func__);
		return -1;
	}
    usleep(200*1000);
    memset(readbuf, 0, sizeof(readbuf));
    count = read(s_fd, readbuf,50);
    if (strstr(readbuf, "OK") == NULL) 
    {
        printf("AT+CGPSXE=%d set failure", flag);
        return -1;
    }
	close(s_fd);
	is_xtra_use = TRUE;
	return 0;	
}

static boolean location_xtra_disable()
{
    int s_fd = -1;
    int count;
    char command[50];
    char readbuf[50];
	 int flag = 0;
    
	s_fd = open (S_DEVICE_PATH, O_RDWR);
	if(s_fd < 0)
	{
		printf("[%s] open device fail.\n", __func__);
		return -1;
	}

    /*enable xtra*/
	memset(command, 0, sizeof(command));
	sprintf(command, "AT+CGPSXE=%d\r\n", flag);
	count = write (s_fd, command, strlen(command));
	if(count != strlen(command))
	{
		printf("[%s] write fail.\n", __func__);
		return -1;
	}
    usleep(200*1000);
    memset(readbuf, 0, sizeof(readbuf));
    count = read(s_fd, readbuf,50);
    if (strstr(readbuf, "OK") == NULL) 
    {
        printf("AT+CGPSXE=%d set failure", flag);
        return -1;
    }
	close(s_fd);
	is_xtra_use = FALSE;
	return 0;	
}

static boolean location_coldstart()
{
	if (location_test_interface_ptr == NULL) {
		garden_print("Please Init Location");
		return FALSE;
	}
	location_test_interface_ptr->location_set_position_mode(0, 0, 1000, 10000, 0);
	location_test_interface_ptr->location_delete_aiding_data(0xFFFFFFFF);
	location_test_interface_ptr->location_start_navigation();
	return TRUE;
}

static boolean location_hotstart()
{
	if (location_test_interface_ptr == NULL) {
		garden_print("Please Init Location");
		return FALSE;
	}
	location_test_interface_ptr->location_set_position_mode(0, 0, 1000, 10000, 0);
	location_test_interface_ptr->location_start_navigation();
	return TRUE;
}

static boolean location_stop()
{
	if (location_test_interface_ptr == NULL) {
		garden_print("Please Init Location");
		return FALSE;
	}
	location_test_interface_ptr->location_stop_navigation();
	return TRUE;
}

/*===============================================================================*/

boolean gps_init(gps_ind_cb_fcn gps_ind_cb)
{
	test_gps_ind_cb = gps_ind_cb;
	return location_init();
}

void gps_deinit()
{
	location_close();
}
boolean gps_xtra_enable()
{
	return location_xtra_enable();
}
boolean gps_xtra_disable()
{
	return location_xtra_disable();
}
boolean gps_coldstart()
{
	return location_coldstart();
}
boolean gps_hotstart()
{
	return location_hotstart();
}
boolean gps_stop()
{
	return location_stop();
}

/*=============================================================================
  FUNCTION test_main

    Android Framework simulation entry point.

    Includes individual tests for specific features.

    Search the makefile for "TEST_ANDROID_GPS_FLAGS" and uncomment the
    flags that correspond to desired tests.

=============================================================================*/
#if 0
void print_option_menu(const char* options_list[], int array_size)
{
    int i,j;
    for (i=0; i<(array_size/2); i++)
    {
        printf("%s",options_list[2*i]);
        for(j=30-strlen(options_list[2*i]); j>0; j--)
        {
            printf(" ");
        }
        printf("%s\n",options_list[2*i+1]);
    }

    if(array_size%2 != 0)
    {
        printf("%s\n",options_list[array_size -1]);
    }
}
int main ()
{

	char scan_string[LOC_MAX_FILE_PATH_LEN] = {0};
	uint8 type;
	int loop = 1;
	int opt;
	const char *options_list[] = {
	  "1. Loc Init",
	  "2. Loc DeInit",
	  "3. XTRA Enable",
	  "4. XTRA Disable",
	  "5. Cold Start",
	  "6. Hot Start",
	  "7. Close GPS",
	  "8. Exit"
	};

    while (loop)
	{
		/* Display menu of options. */
		printf("Please select an option to test from the items listed below.\n");
		print_option_menu(options_list, sizeof(options_list)/sizeof(options_list[0]));
		printf("Option > ");         

		/* Read the option from the standard input. */
		if (fgets(scan_string, sizeof(scan_string), stdin) == NULL)
		continue;
       opt = atoi(scan_string);
		switch (opt)
		{
			case 1:
			{
				if (location_init() == FALSE) {
					garden_print("Locatioin Init Failed");
				}
			}
			break;
			/*=======================================================================================*/
			case 2:
			{
				if (location_close() == FALSE) {
					garden_print("Locatioin DeInit Failed");
				}
			}
			break;
			/*=======================================================================================*/
			case 3:
			{
				location_xtra_enable();
			}
			break;
			/*=======================================================================================*/
			case 4:
			{
				location_xtra_disable();
			}
			break;
			/*=======================================================================================*/
			case 5:
			{
				if(location_coldstart() == FALSE) {
					garden_print("Location ColdStart Failed");
				}
			}
			break;
			/*=======================================================================================*/
			case 6:
			{
				if(location_hotstart() == FALSE) {
					garden_print("Location HoldStart Failed");
				}
			}
			break;
			/*=======================================================================================*/
			case 7:
			{
				if(location_stop() == FALSE) {
					garden_print("Location Stop Failed");
				}
			}
			break;
			/*==================================================================================================*/
			case 8:
			{
				loop = 0;
			}
			break;
			default:
			break;
		}
	}
}
#endif
