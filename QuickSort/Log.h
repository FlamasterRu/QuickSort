#pragma once
#include <string>
#include <fstream>
#include <iostream>



class Log
{
private:
	std::ofstream l_file;

	static Log* l_ptr;


	Log();
	Log(const Log& a) = delete;
	Log& operator= (const Log& a) = delete;
	~Log();



public:


	static Log& createLog();		// Возвращает ссылку на Лог, который может быть только один
	static void deleteLog();		// В конце программы нужно удалить Лог, иначе он не поставит завершающие символы в файле,
	// но в принципе не страшно и забыть(если программа будет завершена окончательно, а то утечка памяти и открытый файл

	template <typename T>
	friend Log& operator<< (Log& log,const T str);		// Запись в Лог, как в поток

	void writeLog(const std::string str);		// Запись через функцию
	void clearFile();


};



template <typename T>
Log& operator<< (Log& log, const T str)
{
	if (Log::l_ptr != nullptr)
	{
		log.l_file << str;
		return log;
	}
	else
	{
		std::cout << "Log was deleted" << std::endl;
	}
}



