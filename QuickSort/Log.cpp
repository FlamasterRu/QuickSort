#include "Log.h"
#include <ctime>


Log* Log::l_ptr = nullptr;


Log::Log()
{
	l_ptr = this;
	l_file.open("myLog.log", std::ios_base::app);
	if (!l_file.is_open())
	{
		throw("Log can't create");
	}
	else
	{
		char str[26];
		time_t seconds = std::time(0);
		tm timeinfo;
		localtime_s(&timeinfo, &seconds);
		asctime_s(str, sizeof str, &timeinfo);
		l_file << "Log start in " << str;
	}
}



Log::~Log()
{
	if (l_file.is_open())
	{
		l_file << std::endl << "------------------------------------------------------" << std::endl << std::endl;
		l_file.close();
	}
}




Log& Log::createLog()
{
	if (l_ptr == nullptr)
	{
		Log *log = new Log();
		return *log;
	}
	else
	{
		return *l_ptr;
	}
}


void Log::deleteLog()
{
	if (l_ptr != nullptr)
	{
		delete l_ptr;
		l_ptr = nullptr;
	}
}




void Log::writeLog(const std::string str)
{
	l_file << str << std::endl;
}




void Log::clearFile()
{
	l_file.close();
	l_file.open("myLog.log");
	if (!l_file.is_open())
	{
		throw("Log can't create");
	}
}







