#ifndef TRANSFER_H_INCLUDED
#define TRANSFER_H_INCLUDED

#include <windows.h>
#include <fstream>
#include <cstring>
#include <iostream>

typedef unsigned char uchar;
typedef unsigned short ushort;

HANDLE get_comm(LPCSTR lp_file_name, DWORD baud_rate);

void close_comm(HANDLE handle);

void transfer(const HANDLE comm_handle, const char* target_path);

#endif // TRANSFER_H_INCLUDED
