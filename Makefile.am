DIR_SRC = ./src

AM_CFLAGS = -Wall \
    -Wundef \
    -Wno-trigraphs \
    -g -O0 \
    -Dstrlcpy=g_strlcpy \
    -Dstrlcat=g_strlcat \
    $(QMIFRAMEWORK_CFLAGS) \
    $(QMI_CFLAGS) \
    -DFEATURE_DSS_LINUX \
    -I$(WORKSPACE)/diag/include \
    -I$(WORKSPACE)/common/inc \
    -I./inc \
    -I./sdk-includes/ \
    -I./sdk-includes/qmi \
    -I./sdk-includes/qmi-framework \
    -I./sdk-includes/dsutils \
    -I./sdk-includes/dsi \
    -I./sdk-includes/data \
    -I./sdk-includes/common \
    -I./sdk-includes/gps \
    -I./sdk-includes/json-c \
    -I./sdk-includes/mqtt \
    -I./sdk-includes/curl \
    -I./sdk-includes/openssl
    

AM_CXXFLAGS = \
    $(AM_CFLAGS) \
    -fpermissive

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_SINGLE_LINK \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_DSM_DUP_ITEMS \
        -DFEATURE_LE_DIAG \
        -DFEATURE_MULTIPROCESSOR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
        -I./inc \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
        -DBUILD_VERSION_WIFI_RTL

SRCS = \
      $(DIR_SRC)/main.c \
      $(DIR_SRC)/OTAUpgrade.c   \
      $(DIR_SRC)/BTControl.c  \
      $(DIR_SRC)/LedControl.c  \
      $(DIR_SRC)/GpioControl.c  \
      $(DIR_SRC)/uart_debug.c \
      $(DIR_SRC)/GPSControl.c \
      $(DIR_SRC)/EthControl.c \
      $(DIR_SRC)/FTPControl.c \
      $(DIR_SRC)/LinuxTimer.c \
      $(DIR_SRC)/uartapp.c \
      $(DIR_SRC)/spidev_test.c \
      $(DIR_SRC)/TCPDemo.c \
      $(DIR_SRC)/ADC.c \
      $(DIR_SRC)/I2C.c \
      $(DIR_SRC)/demoVersion.c \
      $(DIR_SRC)/json-c.c \
      $(DIR_SRC)/http_s.c \
      $(DIR_SRC)/mqtt_s.c \
      $(DIR_SRC)/ftp_s.c \
      $(DIR_SRC)/openssl.c \
      $(DIR_SRC)/simcom_lbs_test.c \
      $(DIR_SRC)/simcom_common.c \
      $(DIR_SRC)/simcom_test_main.c

bin_PROGRAMS = simcom_demo
simcom_demo_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
simcom_demo_SOURCES = $(SRCS)

simcom_demo_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) -fPIC  @GLIB_CFLAGS@
simcom_demo_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLIB  @GLIB_CFLAGS@
simcom_demo_LDFLAGS = -lpthread -lrt  -ldl $(QMIFRAMEWORK_LIBS) $(QMI_LIBS) $(DSUTILS_LIBS)
simcom_demo_LDFLAGS += -ljson-c
simcom_demo_LDFLAGS += -lcurl
simcom_demo_LDFLAGS += -lmosquitto
simcom_demo_LDFLAGS += -ldsi_netctrl

simcom_demo_LDADD = libs/libsdk.a
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/oe-core/meta-qcom/recipes/mcm-core/prebuilt/usr/lib/libmcm.so.0
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/oe-core/meta-qcom/recipes/data/prebuilt/data/usr/lib/libdsi_netctrl.so
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/oe-core/meta-qcom/recipes/gps/prebuilt/loc-xtra/usr/lib/libloc_xtra.so
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/oe-core/build/tmp-glibc/work/armv7a-vfp-neon-oe-linux-gnueabi/loc-stub/git-r1/image/usr/lib/libloc_stub.so
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/simcom-daemons/simcom-demo/libs/libcurl.so.5
#simcom_demo_LDFLAGS += -l$(WORKSPACE)/simcom-daemons/simcom-demo/libs/libmosquitto.so.1


