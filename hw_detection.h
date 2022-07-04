/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_HW_DETECTION_H
#define MICRORADAR_HW_DETECTION_H
#define UNICODE

#include "WinReg.hpp"

std::vector<std::wstring> EnumComs();
std::optional<std::wstring> LookForVidPidCom(std::wstring& vid, std::wstring& pid);

#endif //MICRORADAR_HW_DETECTION_H