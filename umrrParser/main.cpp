//
// Created by Salvo Passaro on 20/04/22.
//

#include <termios.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>
#include "umrrParser.h"

void callback(umrrMessage m){
    std::cout << " - got msg " << m.altitude << "m (" << (m.altitude_valid ? "valid" : "invalid") << ")";
    std::cout << " [speed = " << m.vertical_speed << "m/s]" << std::endl;
}

int main(int argc, char** argv){
    int fd, len;
    struct termios oldtio,newtio;
    fd_set readfs;
    uint8_t buf[255];

    fd = open("/dev/cu.usbserial-AB7EYCUH", O_RDWR | O_NOCTTY);

    if (fd <0)
    {
        printf("\nerror opening serial port.\n");
        exit (-1);
    }

    tcgetattr(fd,&oldtio); /* save current port settings */
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_iflag=0;
    newtio.c_oflag=0;
    newtio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    newtio.c_lflag=0;

    cfsetospeed(&newtio,B115200);
    cfsetispeed(&newtio,B115200);

    newtio.c_cc[VTIME]    = 50;  // (50 / 10) = 5 sec Timeout
    newtio.c_cc[VMIN]     = 0;   // only read as many bytes as there are currently in the buffer.

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    std::function<void(umrrMessage)> cb = callback;
    umrrParser parser(cb);

    while(1) {
        len = read(fd, buf, 255);

        if(len == 0)
            break;

        parser.consume_bytes(buf, len);
    }

    tcsetattr(fd,TCSANOW,&oldtio);

    return 0;
}