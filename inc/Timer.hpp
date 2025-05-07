#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <list>
#include<locale>
#include<iomanip>
#include <map>
#include <variant>
#include <chrono>
#include <thread>
#include <sstream> 
#include <ctime>
#include <mutex>
#include <condition_variable>

/**
 * @brief Enum representing the type of timer.
 */
enum TimerType {
    PERIODIC, 
    ONESHOT   
};

/**
 * @brief Enum representing the type of time specification.
 */
enum Type {
    DURATION,        
    SPECIFIC_TIME    
};

/**
 * @brief Enum representing the type in which timer should start
*/
enum InitializationType{
    THREAD,
    PROCESS
};

/**
 * @brief Struct representing a time duration.
 */
struct TimeDuration {
    int hours;  
    int minutes;  
    int seconds;  
};

/**
 * @brief Callback function type definition.
 * @param[in] timerid,reason,userdata
 */
typedef void (*CallbackFunction)(int timerid,int reason,void *userdata);

/**
 * @brief Class representing a Timer.
 */
class Timer {
public:
    /**
     * @brief Initializes the timer with a specific type.
     * @param[in] type Initialization type: THREAD or PROCESS.
     */
    void init(InitializationType type);

    

    /**
     * @brief Starts a timer with a duration specified by TimeDuration.
     * @param[in] type Type of timer.
     * @param[in] interval Time duration.
     * @param[in] callback Callback function to be executed upon timer completion.
     * @return ID of the started timer if successful, -1 otherwise.
     */
    int start(TimerType type, const TimeDuration& interval,CallbackFunction callback);

    /**
     * @brief Starts a timer with a specific stop time.
     * @param[in] type Type of timer.
     * @param[in] specificTime Specific stop time in format "HH:MM:SS".
     * @param[in] callback Callback function to be executed upon timer completion.
     * @return ID of the started timer if successful, -1 otherwise.
     */
    int start(TimerType type, const std::string& specificTime, CallbackFunction callback);



     /**
     * @brief Stops a timer with the given ID.
     * @param[in] timerId ID of the timer to be stopped.
     * @return True if the timer was successfully stopped, false otherwise.
     */
    bool stop(int timerId);

      /**
     * @brief Stops all timers and cleans up resources.
     * @return True or False
     */
    bool deinit();

private:
    using TimePoint = std::chrono::system_clock::time_point;


    std::list<std::tuple<int,TimerType,TimePoint,TimePoint,Type,CallbackFunction>> timer_list; /**< List of active timers. */
    std::mutex listMutex;  /**< Mutex to synchronize access to timer_list. */
    std::condition_variable cv;  /**< Condition variable for waiting on timer events. */

    int id = 0;/**< ID counter for timers. */

    int status=0;  /**<To check wheather it is already initialized or not*/

    bool shouldWakeUp=false;

    

    std::thread timerThread; /**< Thread for timer execution. */

    /**
     * @brief Parses a specific time string and returns a time point.
     * @param[in] specificTime Specific time string in format "HH:MM:SS".
     * @return Time point.
     */
    TimePoint parseSpecificTime(const std::string& specificTime);

    /**
     * @brief Converts a TimeDuration struct to std::chrono::seconds.
     * @param[in] interval Time duration.
     * @return Duration in seconds.
     */
    std::chrono::seconds convertToDuration(const TimeDuration& interval);

    /**
     * @brief Performs an action.
     */
    void performAction();



       /**
     * @brief Timer function that runs in a separate thread.
     *
     * This function continuously checks the timers and performs actions when necessary.
     * If the timer list is not empty, it iterates over the timers and checks if the current time
     * is greater than or equal to the stop time. If so, it performs the action associated with
     * the timer. If the timer is one-shot, it calls the stop function to remove the timer.
     * If the timer is periodic, it updates the start and stop time in the list and database.
     * If the database is not empty, it updates the list from the database.
     * It then sleeps until the earliest stop time among the timers.
     */
    void timerFunction();

    
};

#endif // TIMER_HPP
