#include <windows.h>
#include <fstream>
#include <cstring>
#include <iostream>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long long ull;

HANDLE get_comm(LPCSTR lp_file_name, DWORD baud_rate){
    HANDLE handle = CreateFile(lp_file_name, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(handle==INVALID_HANDLE_VALUE){
        throw "Error occured while trying to create the COM handle";
    }
    DCB dcb_params = {0};
    dcb_params.DCBlength = sizeof(dcb_params);
    if(!GetCommState(handle, &dcb_params)){
        throw "Error occured while trying to load the COM state";
    }
    dcb_params.BaudRate = baud_rate;
    dcb_params.ByteSize = 0x8;
    dcb_params.StopBits = ONESTOPBIT;
    dcb_params.Parity = NOPARITY;
    if(!SetCommState(handle, &dcb_params)){
        throw "Error occured while trying to store the COM state";
    }

    COMMTIMEOUTS timeouts = {0};
    if(!GetCommTimeouts(handle, &timeouts)){
        throw "Error occured while trying to load COM timeouts";
    }
    timeouts.ReadTotalTimeoutConstant = 10000;
    timeouts.WriteTotalTimeoutConstant = 100;
    if(!SetCommTimeouts(handle, &timeouts)){
        throw "Error occured while trying to store COM timeouts";
    }

    //PurgeComm(handle, PURGE_RXCLEAR);

    return handle;
}

void close_comm(HANDLE handle){
    if(!CloseHandle(handle)){
        throw "Failure while trying to close the COM handle";
    };
}

void transfer(const HANDLE comm_handle, const char* target_path){
    std::ofstream f(target_path, std::ofstream::trunc | std::ofstream::binary);
    std::filebuf* f_pbuf = f.rdbuf();
    uchar* samples = new uchar[0x8];
    uchar nbr = 0;

    ushort max_ushort = 0xffff;
    while(true){
        ReadFile(comm_handle, samples, 0x8, (LPDWORD)&nbr, NULL);

        if(nbr != 0x8){
            throw "Invalid nbr";
        }

        ushort u0 = (ushort)*samples | ((ushort)*(samples+0x1) << 0x8);
        short s0 = (short)u0 - (short)max_ushort;
        ushort u1 = (ushort)*(samples+0x2) | ((ushort)*(samples+0x3) << 0x8);
        short s1 = (short)u1 - (short)max_ushort;
        ushort u2 = (ushort)*(samples+0x4) | ((ushort)*(samples+0x5) << 0x8);
        short s2 = (short)u2 - (short)max_ushort;
        ushort u3 = (ushort)*(samples+0x6) | ((ushort)*(samples+0x7) << 0x8);
        short s3 = (short)u3 - (short)max_ushort;

        memcpy(samples, reinterpret_cast<uchar*>(&s0), 0x2);
        memcpy(samples+0x2, reinterpret_cast<uchar*>(&s1), 0x2);
        memcpy(samples+0x4, reinterpret_cast<uchar*>(&s2), 0x2);
        memcpy(samples+0x6, reinterpret_cast<uchar*>(&s3), 0x2);

        ull samples64 = (ull)*samples | ((ull)*(samples+0x1) << 0x8) | ((ull)*(samples+0x2) << 0x10) | ((ull)*(samples+0x3) << 0x18) | ((ull)*(samples+0x4) << 0x20) | ((ull)*(samples+0x5) << 0x28) | ((ull)*(samples+0x6) << 0x30) | ((ull)*(samples+0x7) << 0x38);
        std::cout << std::hex << samples64 << std::dec << std::endl;

        //std::cout << "Read byte" << std::endl;
        f_pbuf->sputn((char*)samples, 8);
    }
}




