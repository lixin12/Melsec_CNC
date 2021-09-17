/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#ifdef _SIMCOM_WINDOWS_
#include <spidev.h>
#else
#include <linux/spi/spidev.h>
#endif
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define DEFAULT_DEVICE    "/dev/spidev1.0"
#define DEFAULT_I2C_ADDR  0
#define DEFAULT_BAUDRATE  115200
#define DEFAULT_FOSC  14745600 /* 14.7456 MHz */
#define VREG_STATE_PATH \ 
        "/sys/devices/soc:qcom,rpm-smd/soc:qcom,rpm-smd:rpm-regulator-ldoa5/soc:qcom,rpm-smd:rpm-regulator-ldoa5:regulator-l5/regulator/regulator.13/state"
#define VREG_VOL_PATH \ 
        "/sys/devices/soc:qcom,rpm-smd/soc:qcom,rpm-smd:rpm-regulator-ldoa5/soc:qcom,rpm-smd:rpm-regulator-ldoa5:regulator-l5/regulator/regulator.13/microvolts"

unsigned char busnum = 1;

static inline unsigned char REG_NUM(int ch, int a) {
	return (a&0x0f)<<3 | (ch&0x03)<<1;
}

static inline unsigned char FLIP(unsigned char b) {
	b = ( b       << 4) | ( b       >> 4); /* 0000xxxx <<4 xxxx0000 >>4 */
	b = ((b&0x33) << 2) | ((b&0xcc) >> 2); /* 00xx00xx <<2 xx00xx00 >>2 */
	b = ((b&0x55) << 1) | ((b&0xaa) >> 1); /* 0x0x0x0x <<1 x0x0x0x0 >>1 */
	return b;
}

enum UART_REG_NAMES {
	RHR       = 0x00, /* read */
	THR       = 0x00, /* write */
	IER       = 0x01,
	FCR       = 0x02,
	IIR       = 0x02,
	LCR       = 0x03,
	MCR       = 0x04,
	LSR       = 0x05,
	MSR       = 0x06,
	SPR       = 0x07,
	TCR       = 0x06, /* MCR[2]=1 EFR[4]=1 */
	TLR       = 0x07, /* "" */
	TXLVL     = 0x08,
	RXLVL     = 0x09,
	IODIR     = 0x0A,
	IOSTATE   = 0x0B,
	IOINTENA  = 0x0C,
	IOCONTROL = 0x0E,
	EFCR      = 0x0F,
	DLL       = 0x00, /* LCR[7]=1 & LCR != 0xBF: Special reg. */
	DLH       = 0x01, /* "" */
	EFR       = 0x02, /* LCR = 0xBF : Enhanced Register */
	XON1      = 0x04, /* "" */
	XON2      = 0x05, /* "" */
	XOFF1     = 0x06, /* "" */
	XOFF2     = 0x07, /* "" */
	END_MARK  = 0xffff
};

struct sc16is7xx {
	int spi_i2c_fd;
	int i2c_addr;
	unsigned int flags;
	unsigned long fosc; /* in Hz */
	unsigned int xfers;
	unsigned int rxcounter;
	unsigned int txcounter;
};

/* register read/write combined for SPI */
static int
sc16is7xx_reg_xfer_spi(struct sc16is7xx *p,
                     unsigned char regnum, unsigned char txval, unsigned char *rxval)
{
	unsigned char txbuf[2], rxbuf[2];
	struct spi_ioc_transfer xfer;

	txbuf[0] = regnum;
	txbuf[1] = txval;
	rxbuf[0] = rxbuf[1] = '\0';

	bzero(&xfer, sizeof(xfer));
	xfer.rx_buf = (__u64)(unsigned long)rxbuf;
	xfer.tx_buf = (__u64)(unsigned long)txbuf;
	xfer.len    = sizeof(txbuf);
//	xfer.cs_change = 1; /* deselect device after xfer */

	/* one single transfer */
	if (ioctl(p->spi_i2c_fd,SPI_IOC_MESSAGE(1),&xfer) == -1) {
		perror("ioctl(SPI_IOC_MESSAGE)");
		fprintf(stderr,"spi xfer error: %02x %02x --> %02x %02x\n",
			txbuf[0], txbuf[1], rxbuf[0], rxbuf[1]);
		return -1;
	}

	p->xfers++;

	if (rxval)
		*rxval = rxbuf[1];

	return 0;
}

