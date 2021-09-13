#include "I2C.h"

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(*a))



//read write interface
static int do_rdwr(int fd, struct i2c_msg *msgs, int nmsgs)
{
	struct i2c_rdwr_ioctl_data msgset = {
		.msgs = msgs,
		.nmsgs = nmsgs,
	};

	if (msgs == NULL || nmsgs <= 0)
		return -1;

	if (ioctl(fd, I2C_RDWR, &msgset) < 0)
		return -1;

	return 0;
}

/*****************************************************************************
* Function Name : sim_i2c_write
* Description   : write the slave device register
* Input			: uint8 slave_address
 			      uint16_t reg
 			      uint8_t *buf
 			      int len

* Output        : None
* Return        : 0:success, -1:failed
* Auther        : simcom
* Date          : 2018.04.24
*****************************************************************************/
int sim_i2c_write(uint8_t slave_address, uint16_t reg, uint8_t *buf, int len)
{
	uint8_t *data;
	uint8_t reg_len;
	int fd;
	fd = open("/dev/i2c-5",O_RDWR);
	struct i2c_msg msgs[1];
	if(fd < 0)  
    {  
        printf("open i2c device error\n");  
		return -1;
    } 
	if(reg > 0xFF)
		reg_len = 2;
	else
		reg_len = 1;
	data = (uint8_t *)malloc((reg_len + len) * sizeof(*data));
	#if 0  // c
	struct i2c_msg msgs[] = {
		[0] = {
			.addr = (slave_address >> 1),
			.flags = 0,
			.buf = (uint8_t *)data,
			.len = reg_len + len,
		},
	};
	#else //c++
	msgs[0].addr = (slave_address >> 1);
	msgs[0].flags = 0,
	msgs[0].buf = (uint8_t *)data;
	msgs[0].len = reg_len + len;
	#endif
	if (!data) {
		len = -1;
		goto err_malloc;
	}
  	if(reg_len == 2)
  	{
		data[0] = reg >> 8;
		data[1] = reg & 0xFF;
		memcpy(data + 2, buf, len);
	}else
	{
		data[0] = reg;
		memcpy(data + 1, buf, len);
	}

	if (do_rdwr(fd, msgs, ARRAY_SIZE(msgs))) {
		printf("sim_i2c_write error\n"); 
		len = -1;
		goto err_do_rdwr;
	}
err_do_rdwr:
	free(data);
err_malloc:
	close(fd);
	return len;
}
/*****************************************************************************
* Function Name : sim_i2c_read
* Description   : read the slave device register
* Input			: uint8 slave_address
 			      uint16_t reg
 			      uint8_t *buf
 			      int len

* Output        : None
* Return        : 0:success, -1:failed
* Auther        : simcom
* Date          : 2018.04.24
*****************************************************************************/
int sim_i2c_read(uint8_t slave_address, uint16_t reg, uint8_t *buf, int len)
{
	int fd;
	uint8_t reg_len;
	uint8_t reg_data[2];
	int index;
	struct i2c_msg msgs[2];
	fd = open("/dev/i2c-5",O_RDWR);
	if(fd < 0)  
    {  
        printf("open i2c device error\n");  
		return -1;
    } 
	if(reg > 0xFF)
	{
		reg_len = 2;
		reg_data[0] = reg >> 8;
		reg_data[1] = reg & 0xFF;
	}
	else
	{
		reg_len = 1;
		reg_data[0] = reg;
	}
	#if 0 //c
	struct i2c_msg msgs[] = {
		[0] = {
			.addr = slave_address >> 1,
			.flags = 0,
			.buf = (void *)reg_data,
			.len = reg_len,
		},
		[1] = {
			.addr = slave_address >> 1,
			.flags = I2C_M_RD,
			.buf = (void *)buf,
			.len = len,
		},
	};
	#else //c++
	msgs[0].addr = slave_address >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = (uint8_t *)reg_data;
	msgs[0].len = reg_len;
	msgs[1].addr = slave_address >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = (uint8_t *)buf;
	msgs[1].len = len;
	#endif
	if (do_rdwr(fd, msgs, ARRAY_SIZE(msgs))) {
		printf("sim_i2c_read error\n"); 
		return -1;
	}
	return 0;
}

