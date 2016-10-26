/*
 * @file    main.cpp
 * @author  Simon Brummer
 * @version 0.2
 * @desc    Tutorium 3. Ansprechen und konfiguration der Seriellen Schnittstelle.
 *          Dieses Beispiel braucht zwei per RS232 verbundene Recher.
 *          Der initiale Sender muss #SENDER definiert haben.
 *          Der Empfaenger darf nicht #SENDER definiert haben.
 */

#include <iostream>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

// Begin of .h-File
typedef struct {
	uint8_t data;
}Packet;

class Serial{
private:
	const char* dev_;
	int fdesc_;

public:
	Serial();
	~Serial();
	void config(void);
	int sendPacket(Packet* p);
	int recvPacket(Packet* p);

private:
	Serial(const Serial& other);
	Serial& operator=(const Serial& other);
};
// End of .h-File

// Begin of cpp File
Serial::Serial(){
	// Open File descriptor
	this->dev_ = "/dev/ser1";
	this->fdesc_ = open(this->dev_, O_RDWR);
	if(this->fdesc_ == -1){
		exit(-1);
	}

	// Configure Serial Interface
	this->config();
}

Serial::~Serial(){
	// Close File descriptor
	if( close(this->fdesc_) < 0 ){
		exit(-1);
	}
}

void Serial::config(void){
    struct termios ts;
    tcflush(this->fdesc_, TCIOFLUSH);
    tcgetattr(this->fdesc_, &ts);
    cfsetispeed(&ts, B19200);
    cfsetospeed(&ts, B19200);
    ts.c_cflag &= ~CSIZE;
    ts.c_cflag &= ~CSTOPB;
    ts.c_cflag &= ~PARENB;
    ts.c_cflag |= CS8;
    ts.c_cflag |= CREAD;
    ts.c_cflag |= CLOCAL;
    tcsetattr(this->fdesc_, TCSANOW, &ts);
}

int Serial::sendPacket(Packet* p){
	write(this->fdesc_, p, sizeof(Packet));
	return 0;
}

int Serial::recvPacket(Packet* p){
	if( readcond(this->fdesc_, p, sizeof(Packet), sizeof(Packet), 0, 0) > 0) {
		return 0;
	}
	return -1;
}
// End of .cpp-File


int main() {

	//
	Serial ser;


	// P allokieren und initialisieren.
	Packet p;
	memset(&p, 0 , sizeof(p));

	p.data = 1;

	int max = 9;
	int min = 15;

	// Sender beginnt als erstes Daten zu senden.
    #define SENDER
	#ifdef SENDER
		ser.sendPacket(&p);
		//cout << "Ping: " << p.data << endl;
		cout << "Conn: " << (int)p.data << endl;
	#endif

	// Empfangs und Sendeschleife
	while(true){
		ser.recvPacket(&p);
		//if(p.data==1){
		//cout << "Connection est: " << p.data << endl;

		cout << "received: " << p.data << endl;

		//----------------------------------------------------
		if ((int)p.data==11){
			cout << "do some 11" << endl;
		} else if((int)p.data==12){
			cout << "do some 12" << endl;
		} else {
			cout << "---000---" << endl;
		}
		//-----------------------------------------------------

		int randNum = rand()%(max-min + 1) + min;
		p.data = randNum;
		cout << "generate: " << p.data << endl;
		ser.sendPacket(&p);
		cout << "send: " << p.data << endl;
		usleep(1000000);

		//}


	}
}