static int
sc16is7xx_reg_xfer_spi_fifo(
	struct sc16is7xx *p,
	unsigned char regnum,
	unsigned char *txbuf,
	unsigned char *rxbuf,
	int length)
{
	struct spi_ioc_transfer xfer[2];

	bzero(&xfer, sizeof(xfer));

	xfer[0].rx_buf = 0;
	xfer[0].tx_buf = (__u64)(unsigned long)&regnum;
	xfer[0].len    = 1;

	xfer[1].rx_buf = (__u64)(unsigned long)rxbuf;
	xfer[1].tx_buf = (__u64)(unsigned long)txbuf;;
	xfer[1].len    = length;
	xfer[1].cs_change = 1; /* deselect chip */

	/* one single transfer */
	if (ioctl(p->spi_i2c_fd,SPI_IOC_MESSAGE(2),&xfer) == -1) {
		perror("ioctl(SPI_IOC_MESSAGE)");
		return -1;
	}

	p->xfers++;
	return 0;

}

static int
sc16is7xx_reg_rd_i2c(struct sc16is7xx *p, unsigned char regnum, unsigned char *val)
{
	return 0;
}

static int
sc16is7xx_reg_wr_i2c(struct sc16is7xx *p,
                     unsigned char regnum, unsigned char val)
{
	return 0;
}

static int
sc16is7xx_reg_wr_raw(struct sc16is7xx *p,
                     unsigned char regnum, unsigned char val)
{
	if (p->i2c_addr == 0) {/* spi */
		regnum &= ~0x81; /* clear R/#W bit, undef bit */
		return sc16is7xx_reg_xfer_spi(p, regnum, val, NULL);
	}
	return sc16is7xx_reg_wr_i2c(p, regnum & 0x7f, val);
}

static int
sc16is7xx_reg_rd_raw(struct sc16is7xx *p,
                     unsigned char regnum, unsigned char *val)
{
	if (p->i2c_addr == 0) {
		regnum |=  0x80; /* set R/#W bit */
		regnum &= ~0x01; /* clear undef bit */
		return sc16is7xx_reg_xfer_spi(p, regnum, 0, val);
	}
	return sc16is7xx_reg_rd_i2c(p, regnum & 0x7f, val);
}

static int
sc16is7xx_reg_wr(struct sc16is7xx *p, int reg, unsigned char val)
{
	return sc16is7xx_reg_wr_raw(p, REG_NUM(0,reg), val);
}

static int
sc16is7xx_reg_rd(struct sc16is7xx *p, int reg, unsigned char *val)
{
	return sc16is7xx_reg_rd_raw(p, REG_NUM(0,reg), val);
}

int
sc16is7xx_set_baud(struct sc16is7xx *p, unsigned int baud)
{
	unsigned char tmp_lcr;
	unsigned long divisor = p->fosc / (16 * baud);

	printf("Setting divisor to 0x%lx (baud=%u, osc=%lu)\n",
		divisor, baud, p->fosc);

	/* enable access to divisor registers */
	if (sc16is7xx_reg_rd(p, LCR, &tmp_lcr))
		return -1;
	if (sc16is7xx_reg_wr(p, LCR, 0x80))
		return -1;

	/* write divisor */
	if (sc16is7xx_reg_wr(p, DLL, divisor & 0xff))
		return -1;
	if (sc16is7xx_reg_wr(p, DLH, (divisor >> 8) & 0xff))
		return -1;

	/* hide divisor latch registers again */
	if (sc16is7xx_reg_wr(p, LCR, tmp_lcr & ~0x80))
		return -1;
	return 0;
}

ssize_t
sc16is7xx_read(struct sc16is7xx *p, void *buf, size_t length) {
	unsigned char rxlvl, lsr;
	ssize_t ret=0;

	if (length == 0) {
		fprintf(stderr,"Length == 0???\n");
		return 0;
	}

	if (sc16is7xx_reg_rd(p, LSR, &lsr))
		return -1;

	if (!(lsr & 0x01)) { /* no data in receiver */
		return 0;
	}

	if (lsr & 0x02) {
		fprintf(stderr,"\nOverflow! Reset FIFO.\n");
		/* reset read fifo */
		if (sc16is7xx_reg_wr(p, FCR, 0x01))
			return -1;
		return 0;
	}

	if (sc16is7xx_reg_rd(p, RXLVL, &rxlvl))
		return -1;

	if (rxlvl == 0) {
		fprintf(stderr,"RXLVL == 0, even though LSR[0] set!\n");
		return 0;
	}

	if (length > rxlvl)
		length = rxlvl;
	ret = length;

	if (p->i2c_addr == 0) {/* SPI */
		if (sc16is7xx_reg_xfer_spi_fifo(p,REG_NUM(0, RHR) | 0x80,
		       NULL/*tx*/, buf/*rx*/, length) == -1)
			return -1;
		p->rxcounter += length;
		return ret;
	}

	/* I2C */
	while (length) {
		if (sc16is7xx_reg_rd(p, RHR, (unsigned char*)buf))
			return -1;
		buf++;
		length--;
	}

	p->rxcounter += ret;

	return ret;
}

