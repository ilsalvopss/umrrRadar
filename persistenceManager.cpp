/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "persistenceManager.h"

/*
 * persistenceManager constructor
 * checks if config file exists where expected. if not (or inaccessible) one is built and dumped to the filesystem
 */
persistenceManager::persistenceManager(const std::string& path) {
    configFile = path;

    std::filesystem::file_type confT = std::filesystem::status(configFile).type();

    if(confT == std::filesystem::file_type::unknown){
        wxMessageBox("failed to open config file. this will cause issues. (bad file permissions?)", "config file open exception", wxICON_WARNING);

        return;
    }

    if(confT == std::filesystem::file_type::not_found){
        root = json::object();

        root["connectionSettings"] = json::object();
        root["measureSettings"] = json::object();
        root["logSettings"] = json::object();

        root["connectionSettings"]["usbAutoDetection"] = json::object();
        root["connectionSettings"]["usbAutoDetection"]["vid"] = "";
        root["connectionSettings"]["usbAutoDetection"]["pid"] = "";
        root["connectionSettings"]["usbAutoDetection"]["enabled"] = false;

        root["connectionSettings"]["serialPort"] = json::object();
        root["connectionSettings"]["serialPort"]["baud"] = "115200";

        root["connectionSettings"]["autoOpenConnection"] = true;
        root["connectionSettings"]["autoStartLogging"] = false;

        root["logSettings"]["averagePeriod"] = 1000;

        try {
            std::ofstream raw_settings(configFile);
            raw_settings << root;
        }catch(std::exception& e){
            wxMessageBox("failed to write config file to filesystem. this will cause issues.", "config file writing exception", wxICON_WARNING);
        }
    }
    else{
        try {
            std::ifstream raw_settings(configFile);

            raw_settings >> root;
        } catch (std::exception& e){
            wxMessageBox("config file is corrupted/malformed. please delete it and restart the software to generate a clean one", "config file parsing exception", wxICON_ERROR);
        }
    }
}

/*
 * returns a copy of the current connectionSettings section
 */
json persistenceManager::getConnectionSettings() {
    std::unique_lock<std::mutex> l(root_lock);
    json obj;
    obj = root["connectionSettings"];

    return obj;
}

/*
 * returns a copy of the current logSettings section
 */
json persistenceManager::getLogSettings() {
    std::unique_lock<std::mutex> l(root_lock);

    json obj;
    obj = root["logSettings"];

    return obj;
}

/*
 * sets auto* values without dumping them to the filesystem
 */
void persistenceManager::setAutoToggle(const std::optional<bool> autoconnect, const std::optional<bool> autolog) {
    std::unique_lock<std::mutex> l(root_lock);

    if(autoconnect.has_value())
        root["connectionSettings"]["autoOpenConnection"] = autoconnect.value();

    if(autolog.has_value())
        root["connectionSettings"]["autoStartLogging"] = autolog.value();
}

/*
 * the following three setters update the root with different value types.
 * the first parameter is a super convenient pointer offered by the json library
 *  to specify the inserting site similarly as a path
 */
void persistenceManager::setByPath(const json::json_pointer& pointer, std::string& val){
    std::unique_lock<std::mutex> l(root_lock);

    root[pointer] = val;
}

void persistenceManager::setByPath(const json::json_pointer& pointer, int val){
    std::unique_lock<std::mutex> l(root_lock);

    root[pointer] = val;
}

void persistenceManager::setByPath(const json::json_pointer& pointer, bool val) {
    std::unique_lock<std::mutex> l(root_lock);

    root[pointer] = val;
}

/*
 * dumps root to the filesystem
 */
void persistenceManager::dumpRoot() {
    std::unique_lock<std::mutex> l(root_lock);

    try {
        std::ofstream raw_settings(configFile);

        raw_settings << root;

        raw_settings.flush();
    } catch (std::exception& e){
        wxMessageBox("config file is corrupted/malformed. please delete it and restart the software to generate a clean one", "config file parsing exception", wxICON_ERROR);
    }
}