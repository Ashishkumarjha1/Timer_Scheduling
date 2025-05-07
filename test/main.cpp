#include "Timer.hpp"
#include<iostream>

void sampleCallback(int id, int reason, void* userdata) {
    std::cout << "Callback function called for Timer ID: " << id << std::endl;
    std::cout << "Reason: " << reason << std::endl;
    if (userdata != nullptr) {
        std::cout << "Received data: " << *static_cast<std::string*>(userdata) << std::endl;
    }
}

int main() {
    // Create a Timer object
    Timer timer;

    // Initialize the timer (assuming THREAD initialization)
    timer.init(InitializationType::THREAD);

    // Example: Start a periodic timer with a duration of 1 hour
    TimeDuration durationInterval{0, 2, 0};
    // CallbackFunction callback1 = nullptr; // Adjust the callback function as needed
    // int timerId1 = timer.start(TimerType::ONESHOT, durationInterval, sampleCallback);
    // if (timerId1 != -1) {
    //     std::cout << "Timer started with ID: " << timerId1 << std::endl;
    // } else {
    //     std::cerr << "Failed to start timer." << std::endl;
    // }

    // // Example: Start a one-shot timer with a specific stop time
    std::string specificTime = "18:55:23";
    // std::cout<<"$%^^"<<std::endl;
    //CallbackFunction callback2 = nullptr; // Adjust the callback function as needed
    int timerId2 = timer.start(TimerType::PERIODIC, specificTime, sampleCallback);
    if (timerId2 != -1) {
        std::cout << "Timer started with ID: " << timerId2 << std::endl;
    } else {
        std::cerr << "Failed to start timer." << std::endl;
    }
    std::cout<<"$%^^"<<std::endl;


    std::this_thread::sleep_for(std::chrono::seconds(10));
    durationInterval={0, 1, 0};
    int timerId3 = timer.start(TimerType::PERIODIC, durationInterval, sampleCallback);
    if (timerId3 != -1) {
        std::cout << "Timer started with ID: " << timerId3 << std::endl;
    } else {
        std::cerr << "Failed to start timer." << std::endl;
    }
     std::this_thread::sleep_for(std::chrono::seconds(30));

    durationInterval={0, 3, 0};
    int timerId4 = timer.start(TimerType::PERIODIC, durationInterval, sampleCallback);
    if (timerId4 != -1) {
        std::cout << "Timer started with ID: " << timerId4 << std::endl;
    } else {
        std::cerr << "Failed to start timer." << std::endl;
    }

    // Do other work here...

    // Stop the timer after a certain duration (for demonstration purpose)
    std::this_thread::sleep_for(std::chrono::seconds(10));
    //std::cout<<"ash"<<std::endl;
    bool t=timer.stop(1);
    //std::cout<<"dd"<<t;

    std::this_thread::sleep_for(std::chrono::seconds(70));
    
    // Deinitialize the timer
    if (timer.deinit()) {
        std::cout << "Timer deinitialized successfully." << std::endl;
    } else {
        std::cerr << "Failed to deinitialize timer." << std::endl;
    }
    //std::cout<<"qwer"<<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(700));

    return 0;
}
