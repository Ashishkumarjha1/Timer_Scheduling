#include "Timer.hpp"

void Timer::init(InitializationType type) {
    if (status == 1) {
        std::cerr << " initialized Successfully" << std::endl;
        return;
    }
      status = 1;

    if (type == InitializationType::THREAD) {
        timerThread = std::thread(&Timer::timerFunction, this);
        std::cout<<"Timer started successfully"<<std::endl;
    }
    // Add support for PROCESS initialization if needed
    
    return;
  
}

int Timer::start(TimerType type, const TimeDuration& interval, CallbackFunction callback) {
    std::lock_guard<std::mutex> lock(listMutex); 
    if (status != 1) {
        std::cerr << "Timer is not initialized." << std::endl;
        return -1;
    }

    auto intervalDuration = convertToDuration(interval);
    std::cout<<intervalDuration.count()<<std::endl;

    if (intervalDuration.count() == 0) {
        std::cerr << "Invalid time interval." << std::endl;
        return -1;
    }

    TimePoint currentTime = std::chrono::system_clock::now();
    TimePoint stopTime = currentTime + intervalDuration;
    
    

    int newId = id++;
    

    //Add the timer to list
    timer_list.push_back(std::make_tuple(newId,type,currentTime,stopTime,Type::DURATION,callback));

    for (const auto& timer : timer_list) {
            TimePoint stopTimes = std::get<3>(timer);
            
            if (stopTime<stopTimes) {
                
                shouldWakeUp=true;
                 
                cv.notify_one();
                break;
            }
        }
    return newId;
}

int Timer::start(TimerType type, const std::string& specificTime, CallbackFunction callback) {
    
    
    std::lock_guard<std::mutex> lock(listMutex); 
    if (status != 1) {
        std::cerr << "Timer is not initialized." << std::endl;
        return -1;
    }
    
    TimePoint currentTime = std::chrono::system_clock::now();
    auto stopTime = parseSpecificTime(specificTime);
    if (stopTime == std::chrono::system_clock::time_point{}) {
        std::cerr << "Error parsing specific time." << std::endl;
        return -1;
    }
    
    int newId = id++;
    //Add the timer to list
    timer_list.push_back(std::make_tuple(newId,type,currentTime,stopTime,Type::SPECIFIC_TIME,callback));
    
    for (const auto& timer : timer_list) {
            TimePoint stopTimes = std::get<3>(timer);
            
            if (stopTime<stopTimes) {
                shouldWakeUp=true;
                cv.notify_one();
                break;
            }
        }
    return newId;

}





bool Timer::stop(int timerId) {
    std::unique_lock<std::mutex> lock(listMutex);
    if (status != 1) {
        std::cerr << "Timer is not initialized." << std::endl;
        return false;
    }
    shouldWakeUp=true;
    cv.notify_one();

    for (auto it=timer_list.begin();it!=timer_list.end();it++) {
        
        if (std::get<0>(*it)== timerId) {
            it = timer_list.erase(it);
            return true;
        }
    }

    std::cerr << "Timer with ID " << timerId << " not found." << std::endl;
    return false;
}


// It stops the seprate running thread
bool Timer::deinit() {
    if (status != 1) {
        std::cerr << "Timer is not initialized." << std::endl;
        return false;
    }

    //timerThread.join();
    status = 0;
    return true;
}



//This function takes the time in string and convert it to timepoint
Timer::TimePoint Timer::parseSpecificTime(const std::string& specificTime) {
    std::tm tm = {};
    std::istringstream ss(specificTime);
    std::cout << "Parsing specific time: " << specificTime << std::endl;

    int hours, minutes, seconds;
    char delimiter;
    ss >> hours >> std::noskipws >> delimiter >> minutes >> delimiter >> seconds;

    if (ss.fail() || delimiter != ':' || hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
        std::cerr << "Error parsing specific time: " << specificTime << std::endl;
        return {};
    }

    // Get the current time
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm = *std::localtime(&now); // Set tm to the current local time

    // Set the specified time components
    tm.tm_hour = hours;
    tm.tm_min = minutes;
    tm.tm_sec = seconds;

    // Convert tm to a time_point
    auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return time;
}


//This function take input in a TimeDuration fomrat and returns duration in seconds
std::chrono::seconds Timer::convertToDuration(const TimeDuration& interval) {
    return std::chrono::hours(interval.hours) + std::chrono::minutes(interval.minutes) + std::chrono::seconds(interval.seconds);
}



//This function is getting called when the timer expires
void Timer::performAction() {
    TimePoint currentTime = std::chrono::system_clock::now();
    time_t stw=std::chrono::system_clock::to_time_t(currentTime);
    std::cout << "Performing action..."<< ctime(&stw)<< std::endl;
}



//This function is running in separate thread to monitor list of timers
void Timer::timerFunction() {
    
    while (status == 1) {
        std::unique_lock<std::mutex> lock(listMutex); //Acquires lock
        
        if(!timer_list.empty())
        {
        
            for (auto timer = timer_list.begin(); timer != timer_list.end();){
                TimePoint currentTime = std::chrono::system_clock::now();
                TimePoint stopTime = std::get<3>(*timer);
                

                if (currentTime >= stopTime) { //for a particular timer check that currenttime>=stoptime
                    performAction();
                    TimerType timerType = std::get<1>(*timer);
                    Type type = std::get<4>(*timer);

                    CallbackFunction callback = std::get<5>(*timer);

                    if (callback != nullptr) {
                        callback(std::get<0>(*timer), 0, nullptr);
                    }

                    if (timerType == TimerType::ONESHOT) { //If timertype is ONESHOT delete that timer from list
                        timer = timer_list.erase(timer);
                        continue;
                    } else if (timerType == TimerType::PERIODIC && type == Type::DURATION) {//If it is periodic  and Duration then update the start and stop time of that timer
                        TimePoint originalStartTime = std::get<2>(*timer);
                        TimePoint originalStopTime =std::get<3>(*timer);
                        auto difference = originalStopTime - originalStartTime;
            

                        std::get<2>(*timer) = currentTime;
                        
                        TimePoint newEndTime = currentTime + difference;
                        std::get<3>(*timer) = newEndTime;
                    } else if (timerType == TimerType::PERIODIC && type == Type::SPECIFIC_TIME) {//If it is periodic  and Specific then update the start and stop time of that timer

                        std::get<2>(*timer) = currentTime;
                        TimePoint startTime = currentTime;
                        TimePoint endTime = startTime + std::chrono::hours(24);// Add 24hrs
                        std::get<3>(*timer) = endTime;
                    }
                }
                ++timer;
            }
              // Initialize minStopTime with a large value
              TimePoint minStopTime = std::chrono::system_clock::time_point::max();

                // Iterate over the timer_list
                for (const auto& timer : timer_list) {
                    TimePoint stopTime = std::get<3>(timer);
                    

                    // Update minStopTime if a smaller stop time is found
                    if (stopTime < minStopTime) {
                        minStopTime = stopTime;
                    }
                }

            // Calculate the time difference between minStopTime and the current time
                    TimePoint currentTime = std::chrono::system_clock::now();
                    auto timeDifference = minStopTime - currentTime;
                    // Convert the duration to seconds
                    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeDifference);

                    // Print the time difference in seconds
                    //std::cout << "2.)))) Waiting for " << seconds.count() << " seconds" << std::endl;
                    
    
                    // Wait for the calculated time difference or until shouldWakeUp becomes true
                    cv.wait_for(lock, timeDifference, [this] { return shouldWakeUp; });
                    shouldWakeUp=false;
                    
        }     
    
    }
    return;
}
