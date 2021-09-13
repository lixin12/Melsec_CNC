
CFLAGS = -c -O2 
	 
MAIN_DIR = .

TARGET_BIN_DIR = $(MAIN_DIR)/bin
TARGET_OBJ_DIR = $(MAIN_DIR)/obj

BIN_TIME  = $(shell echo date "%d_%B_%Y")
BIN_NAME  = zy_app_CNC

TEST_DIR = $(MAIN_DIR)
TESTC_DIR = $(MAIN_DIR)/usrc

SRC_DIR = $(TEST_DIR) 

INCLUDE_PREFIX = -I

LINK_PREFIX = -L

ALL_PATHS = $(SRC_DIR)
#ALL_PATHS += ./inc
ALL_PATHS += ./uinc
ALL_PATHS += ./sdk-includes/common
ALL_PATHS += ./sdk-includes/data
ALL_PATHS += ./sdk-includes/dsi
ALL_PATHS += ./sdk-includes/dsutils
ALL_PATHS += ./sdk-includes/gps
ALL_PATHS += ./sdk-includes/qmi
ALL_PATHS += ./sdk-includes/qmi-framework
ALL_PATHS += ./sdk-includes/linux/spi
ALL_PATHS += ./sdk-includes/mqtt
ALL_PATHS += ./sdk-includes/openssl
ALL_PATHS += ./sdk-includes/curl
ALL_PATHS += ./sdk-includes/json-c
ALL_PATHS += $(LIB_PATH)/usr/include

ALL_INCLUDES = $(addprefix $(INCLUDE_PREFIX), $(ALL_PATHS))


ALL_LINKS += $(addprefix $(LINK_PREFIX), ./libs)

OBJ_CMD = -o 

LD_CMD = -o

TEST_OBJS = $(TARGET_OBJ_DIR)/IOT_mqtt_pub.o \
			$(TARGET_OBJ_DIR)/IOT_ADC.o \
			$(TARGET_OBJ_DIR)/IOT_Data.o\
			$(TARGET_OBJ_DIR)/IOT_http_s.o \
			$(TARGET_OBJ_DIR)/IOT_PBoxInfo.o \
			$(TARGET_OBJ_DIR)/IOT_SIM7600CE.o \
			$(TARGET_OBJ_DIR)/IOT_NTP.o \
			$(TARGET_OBJ_DIR)/IOT_SecsBinaryCreate.o\
			$(TARGET_OBJ_DIR)/IOT_IPSet.o\
			$(TARGET_OBJ_DIR)/IOT_LED.o\
			$(TARGET_OBJ_DIR)/IOT_Timer.o\
			$(TARGET_OBJ_DIR)/IOT_LEDControl.o\
			$(TARGET_OBJ_DIR)/IOT_Uart.o\
			$(TARGET_OBJ_DIR)/IOT_APP.o\
			$(TARGET_OBJ_DIR)/log.o \
			$(TARGET_OBJ_DIR)/IOT_DomainNameToIP.o\
			$(TARGET_OBJ_DIR)/GPSControl.o\
			$(TARGET_OBJ_DIR)/IOT_GPS.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC_CELL.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC_Analyze.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC_Command.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC.o\
			$(TARGET_OBJ_DIR)/main.o 

LIB_OBJS =  $(TARGET_OBJ_DIR)/IOT_mqtt_pub.o \
			$(TARGET_OBJ_DIR)/IOT_ADC.o \
			$(TARGET_OBJ_DIR)/IOT_Data.o\
			$(TARGET_OBJ_DIR)/IOT_http_s.o \
			$(TARGET_OBJ_DIR)/IOT_PBoxInfo.o \
			$(TARGET_OBJ_DIR)/IOT_SIM7600CE.o \
			$(TARGET_OBJ_DIR)/IOT_NTP.o \
			$(TARGET_OBJ_DIR)/IOT_SecsBinaryCreate.o\
			$(TARGET_OBJ_DIR)/IOT_IPSet.o\
			$(TARGET_OBJ_DIR)/IOT_LED.o\
			$(TARGET_OBJ_DIR)/IOT_Timer.o\
			$(TARGET_OBJ_DIR)/IOT_LEDControl.o\
			$(TARGET_OBJ_DIR)/IOT_APP.o\
			$(TARGET_OBJ_DIR)/IOT_Uart.o\
			$(TARGET_OBJ_DIR)/IOT_DomainNameToIP.o\
			$(TARGET_OBJ_DIR)/GPSControl.o\
			$(TARGET_OBJ_DIR)/IOT_GPS.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC_Analyze.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC_Command.o\
			$(TARGET_OBJ_DIR)/Siemens_CNC.o\


STD_LIB= $(LIB_PATH)/usr/lib/libdsi_netctrl.so     \
         $(LIB_PATH)/usr/lib/libdsutils.so         \
         $(LIB_PATH)/usr/lib/libqmiservices.so     \
         $(LIB_PATH)/usr/lib/libqmi_cci.so         \
         $(LIB_PATH)/usr/lib/libqmi_common_so.so   \
         $(LIB_PATH)/usr/lib/libqmi.so             \
		 $(LIB_PATH)/lib/libpthread.so.0	 \
		 $(LIB_PATH)/usr/lib/libqdi.so		\
		 $(LIB_PATH)/usr/lib/librt.so		\
		 $(LIB_PATH)/usr/lib/libssl.so		\
		 $(LIB_PATH)/lib/libcrypto.so		\
		 $(LIB_PATH)/usr/lib/libdl.so		\
         $(LIB_PATH)/usr/lib/libloc_stub.so     \
	     $(LIB_PATH)/usr/lib/libcurl.so         \
	     $(LIB_PATH)/usr/lib/libmosquitto.so    \
	     $(LIB_PATH)/usr/lib/libjson-c.so     
	   


BIN_OBJS = $(TEST_OBJS) 

$(TARGET_OBJ_DIR)/%.o:$(TESTC_DIR)/%.c
	echo ---------------------------------------------------------
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------

$(TARGET_OBJ_DIR)/%.o:$(TESTC_DIR)/%.cpp
	#echo $(ALL_PATHS)
	#echo $(ALL_INCLUDES)
	echo Build OBJECT $(@) from SOURCE $<
	$(CC) $(CFLAGS) $(ALL_INCLUDES) $(OBJ_CMD) $@ $<
	echo ---------------------------------------------------------

.PHONY: all clean

all:prep bin 

prep:
	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi
	@if test ! -d $(TARGET_OBJ_DIR); then mkdir $(TARGET_OBJ_DIR); fi

bin:$(BIN_OBJS)
	@echo Create bin file $(BIN_NAME)
	@$(CC) -lm -L./libs $(ALL_LINKS) $(BUILD_LDFLAGS) $(STD_LIB) -o $(TARGET_BIN_DIR)/$(BIN_NAME) $^ -lsdk
	@echo ---------------------------------------------------------
	arm-oe-linux-gnueabi-ar -r libTCP.a $(LIB_OBJS)
clean:
	@rm -fr $(TARGET_OBJ_DIR) $(TARGET_BIN_DIR) 
	@rm -fr libTCP.a
	
