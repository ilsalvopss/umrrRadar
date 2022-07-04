/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_FILESYSTEMLOGMANAGER_H
#define MICRORADAR_FILESYSTEMLOGMANAGER_H

#include <wx/wx.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <optional>
#include <utility>
#include <condition_variable>
#include <cfloat>
#include "umrrParser/umrrParser.h"
#include "gui_helpers.h"
#include "welfordAvg.h"
#include "triangularAvg.h"

struct timedMsg{
    umrrMessage m;
    std::chrono::time_point<std::chrono::high_resolution_clock> t;
};

class filesystemLogManager{
    std::filesystem::path out_path;
    std::ofstream out_stream;
    std::mutex fs_lock;
    std::deque<timedMsg> out_queue;
    std::mutex deque_lock;

    std::atomic<bool> isLogging = false;
    std::function<void(ErrorID, std::optional<std::string>)> callback_error;
    std::function<std::unique_ptr<avgAccumulator>()> accumulator;

    std::unique_ptr<std::thread> worker;
    std::mutex worker_lock;

    void task();

    std::chrono::milliseconds avg_period;
    modularFlags logModularFlags;

    bool dirty = false;

public:
    void openOut(const std::filesystem::path&);
    void closeOut();
    void startLogging(std::chrono::milliseconds period, std::function<std::unique_ptr<avgAccumulator>()> accumulator,
                      std::function<void(ErrorID, std::optional<std::string>)>, modularFlags modular = (modularFlags)(TIME | AVG | SEM | DEV));
    void stopLogging();
    void handle_measure(umrrMessage&);
    bool getisLogging();
    bool getisOpen();
    bool getisDirty();
    std::filesystem::path getOutPath();
};

#endif //MICRORADAR_FILESYSTEMLOGMANAGER_H