ssize_t
sc16is7xx_write(struct sc16is7xx *p, const void *buf, size_t length) {
	unsigned char txlvl;
	ssize_t ret=0;

	if (sc16is7xx_reg_rd(p, TXLVL, &txlvl))
		return -1;

	if (txlvl == 0)
		return 0;

	if (length > txlvl)
		length = txlvl;
	ret = length;

	while (length) {
		if (sc16is7xx_reg_wr(p, THR, *(const unsigned char*)buf))
			return -1;
		buf++;
		length--;
	}

	p->txcounter += ret;

	return ret;
}

ssize_t
sc16is7xx_get_rxlvl(struct sc16is7xx *p) {
	unsigned char rxlvl;
	if (sc16is7xx_reg_rd(p, RXLVL, &rxlvl))
		return -1;
	return rxlvl;
}

ssize_t
sc16is7xx_get_txlvl(struct sc16is7xx *p) {
	unsigned char txlvl;
	if (sc16is7xx_reg_rd(p, TXLVL, &txlvl))
		return -1;
	return txlvl;
}

void
sc16is7xx_stats(struct sc16is7xx *p,
	unsigned int *xfers,
	unsigned int *txcounter,
	unsigned int *rxcounter
){
	if (xfers)
		*xfers = p->xfers;
	if (txcounter)
		*txcounter = p->txcounter;
	if (rxcounter)
		*rxcounter = p->rxcounter;
}

struct sc16is7xx_initdata {
	unsigned int regnum;
	unsigned char val;
};

static const struct sc16is7xx_initdata sc16is7xx_initdata[] = {
	{ LCR,      0x03 }, /* 8 N 1 */
	{ FCR,      0x07 }, /* enable Fifo */
	{ SPR,      0x42 }, /* random data to scratch register */
	{ IODIR,    0x01 },
	{ IOSTATE,  0x0c },
	{ END_MARK, 0x00 }
};

int
sc16is7xx_gpio_out(struct sc16is7xx *p,
	unsigned char iodir,
	unsigned char outval
) {
	if (sc16is7xx_reg_wr(p, IODIR, iodir))
		return -1;
	if (sc16is7xx_reg_wr(p, IOSTATE, outval))
		return -1;
	return 0;
}

int
sc16is7xx_gpio_in(struct sc16is7xx *p, unsigned char *inval) {
	if (sc16is7xx_reg_rd(p, IOSTATE, inval))
		return -1;
	return 0;
}


struct sc16is7xx *
sc16is7xx_new(int spi_i2c_fd, int i2c_addr, unsigned int flags)
{
	struct sc16is7xx *p;
	const struct sc16is7xx_initdata *idt;
	unsigned char val;
	__u32 mode32;
	char testbuff[12]={0xA5,0xC7,0xb6,0xd2,0x67,0x33,0x57,0x89,0x10,0x44,0x11};
	int i = 0,j;

	p = calloc(1, sizeof *p);

	p->spi_i2c_fd = spi_i2c_fd;
	p->i2c_addr = i2c_addr;
	p->flags = flags;
	p->fosc = DEFAULT_FOSC;

	if (p->i2c_addr == 0) { /* spi */
		mode32 = SPI_MODE_0;

		if(ioctl(p->spi_i2c_fd,SPI_IOC_WR_MODE32,&mode32)){
			perror("ioctl(SPI_IOC_WR_MODE)");
			goto err_out;
		}
	}
	
	while (i < 12) {
		sc16is7xx_reg_wr(p, SPR, testbuff[i]);
		poll(0,0,300);
		sc16is7xx_reg_rd(p, SPR, &val);
		fprintf(stderr,"Value in SPR register [%d]read 0x%02x == 0x%02x?\n",i,testbuff[i],val);
		if(testbuff[i] == val){
			i++;
		}else{
			i=0;
		}
	}

	for (idt = sc16is7xx_initdata; idt->regnum != END_MARK; idt++) {
		printf("Init: reg %d = %d (0x%02x)\n",idt->regnum,
			idt->val, idt->val);
		if (sc16is7xx_reg_wr(p, idt->regnum, idt->val) == -1) {
			perror("Cannot write init data");
			goto err_out;
		}
	}

	for (i=0; i<16; i++) {
		j = sc16is7xx_reg_rd(p,i, &val);
		if (j != 0) {
			fprintf(stderr,"reg_rd returned %d\n", j);
			perror("ioctl()");
			continue;
		}
		printf("Reg %d -> %u (0x%02x)\n", i, val, val);
	}


	if (sc16is7xx_reg_rd(p, SPR, &val)) {
		perror("sc16is7xx_read(SPR)");
		goto err_out;
	}


	if (sc16is7xx_set_baud(p, 115200)) {
		perror("sc16is7xx_set_baud");
		goto err_out;
	}


	return p;

err_out:
	free(p);
	return NULL;
}


