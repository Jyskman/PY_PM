


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

void OPC::OPC_close(){
	
	
	close(serial_port);
	
	
};

void OPC::OPC_fan(bool state) {
	
	unsigned char send_command[2] = {0x61, 0x03} ;
	unsigned char send_fan_on[3] = {0x61, 0x03,0x03} ;
	unsigned char send_fan_off[3] = {0x61, 0x03,0x02} ;

	int bytes_read = 0;
	char main_read_buf[10];
	memset(&main_read_buf, '\0', sizeof(main_read_buf)); 


	if ( state == true ) {

		write(serial_port, send_command, 2);
		write(serial_port, send_fan_on, 3);
		
	};
	
	if (state == false) {

		write(serial_port, send_command, 2);
		write(serial_port, send_fan_off, 3);	
		
	};

	usleep(10000);
	bytes_read = read(serial_port, &main_read_buf, sizeof(main_read_buf));
};


void OPC::OPC_read_config() {
	
	unsigned char send_command[2] = {0x61, 0x3C} ;
	
	char send_command_1[63];
	char send_command_2[63];
	char send_command_3[46]; // 46
	
	int read_bytes = 0;
	char read_buf[200];
	// 0 - 167 is 168 values. 168 + 1, 169, is a 168 array 
	
	memset(&read_buf, '\0', sizeof(read_buf));
		
	memset(&send_command_1, 0x3C, sizeof(send_command_1));
	memset(&send_command_2, 0x3C, sizeof(send_command_2));
	memset(&send_command_3, 0x3C, sizeof(send_command_3));
	
		
	send_command_1[0] = 0x61;
	send_command_2[0] = 0x61;
	send_command_3[0] = 0x61;
		
	
	write(serial_port, send_command, 2);
	read_bytes = read(serial_port, &read_buf, sizeof(read_buf));
	printf("Bytes read %i \n", read_bytes);
	usleep(10000);
	
	write(serial_port, send_command_1, 63);
	write(serial_port, send_command_2, 63);
	write(serial_port, send_command_3, 46);
		
	usleep(10000);
	
	memset(&read_buf, '\0', sizeof(read_buf));
	read_bytes = read(serial_port, &read_buf, sizeof(read_buf));
	printf("Bytes read %i \n", read_bytes);
	
	
	// Create an array where data is copied from the read_buf. It is to avoid the FF etc bytes
	char data_in[168];
	memset(&data_in, 0, sizeof(data_in));
	
	
	// pointer to the first element in read_buf
	char * point_1;
	point_1 = &read_buf[0];	
	
	// copy 61 elements from read_buf pos 0+2 to data_in array
	memcpy( &data_in, point_1+2,61 );	
	
	
	// now make pointer to data_in array
	char * data_in_point;
	data_in_point = &data_in[0]; 
	
	// copy 62 elements from pos 0+64 in read_buf to pos 0+61 in data_in array
	memcpy( data_in_point+61, point_1+64,62 );
	
	// copy 45 elements from read_buf pos 0+127 to pos 0+123 in data_in
	memcpy( data_in_point+123, point_1+127,45 );
	
	//~ for ( unsigned int i = 0; i < sizeof(data_in); i++ ) {
		//~ printf("Byte %x at pos %i \n", data_in[i],i);
	//~ };
	
	memcpy( &OPC_struct_config_variables, data_in_point,sizeof(OPC_struct_config_variables)  );
	
	
	

	printf("Bin Boundries ADC");
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC0);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC1);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC2);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC3);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC4);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC5);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC6);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC7);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC8);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC9);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC10);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC11);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC12);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC13);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC14);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC15);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC16);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC17);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC18);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC19);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC20);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC21);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC22);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC23);
	printf(" ADC %i \n",OPC_struct_config_variables.binBoundriesADC24);

printf("------------------------------------------------- \n");
printf("Bin Boundries Diametors \n");


	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor0);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor1);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor2);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor3);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor4);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor5);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor6);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor7);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor8);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor9);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor10);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor11);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor12);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor13);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor14);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor15);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor16);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor17);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor18);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor19);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor20);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor21);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor22);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor23);
	printf(" Bin D %i \n",OPC_struct_config_variables.binBoundriesDiametor24);

	printf("------------------------------------------------- \n");
	printf("Bin Weights \n");

	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings0);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings1);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings2);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings3);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings4);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings5);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings6);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings7);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings8);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings9);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings10);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings11);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings12);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings13);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings14);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings15);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings16);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings17);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings18);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings19);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings20);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings21);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings22);
	printf(" Bin We %i \n",OPC_struct_config_variables.binWeightings23);
	       
       
	printf("------------------------------------------------- \n");
	printf("PM Diametors \n");

	printf(" PM D A %i \n",OPC_struct_config_variables.pmDiametorA);
	printf(" PM D B %i \n",OPC_struct_config_variables.pmDiametorB);
	printf(" PM D C %i \n",OPC_struct_config_variables.pmDiametorC);
       
	printf("------------------------------------------------- \n");
	printf("PM MSLNS \n");

	printf(" maximumTimeOfFlight %i \n",OPC_struct_config_variables.maximumTimeOfFlight);
	printf(" AMSamplingIntervalCount %i \n",OPC_struct_config_variables.AMSamplingIntervalCount);
	printf(" AMIdleIntervalCount %i \n",OPC_struct_config_variables.AMIdleIntervalCount);
	printf(" AMMaxDataArraysInFile %i \n",OPC_struct_config_variables.AMMaxDataArraysInFile);
	printf(" AMOnlySavePMData %i \n",OPC_struct_config_variables.AMOnlySavePMData);
	printf(" AMFanOnInIdle %i \n",OPC_struct_config_variables.AMFanOnInIdle);
	
	
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
	.def("ISS_setup",&OPC::ISS_setup)
	.def("OPC_close",&OPC::OPC_close)
	.def("OPC_read_config",&OPC::OPC_read_config)
	.def("OPC_fan",&OPC::OPC_fan);
	
	
};

