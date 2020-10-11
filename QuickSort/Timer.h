#pragma once
#include <chrono>


class Timer
{
private:

	std::chrono::system_clock::time_point t_start;			// момент создания объекта Timer
	std::chrono::system_clock::time_point t_lastStart;


public:

	Timer();
	~Timer();


	void PrintTimeFromStart();		// Выводит в консоль полное время существования объекта Timer 
	void StartTimer();			// Запускать перед участком кода, на котором происходит замер
	double StopTimer();			// Возвращает время выполнения кода с момента последнего вызова StartTimer
};
