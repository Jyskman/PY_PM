


#include <iostream>
#include <pybind11/pybind11.h>
#include "PY_PM.h"

// phyton.h was found after using this compile command
// sudo g++ -I/usr/include/python2.7 -lpython2.7 ptest.cxx -o ptest.o
// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ptest.cxx -o ptest$(python3-config --extension-suffix)

// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ptest.cxx -o ptest.so
// g++ -I/usr/include/python2.7 -lpython2.7 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) PY_PM.cxx -o PY_PM.so


namespace py = pybind11;

int add( int i, int j ) {
	return i + j;
};

OPC::OPC() {



	//~ // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
	serial_port = open("/dev/ttyACM0", O_RDWR);

	// Create new termios struc, we call it 'tty' for convention
	struct termios tty;

	// Read in existing settings, and handle any error
	if(tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		//~ return 1;
	}

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	// Set in/out baud rate to be 9600
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	// Save tty settings, also checking for error
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		//~ return 1;
	}

	
};

void OPC::ISS_setup() {

	int bytes_read = 0;
	char main_read_buf[10];
	memset(&main_read_buf, '\0', sizeof(main_read_buf)); 
	
	
	unsigned char send[4] = {0x5A, 0x02,0x92,0x0D} ;
	write(serial_port, send, 4);
	bytes_read = read(serial_port, &main_read_buf, sizeof(main_read_buf));

	usleep(1000);


};


//~ PYBIND11_MODULE( PY_PM, m ) {
	//~ // m.doc() = "OPC N3 module for RPI using USB ISS";
	//~ // m.def("add", &add, "Func");
	
	//~ py::class_<test>(m, "test")
	//~ .def(py::init())
	//~ .def("adder",&test::adder);
	
	
//~ };

PYBIND11_MODULE( PY_PM, m ) {
	//~ m.doc() = "OPC N3 module for RPI using USB ISS";
	//~ m.def("add", &add, "Func");
	
	py::class_<OPC>(m, "OPC")
	.def(py::init())
	.def("ISS_setup",&OPC::ISS_setup);
	
	
};