static void pabort(const char *s)
{
	perror(s);
	abort();
}

int spi_master_power(unsigned char control)
{
 	int fd;
	char vpath[200] = {0};
	sprintf(vpath,"/sys/class/spi_master/spi%d/device/power/control",busnum);
	fd = open(vpath, O_RDWR);
	if (fd < 0){
		pabort("can't open device");
		return fd;
	}
	if(control){
		write(fd,"on",3);
	}else{
		write(fd,"auto",5);
	}
	close(fd);
}
int spi_try_dev_mount(char* devpath)
{
	unsigned char chipselect;
	if(0 == strncmp("/dev/spidev",devpath,11)){
		busnum = devpath[11] - '0';
		if(busnum > 5)
			return -1;
		if(devpath[12] != '.')
			return -1;
		chipselect = devpath[13] - '0';
		if(chipselect > 9)
			return -1;
	}

	if(0 != access(devpath,F_OK)){
		char mountcmd[128] = {0};
		sprintf(mountcmd,"insmod /usr/lib/modules/3.18.20/kernel/drivers/spi/spidev.ko busnum=%d chipselect=%d maxspeed=960000 spimode=0",busnum,chipselect);
		printf("%s\n", mountcmd);
		system(mountcmd);
		usleep(50000);
	}

	if(0 != access(devpath,F_OK))
		return -1;

	return 0;
}
void spidev_power_init(void)
{
	int writecount;
	int fd = -1;
	fd = open(VREG_VOL_PATH,O_WRONLY);
	if(fd < 0)
	{
		printf("power_int open path file error!!!!");
		return;
	}else
	{
		printf("power_int fd=%d\n");
	}

	writecount = write(fd,(unsigned char *)"2350000",8);
	if(writecount <=0 )
	{
		printf("power_int set-v write error writecount = %d\n");
	}else
	{
		printf("power_int set-v write ok\n");
	}
	close(fd);

	fd = open(VREG_STATE_PATH,O_WRONLY);
	if(fd < 0)
	{
		return;
	}else
	{
		fprintf(stderr,"power_int fd=%d\n");
	}
	writecount = write(fd,"1",1);
	if(writecount <=0 )
	{
		fprintf(stderr,"power_int write error writecount = %d\n");
	}else
	{
		fprintf(stderr,"power_int write ok\n");
	}
	close(fd);
}

