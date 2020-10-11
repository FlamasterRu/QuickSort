#include "Timer.h"
#include <iostream>

Timer::Timer()
{
    t_start = std::chrono::system_clock::now();
    t_lastStart = t_start;
}


Timer::~Timer()
{
}


void Timer::PrintTimeFromStart()
{
    std::chrono::duration<double> duration = std::chrono::system_clock::now() - t_start;
    std::cout << duration.count() << " sec";
}

void Timer::StartTimer()
{
    t_lastStart = std::chrono::system_clock::now();
}

double Timer::StopTimer()
{
    std::chrono::duration<double> duration = std::chrono::system_clock::now() - t_lastStart;
    return duration.count();
}



