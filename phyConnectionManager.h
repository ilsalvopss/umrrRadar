/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_PHYCONNECTIONMANAGER_H
#define MICRORADAR_PHYCONNECTIONMANAGER_H

#include "wx/wx.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>
#include "umrrParser/umrrParser.h"
#include "gui_helpers.h"

#ifdef _WIN32
#define UNICODE
#include "windows.h"
#endif

class phyConnectionManager{
    std::unique_ptr<std::thread> worker;
    std::unique_ptr<umrrParser> parser;

    std::mutex phyLock;
    std::mutex parserLock;

    std::wstring com;
    unsigned int baud;

    std::atomic<bool> do_work = false;
    std::atomic<bool> phyOpen = false;

    std::function<void(const ErrorID, const std::optional<std::string>&)> error_callback;

    void task();

#ifdef _WIN32
    HANDLE m_hComm = INVALID_HANDLE_VALUE;

    bool WinApiWrap_Open(std::wstring& port, bool overlapped = false, DWORD baud = 115200);
    void WinApiWrap_Close();
    DWORD WinApiWrap_Read(void* lpBuffer, _In_ DWORD dwNumberOfBytesToRead);
    DWORD WinApiWrap_Write(const void* lpBuffer, DWORD dwNumberOfBytesToWrite);
#endif

public:
    void setCom(std::wstring&);
    void setBaud(unsigned int);
    bool isPhyOpen();
    std::wstring getCom();
    unsigned int getBaud();
    void open_phy(std::function<void(const ErrorID, const std::optional<std::string>&)>);
    void close_phy();
    void start_listening(std::function<void(umrrMessage)>&);
    void stop_listening();
};

#endif //MICRORADAR_PHYCONNECTIONMANAGER_H