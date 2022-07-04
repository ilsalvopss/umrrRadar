/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_PERSISTENCEMANAGER_H
#define MICRORADAR_PERSISTENCEMANAGER_H

#include <wx/wx.h>
#include "json.hpp"
#include <fstream>
#include <mutex>
#include <optional>

using json = nlohmann::json;

class persistenceManager{
    json root;
    std::filesystem::path configFile;
    std::mutex root_lock;

public:
    explicit persistenceManager(const std::string& path= "microRadar.config");
    json getConnectionSettings();
    json getLogSettings();
    void setAutoToggle(std::optional<bool> = std::nullopt, std::optional<bool> = std::nullopt);
    void setByPath(const json::json_pointer&, std::string&);
    void setByPath(const json::json_pointer&, int);
    void setByPath(const json::json_pointer&, bool);
    void dumpRoot();
};

#endif //MICRORADAR_PERSISTENCEMANAGER_H