/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include "filesystemLogManager.h"

#include <utility>

/*
 * opens log file, if it's allowed to by the OS
 * and creates an ofstream
 */
void filesystemLogManager::openOut(const std::filesystem::path& path) {
    if(out_stream.is_open())
        closeOut();

    dirty = false;

    std::unique_lock<std::mutex> _l(fs_lock);

    out_path = path;

    std::filesystem::file_type outT = std::filesystem::status(out_path).type();
    if(outT == std::filesystem::file_type::unknown){
        wxMessageBox("failed to open output file. this will cause issues. (bad file permissions?)", "output file open exception", wxICON_WARNING);
        return;
    }

    out_stream = std::ofstream(out_path);
}

void filesystemLogManager::closeOut() {
    std::unique_lock<std::mutex> _l(fs_lock);

    if(isLogging)
        stopLogging();

    out_stream.flush();
    out_stream.close();
}

/*
 * task executed by the filesystemLogManager worker.
 * swaps the message queue with a fresh one and dumps valid messages to the output stream through the chosen accumulator
 *
 * then waits for the next appropriate time slot accounting for the time it already wasted dumping values
 */
void filesystemLogManager::task() {
    while(isLogging){
        std::unique_lock<std::mutex> _l(worker_lock);
        std::deque<timedMsg> local;
        const std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
        const auto wall_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + avg_period/2);
        std::unique_ptr<avgAccumulator> acc = accumulator();

        {
            std::unique_lock<std::mutex> _d(deque_lock);
            out_queue.swap(local);
        }


        if(logModularFlags & RAW){
            std::unique_lock<std::mutex> _f(fs_lock);

            for (auto m: local)
                if (start - m.t < avg_period)
                    out_stream << std::fixed << std::setprecision(6) << m.m.altitude << std::endl;

        } else {
            for (auto m: local)
                if (start - m.t < avg_period)
                    acc->newValue(m.m.altitude);

            {
                std::unique_lock<std::mutex> _f(fs_lock);
                if (logModularFlags & TIME)
                    out_stream << std::put_time(std::localtime(&wall_time), "%Y-%m-%d %H:%M:%S") << "\t";

                if (logModularFlags & AVG)
                    out_stream << std::fixed << std::setprecision(6) << acc->avg() << "\t";

                if (logModularFlags & DEV)
                    out_stream << std::fixed << std::setprecision(6) << acc->sd() << "\t";

                if (logModularFlags & SEM)
                    out_stream << std::fixed << std::setprecision(6) << acc->mr() << "\t";

                out_stream << std::endl;
            }
        }

        while(std::chrono::high_resolution_clock::now() - start < avg_period){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    {
        std::unique_lock<std::mutex> _l(worker_lock);
        worker->detach();
        worker = nullptr;
    }
}

/*
 * starts worker
 */
void filesystemLogManager::startLogging(std::chrono::milliseconds _period, std::function<std::unique_ptr<avgAccumulator>()> acc, std::function<void(ErrorID, std::optional<std::string>)> ecb, modularFlags modular) {
    if(isLogging)
        return;

    dirty = true;
    callback_error = std::move(ecb);
    avg_period = _period;
    logModularFlags = modular;
    accumulator = std::move(acc);

    {
        std::unique_lock<std::mutex> _l(worker_lock);
        isLogging = true;
        worker = std::make_unique<std::thread>(&filesystemLogManager::task, this);
    }
}

/*
 * stops worker and flushes output stream
 */
void filesystemLogManager::stopLogging() {
    if(!isLogging)
        return;

    isLogging = false;

    {
        std::unique_lock<std::mutex> _l(fs_lock);
        out_stream.flush();
    }
}

/*
 * enqueues a fresh measure locking the queue as little time as possible
 */
void filesystemLogManager::handle_measure(umrrMessage& msg) {
    if(!isLogging)
        return;

    timedMsg m;
    m.t = std::chrono::high_resolution_clock::now();
    m.m = msg;

    if(msg.altitude_valid){
        std::unique_lock<std::mutex> _d(deque_lock);
        out_queue.push_back(m);
    }
}

bool filesystemLogManager::getisLogging() {
    return isLogging;
}

bool filesystemLogManager::getisOpen() {
    std::unique_lock<std::mutex> _l(fs_lock);

    return out_stream.is_open();
}

bool filesystemLogManager::getisDirty(){
    return dirty;
}

std::filesystem::path filesystemLogManager::getOutPath() {
    std::unique_lock<std::mutex> _l(fs_lock);

    return out_path;
}