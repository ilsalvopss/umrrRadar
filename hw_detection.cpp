/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "hw_detection.h"
#include <iostream>

/*
 * queries Windows registry to enum COM ports.
 * it also verifies that the port is actually a DosDevice
 */
std::vector<std::wstring> EnumComs(){
    std::vector<std::wstring> coms;
	wchar_t buf[500];

    winreg::RegKey root{HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", KEY_READ};

    if(root) {
        for (auto subcom: root.EnumValues()) {
            auto k = root.GetStringValue(subcom.first);

            if (k.rfind(L"COM", 0) == 0)
                if(QueryDosDevice(k.c_str(), buf, 500) != 0)
                    coms.push_back(k);
        }
    }
    return coms;
}

/*
 * queries Windows registry to try and find USB devices with vid and pid AND a valid serial port associated
 */
std::optional<std::wstring> LookForVidPidCom(std::wstring &vid, std::wstring &pid) {
    wchar_t buf[500];

    try {
        winreg::RegKey root{HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum", KEY_READ};

        for (const auto& enum_subkey_name: root.EnumSubKeys()) {
            winreg::RegKey upkey{root.Get(), enum_subkey_name, KEY_READ};
            for (const auto& sub: upkey.EnumSubKeys()) {
                if (sub.find(L"VID_" + vid) != std::wstring::npos && sub.find(L"PID_" + pid) != std::wstring::npos) {
                    winreg::RegKey devkey{upkey.Get(), sub, KEY_READ};
                    for (const auto& uniq: devkey.EnumSubKeys()) {
                        winreg::RegKey subdevkey{devkey.Get(), uniq, KEY_READ};
                        for (const auto& subuniq: subdevkey.EnumSubKeys()) {
                            if (subuniq == L"Device Parameters") {
                                winreg::RegKey devparamkey{subdevkey.Get(), subuniq, KEY_READ};
                                for (const auto& devparams: devparamkey.EnumValues()) {
                                    if (devparams.first == L"PortName") {
                                        auto com = devparamkey.GetStringValue(devparams.first);
                                        if(QueryDosDevice(com.c_str(), buf, 500) != 0)
                                            return com;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } catch (winreg::RegException &e) {
        /// let's not bother user with OS errors,
        /// calling methods will notify failures
    }

    return {};
}