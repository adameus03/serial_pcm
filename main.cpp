#include <iostream>
#include <cstdlib>
#include <windows.h>
#include "transfer.h"

int main(){
    HANDLE commHandle = get_comm("COM4", 115200);
    transfer(commHandle, "C:\\Users\\amade\\Downloads\\przechwyt.raw");
    close_comm(commHandle);
    return 0;
}