int spi_main(void)
{
	int spi_i2c_fd;
	char *spi_i2c_devname = DEFAULT_DEVICE;
	struct sc16is7xx *sc16is7xx;
	ssize_t txlvl, rxlvl;
	unsigned char buf[128];
	int i2c_addr = DEFAULT_I2C_ADDR;
	int ptyfd, nfds, i, j;
	fd_set readfds, writefds;
	struct timeval to;
	int baudrate = DEFAULT_BAUDRATE;
	unsigned int xfers, txcounter, rxcounter;
	int gpio_out = -1;
	char scan_string[256] = {0};

//	parse_opts(argc, argv);
	printf("\nPlease enter device (/dev/spidev1.0):");
	fgets(scan_string, sizeof(scan_string), stdin);
	if(scan_string[0] != '\n'){
		char* strip_s = &scan_string[0];
		while((*strip_s != '\r') && (*strip_s != '\n')) strip_s ++;
		*strip_s = '\0';
		spi_i2c_devname = scan_string;
	}

	if(0 != spi_try_dev_mount((char* )spi_i2c_devname)){
		printf("try get spidev %s error\n",spi_i2c_devname);
		return -1;
	}
	spidev_power_init();
	spi_master_power(1);
	usleep(5000);

	if ((spi_i2c_fd = open(spi_i2c_devname, O_RDWR|O_NOCTTY)) == -1) {
		perror(spi_i2c_devname);
		return -1;
	}

	sc16is7xx = sc16is7xx_new(spi_i2c_fd, i2c_addr, 0/*flags*/);
	if (!sc16is7xx) {
		fprintf(stderr, "Could not create sc16is7xx instance.\n");
		goto _out0;
	}

	if (sc16is7xx_set_baud(sc16is7xx, baudrate) == -1) {
		perror("sc16is7xx_set_baud()");
		goto _out1;
	}

	if (gpio_out != -1) {
		fprintf(stderr, "setting GPIO to 0x%04x\n", gpio_out & 0xff);
		for (i=7; i>=0; i--)
			fprintf(stderr," D%d=%d", i, !!(gpio_out & (1<<i)));
		fprintf(stderr,"\n");
		if (sc16is7xx_gpio_out(sc16is7xx, 0xff, gpio_out & 0xff) == -1) {
			perror("sc16is7xx_gpio_out");
			goto _out1;
		}
	}

	fprintf(stderr,"Baudrate is %d.\n\n", baudrate);
	ptyfd = STDIN_FILENO;
	/* main loop */
	while (1) {
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		nfds=0;

		/* are we able to write? */
		txlvl = sc16is7xx_get_txlvl(sc16is7xx);
		if (txlvl == -1) {
			perror("sc16is7xx_get_txlvl()");
			goto _out1;
		}

		if (txlvl > 0) {
			if (txlvl > (signed int)sizeof(buf))
				txlvl = (signed int)sizeof(buf);
			/* so register for reading on pty*/
			FD_SET(ptyfd, &readfds);
			if (nfds < ptyfd + 1)
				nfds = ptyfd + 1;
		}

		/* is there something in the receive buffer? */
		rxlvl = sc16is7xx_get_rxlvl(sc16is7xx);
		if (rxlvl == -1) {
			perror("sc16is7xx_get_rxlvl()");
			goto _out1;
		}

		if (rxlvl > 0) {
			if (rxlvl > (signed int)sizeof(buf))
				rxlvl = (signed int)sizeof(buf);
			/* so register for writing to the pty */
			FD_SET(ptyfd, &writefds);
			if (nfds < ptyfd + 1)
				nfds = ptyfd + 1;
		}

		to.tv_sec = 0;
		to.tv_usec = (rxlvl > 0) ? 0 : 500000; /* 1ms */

		i = select(nfds, &readfds, &writefds, NULL, &to);
		if (i == -1) {
			perror("select()");
			goto _out1;
		}
/*
		sc16is7xx_stats(sc16is7xx, &xfers, &txcounter, &rxcounter);
		fprintf(stderr,"\r%u %u %u %4d %4d>",
			xfers, txcounter, rxcounter, txlvl, rxlvl);
		fflush(stderr);
*/
		/* amuse Marco */
		if ((txlvl==64) && (rxlvl==0) && (gpio_out != -1)) {
			gpio_out = gpio_out ^ 0xff;
			if (sc16is7xx_gpio_out(sc16is7xx, 0xff, gpio_out & 0xff) == -1) {
				perror("sc16is7xx_gpio_out");
				goto _out1;
			}
		}

		if (i == 0)
			continue;

		/* check if we can read from pty -> write to uart */
		if (FD_ISSET(ptyfd, &readfds)) {
			if (fgets(buf, sizeof(buf), stdin) == NULL)
			{
				continue;
			}
			i = strlen(buf);
			if (i < 0) {
				perror("read(stdin)");
				goto _out1;
			}

			j = sc16is7xx_write(sc16is7xx, buf, i);
			if (j < 0) {
				perror("sc16is7xx_write()");
				goto _out1;
			}

			if (i != j) {
				fprintf(stderr,
					"Wrote only %d of %d bytes to UART!\n",
					j, i);
			}
			i = 0;
			fprintf(stderr,"Wrote to uart: ");
			while(j > i)fprintf(stderr,"%02x ",buf[i++]);
			fprintf(stderr,"\r\n");
		}

		if (FD_ISSET(ptyfd, &writefds)) {
			i = sc16is7xx_read(sc16is7xx, buf, sizeof(buf));
			if (i < 0) {
				perror("sc16is7xx_read()");
				goto _out1;
			}

			if((buf[0] > 0) && (buf[0] < 0x7F)){
				j = 0;
				fprintf(stderr,"read form uart: ");
				while(i > j)fprintf(stderr,"%02x ",buf[j++]);
				fprintf(stderr,"\r\n");
			}
		}

	} /* while(1) */

_out1:
	free(sc16is7xx);
_out0:
	close(spi_i2c_fd);

	spi_master_power(0);
	return 0;
}
