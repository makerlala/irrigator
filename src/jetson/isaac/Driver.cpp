// The code on which this is based on is lincensed under GPL-2.0-only
// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 *
 * Modified by Dumi Loghin - 2020.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include  <signal.h>

#include "Driver.hpp"

static const char *device1 = "/dev/spidev0.0";
static const char *device2 = "/dev/spidev1.0";

static int fd1, fd2;

pthread_mutex_t lock1, lock2;

volatile int run_flag = 1;

static uint32_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1000;
static uint16_t delay;

// sensors
volatile float dist1;
volatile float dist2;

union _spi_float {
	float val;
	uint8_t bytes[4];
};

union _spi_int {
	int val;
	uint8_t bytes[4];
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

int asynctask(void* (*task)(void* args), void* arg) {
	pthread_t th;
	pthread_attr_t attr;

	// set thread detachstate attribute to DETACHED 
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	return pthread_create(&th, &attr, task, arg);
}

void  sigint_handler(int sig) {
	printf("Exiting on Ctrl+C ...\n");
	run_flag = 0;
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, unsigned int len)
{
	int ret;
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.speed_hz = speed;
	tr.delay_usecs = delay;
	tr.bits_per_word = bits;
	/*
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.speed_hz = speed,
		.delay_usecs = delay,
		.bits_per_word = bits
	};
	*/
	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");	

	// usleep(200000);
}

int open_spi(const char *device) {
	int ret;
	int fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	return fd;
}

int read_int_spi(int spi_fd, pthread_mutex_t lock, uint8_t command) {
	uint8_t tx = 'r';
	uint8_t xx;
	union _spi_int rx;
	pthread_mutex_lock(&lock);
	transfer(spi_fd, &command, &xx, 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[0]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[1]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[2]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[3]), 1);
	pthread_mutex_unlock(&lock);
	return rx.val;	
}

int read_float_spi(int spi_fd, pthread_mutex_t lock, uint8_t command) {
	uint8_t tx = 'r';
	uint8_t xx;
	union _spi_float rx;
	pthread_mutex_lock(&lock);
	transfer(spi_fd, &command, &xx, 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[0]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[1]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[2]), 1);
	transfer(spi_fd, &tx, (uint8_t*)&(rx.bytes[3]), 1);
	pthread_mutex_unlock(&lock);
	return rx.val;	
}

uint8_t send_spi(int spi_fd, pthread_mutex_t lock, uint8_t command) {
	uint8_t ret = 0;
	pthread_mutex_lock(&lock);
	transfer(spi_fd, &command, &ret, 1);
	pthread_mutex_unlock(&lock);	
	return ret;
}

static inline void go_forward() {
	send_spi(fd1, lock1, 'w');
}

static inline void go_left() {
	send_spi(fd1, lock1, 'a');
}

static inline void go_left_90() {
	send_spi(fd1, lock1, 'a');
	sleep(3);
	send_spi(fd1, lock1, 'q');
}

static inline void go_right() {
	send_spi(fd1, lock1, 'd');
}

static inline void go_right_90() {
	send_spi(fd1, lock1, 'd');
	sleep(3);
	send_spi(fd1, lock1, 'q');
}

static inline void go_backwards() {
	send_spi(fd1, lock1, 's');
}

static inline void stop() {
	send_spi(fd1, lock1, 'q');
}

static inline void on_light() {
	send_spi(fd1, lock1, 'l');
}

static inline void off_light() {
	send_spi(fd1, lock1, 'k');
}

static inline void on_pump() {
	send_spi(fd1, lock1, 'm');
}

static inline void off_pump() {
	send_spi(fd1, lock1, 'n');
}

int run_ai() {
	return system("./run-all-models.sh");
}

void* read_sensors(void *args) {
	int t = 0;
	while (run_flag) {
		// int left_enc = read_int_spi(fd2, lock2, (uint8_t)'0');
		// int right_enc = read_int_spi(fd2, lock2, (uint8_t)'1');
		dist1 = read_float_spi(fd2, lock2, (uint8_t)'2');
		dist2 = read_float_spi(fd2, lock2, (uint8_t)'3');
		// float current_all = read_float_spi(fd1, lock1, (uint8_t)'0');
		// float water_level = read_float_spi(fd1, lock1, (uint8_t)'4');
		t++;
		if (t == 8) {
			t = 0;
			// printf("Encoder left : %d\n", left_enc);
			// printf("Encoder right: %d\n", right_enc);
			printf("Distance 1: %4.2f\n", dist1);
			printf("Distance 2: %4.2f\n", dist2);
			// printf("Current all: %4.2f\n", current_all);
			// printf("Water level: %4.2f\n", water_level);
			printf("-------------------\n");
		}
		usleep(250000);
		// sleep(1);
	}
	printf("Exiting sensors reading thread ...\n");
	return NULL;
}

// ---------------- Driver Class ---------------------

namespace isaac {

namespace irrigator {

Driver::Driver() {}

Driver::~Driver() {}

void Driver::start() {
	fd1 = open_spi(device1);
	fd2 = open_spi(device2);
	
	if (pthread_mutex_init(&lock1, NULL) != 0) { 
       		pabort("Mutex init has failed (lock1) \n"); 
        	return; 
    	}
	if (pthread_mutex_init(&lock2, NULL) != 0) { 
        	pabort("Mutex init has failed (lock2) \n"); 
        	return; 
    	}

	asynctask(read_sensors, NULL);

	sleep(2);

	go_forward();

	_state = 0;

	tickPeriodically();
}

void Driver::stop() {
	run_flag = false;
	off_pump();
	usleep(250000);
	off_light();
	usleep(250000);
	stop();
	close(fd1);
	close(fd2);
}

void Driver::tick() {
	if (!run_flag)
		return;
	
	// state 0 -> go forward
	if (_state == 0) {
		if (dist1 < 100 || dist2 < 100) {		
			_state = 1;
			stop();
			usleep(250000);
			on_light();
			auto proto = tx_command().initProto();
  			proto.setMessage("detect");
  			tx_command().publish();
		}
	}
	else {
		// state 1 -> stopped, waiting for reply
		if (_state == 1) {
			auto proto = rx_result().getProto();
  			const std::string message = proto.getMessage();
			if (message == "yes") {
				// state 2 -> irrigate
				_state = 2;
				go_forward();
				sleep(2);
				stop();
				usleep(250000);
				on_pump();
				sleep(5);
				off_pump();
				usleep(250000);
				go_backwards();
				sleep(2);
				stop();
				usleep(250000);
			}
		}
	}
}

} // namespace irrigator

} // namespace isaac
