#ifndef _I2C_H_
#define _I2C_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>



	int sim_i2c_write(uint8_t slave_address, uint16_t reg, uint8_t *buf, int len);
	int sim_i2c_read(uint8_t slave_address, uint16_t reg, uint8_t *buf, int len);



#endif
