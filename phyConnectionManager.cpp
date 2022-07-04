/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "phyConnectionManager.h"

#include <utility>

/*
 * these are convenience wrappers around windows serial APIs.
 * I wrote them to avoid bloating modern c++ with very old-looking native calls
 *
 * errors returned as a non-zero value by Windows APIs are delegated to exceptions
 */
#ifdef _WIN32
bool phyConnectionManager::WinApiWrap_Open(std::wstring& port, bool overlapped, DWORD baud) {
    const std::wstring formattedPort = L"\\\\.\\" + port;

    WinApiWrap_Close(); //In case for some reason we are already open

    m_hComm = CreateFile(formattedPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, overlapped ? FILE_FLAG_OVERLAPPED : 0, nullptr);
    if(m_hComm == INVALID_HANDLE_VALUE)
        throw std::runtime_error("invalid port");

    COMMTIMEOUTS timeouts{};
    if(!GetCommTimeouts(m_hComm, &timeouts))
        throw std::runtime_error("timeouts_get");

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 2500; //2.5s timeout

    if(!SetCommTimeouts(m_hComm, &timeouts))
        throw std::runtime_error("timeouts_set");

    DCB dcb{};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(m_hComm, &dcb))
        throw std::runtime_error("dcb_get");

    dcb.BaudRate = baud;
    dcb.Parity = NOPARITY;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDsrSensitivity = FALSE;

    // no flow control
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    if(!SetCommState(m_hComm, &dcb))
        throw std::runtime_error("dcb_set");

    return true;
}

void phyConnectionManager::WinApiWrap_Close(){
    if(m_hComm != nullptr && m_hComm != INVALID_HANDLE_VALUE)
        CloseHandle(m_hComm);

    m_hComm = INVALID_HANDLE_VALUE;
}

DWORD phyConnectionManager::WinApiWrap_Read(void* lpBuffer, DWORD dwNumberOfBytesToRead){
    DWORD actuallyRead = 0;

    if(!ReadFile(m_hComm, lpBuffer, dwNumberOfBytesToRead, &actuallyRead, nullptr))
        throw std::runtime_error("serial_read");

    return actuallyRead;
}

DWORD phyConnectionManager::WinApiWrap_Write(const void* lpBuffer, DWORD dwNumberOfBytesToWrite){
    DWORD actuallyWrote = 0;

    if(!WriteFile(m_hComm, lpBuffer, dwNumberOfBytesToWrite, &actuallyWrote, nullptr))
        throw new std::runtime_error("serial_write");

    return actuallyWrote;
}

#endif

/*
 * task executed by the worker.
 * it reads as much as possible from the serial port and passes it to umrrParser
 */
void phyConnectionManager::task() {
    while(do_work){
        std::unique_lock<std::mutex> _l(parserLock);

        try{
            uint8_t buf[1024];
            unsigned int r = 0;
			{
				std::unique_lock<std::mutex> _p(phyLock);
				if(phyOpen)
					r = WinApiWrap_Read(&buf, 1024);
				else
					break;
			}
            if(!r)
                throw std::runtime_error("serial timeout");

            parser->consume_bytes(buf, r);
        }catch (std::runtime_error& e){
            do_work = false;
            close_phy();

            error_callback(Phy_Failed, std::nullopt);
        }
    }
	
	{
		std::unique_lock<std::mutex> _l(parserLock);
		worker->detach();
		worker = nullptr;
	}
}

void phyConnectionManager::setCom(std::wstring& _com) {
    std::unique_lock<std::mutex> _l(phyLock);

    com = _com;
}

void phyConnectionManager::setBaud(unsigned int _baud) {
    std::unique_lock<std::mutex> _l(phyLock);

    baud = _baud;
}

void phyConnectionManager::open_phy(std::function<void(const ErrorID, const std::optional<std::string>&)> error_cb) {
    std::unique_lock<std::mutex> _l(phyLock);

    if(phyOpen)
        return;

    if(com.empty() || !baud)
        return;

    error_callback = std::move(error_cb);
#ifdef _WIN32
    try {
        WinApiWrap_Open(com, false, baud);
    }catch(std::runtime_error& e){
        wxMessageBox("failed to open serial port!", "serial handle exception", wxICON_WARNING);
        return;
    }
#endif

    phyOpen = true;
}

void phyConnectionManager::close_phy() {
    std::unique_lock<std::mutex> _l(phyLock);

    if(!phyOpen)
        return;

    if(do_work)
        stop_listening();

#ifdef _WIN32
    WinApiWrap_Close();
#endif

    phyOpen = false;
}

/*
 * constructs an umrrParser and spawns a worker
 */
void phyConnectionManager::start_listening(std::function<void(umrrMessage)>& pc) {
    std::unique_lock<std::mutex> _l(parserLock);

    if(do_work)
        return;

    {
        std::unique_lock<std::mutex> _p(phyLock);
        if (!phyOpen)
            return;
    }

    parser = std::make_unique<umrrParser>(pc);

    do_work = true;
    worker = std::make_unique<std::thread>(&phyConnectionManager::task, this);
}

void phyConnectionManager::stop_listening() {
    if(!do_work)
        return;

    do_work = false;
}

bool phyConnectionManager::isPhyOpen() {
    std::unique_lock<std::mutex> _l(phyLock);

    return phyOpen;
}

std::wstring phyConnectionManager::getCom() {
    std::unique_lock<std::mutex> _l(phyLock);

    return com;
}

unsigned int phyConnectionManager::getBaud() {
    std::unique_lock<std::mutex> _l(phyLock);

    return baud;
}
