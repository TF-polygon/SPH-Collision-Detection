#include "SerialClass.h"

Serial::Serial(const char* portName)
{
    this->connected = false;
    this->hSerial = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (this->hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else
    {
        DCB dcbSerialParams = { 0 };
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            printf("failed to get current serial parameters!");
        }
        else
        {
            dcbSerialParams.BaudRate = CBR_115200;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
            if (!SetCommState(hSerial, &dcbSerialParams))
            {
                printf("ALERT: Could not set Serial Port parameters");
            }
            else
            {
                this->connected = true;
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }

}

Serial::~Serial()
{
    if (this->connected)
    {
        this->connected = false;
        CloseHandle(this->hSerial);
    }
}

int Serial::ReadData(char* buffer, unsigned int nbChar)
{
    DWORD bytesRead;
    unsigned int toRead;
    ClearCommError(this->hSerial, &this->errors, &this->status);
    if (this->status.cbInQue > 0)
    {
        if (this->status.cbInQue > nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }
        if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
        {
            return bytesRead;
        }

    }
    return 0;

}


bool Serial::WriteData(const char* buffer, unsigned int nbChar)
{
    DWORD bytesSend;
    if (!WriteFile(this->hSerial, (void*)buffer, nbChar, &bytesSend, 0))
    {
        ClearCommError(this->hSerial, &this->errors, &this->status);
        return false;
    }
    else
        return true;
}

bool Serial::IsConnected()
{
    return this->connected;
}