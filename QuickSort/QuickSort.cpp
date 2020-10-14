#include <iostream>
#include <iterator>
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <ctime>
#include <thread>
#include <atomic>
#include <utility>
#include <mutex>
#include "Log.h"
#include "Timer.h"




const int numCores = std::thread::hardware_concurrency() >= 2 ? std::thread::hardware_concurrency() - 1 : 0;		// количество свободных потоков железа
std::mutex mut;



int nearDegreeTwo(int number)
{
	int i = 0;
	while (numCores >= std::pow(2, i))
	{
		++i;
	}
	--i;
	return std::pow(2,i);
}







template <typename BiIterator, typename Compare>
void QuickSort(BiIterator first, BiIterator last, Compare cmp)
{
	if (first != last)
	{
		BiIterator left = first;
		BiIterator right = last;
		BiIterator middle = left++;
		while (left != right)
		{
			if (cmp(*left, *middle))
			{
				++left;
			}
			else
			{
				while ((left != --right) and cmp(*middle, *right));
				std::iter_swap(left, right);
			}
		}
		--left;
		std::iter_swap(first, left);

		QuickSort(first, left, cmp);
		QuickSort(right, last, cmp);
	}
}

template <typename BiIterator>
void QuickSort(BiIterator first, BiIterator last)
{
	QuickSort(first, last, std::less_equal< typename std::iterator_traits< BiIterator >::value_type >());
}







template <typename BiIterator, typename Compare>
std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> QuickSortSinglePass(BiIterator first, BiIterator last, Compare cmp)
{
	if (first != last)
	{
		BiIterator left = first;
		BiIterator right = last;
		BiIterator middle = left++;
		while (left != right)
		{
			if (cmp(*left, *middle))
			{
				++left;
			}
			else
			{
				while ((left != --right) and cmp(*middle, *right));
				std::iter_swap(left, right);
			}
		}
		--left;
		std::iter_swap(first, left);

		std::pair<BiIterator, BiIterator> p1(first, left), p2(right, last);
		std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> p(p1, p2);


		//BiIterator temp1 = first, temp2 = last;
		//for (; temp1 != temp2; ++temp1)
		//{
		//	std::cout << *temp1 << " ";
		//}
		//std::cout << std::endl << std::endl;



		return p;
	}
	else
	{
		std::pair<BiIterator, BiIterator> p1(first, last);
		std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> p(p1, p1);
		return p;
	}
	
}

template <typename BiIterator>
std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> QuickSortSinglePass(BiIterator first, BiIterator last)
{
	QuickSortSinglePass(first, last, std::less_equal< typename std::iterator_traits< BiIterator >::value_type >());
}







template <typename BiIterator, typename Compare >
void QuickSortMP(BiIterator first, BiIterator last, Compare cmp)
{
	if (numCores == 0)
	{
		QuickSort(first, last, cmp);
	}

	int nearDegree = nearDegreeTwo(numCores);
	std::pair<BiIterator, BiIterator> temp;
	std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> p;
	std::queue<std::pair<BiIterator, BiIterator>> pairForSort;			// содержит пары итераторов, на которые вызовется сортировка
	pairForSort.push(std::pair<BiIterator, BiIterator>(first, last));

	std::queue<std::thread> threads;
	std::unique_lock<std::mutex> l(mut, std::defer_lock);


	auto funForThreadSinglePass = [&pairForSort, &cmp, &l]()
	{
		std::pair<std::pair<BiIterator, BiIterator>, std::pair<BiIterator, BiIterator>> pForThreads;
		std::pair<BiIterator, BiIterator> tempForThreads;

		l.lock();
		tempForThreads = pairForSort.front();
		pairForSort.pop();
		l.unlock();

		pForThreads = QuickSortSinglePass(tempForThreads.first, tempForThreads.second, cmp);

		l.lock();
		if (pForThreads.first != pForThreads.second)
		{
			pairForSort.push(pForThreads.first);
			pairForSort.push(pForThreads.second);
		}
		l.unlock();
	};


	while (pairForSort.size() < numCores)
	{
		temp = pairForSort.front();
		pairForSort.pop();

		int k = pairForSort.size();
		for (int i = 0; ((i < k) and (i < numCores - k)); ++i)
		{
			threads.push(std::thread(funForThreadSinglePass));
		}

		p = QuickSortSinglePass(temp.first, temp.second, cmp);

		l.lock();
		if (p.first != p.second)
		{
			pairForSort.push(p.first);
			pairForSort.push(p.second);
		}
		l.unlock();

		while (threads.size() != 0)
		{
			threads.front().join();
			threads.pop();
		}

	}

	


	while (pairForSort.size() != 1)
	{
		temp = pairForSort.front();
		pairForSort.pop();
		threads.push(std::thread(QuickSort<BiIterator, Compare>, temp.first, temp.second, cmp));
	}
	temp = pairForSort.front();
	pairForSort.pop();
	QuickSort(temp.first, temp.second, cmp);

	while (threads.size() != 0)
	{
		threads.front().join();
		threads.pop();
	}


}


template <typename BiIterator>
void QuickSortMP(BiIterator first, BiIterator last)
{
	QuickSortMP(first, last, std::less_equal< typename std::iterator_traits< BiIterator >::value_type >());
}






int main()
{
	std::vector<int> v;
	std::deque<int> d;
	std::list<int> l;

	Timer t;
	//Log& log = Log::createLog();
	srand(time(0));
	const int n = 10000000;
	setlocale(LC_ALL, "rus");


	for (int i = 0; i < n; ++i)
	{
		d.push_back(rand() % (RAND_MAX - RAND_MAX / 2) * 1024 + rand() % 1024);
		v.push_back(rand() % (RAND_MAX - RAND_MAX / 2) * 1024 + rand() % 1024);
		l.push_back(rand() % (RAND_MAX - RAND_MAX / 2) * 1024 + rand() % 1024);
	}


	t.StartTimer();
	QuickSortMP(v.begin(), v.end());
	std::cout << "Размер вектора: " << n << " время выполнения: " << t.StopTimer() << '\n';

	//for (auto i = v.begin(); i != v.end(); ++i) std::cout << *i << " ";
	//std::cout << std::endl;


	t.StartTimer();
	QuickSortMP(d.begin(), d.end());
	std::cout << "Размер дека: " << n << " время выполнения: " << t.StopTimer() << '\n';

	//for (auto i = d.begin(); i != d.end(); ++i) std::cout << *i << " ";
	//std::cout << std::endl;

	t.StartTimer();
	QuickSortMP(l.begin(), l.end());
	std::cout << "Размер двусвязного списка: " << n << " время выполнения: " << t.StopTimer() << '\n';

	//for (auto i = l.begin(); i != l.end(); ++i) std::cout << *i << " ";
	//std::cout << std::endl;



	//log.deleteLog();
}
