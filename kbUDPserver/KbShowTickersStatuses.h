#pragma once
#include <iostream>
#include <thread>
#include "KbTickerThreadedClass.h"

using namespace std;

class KbShowTickersStatuses
{
public:
	std::thread separate_thread_for_ticker_statuses_update;
	bool stopThreadFlag = true;
	bool* do_not_show_current_time_;
	array<KbTickerThreadedClass, 16> * tickers;

	KbShowTickersStatuses (bool* ptr, array<KbTickerThreadedClass, 16>* t) : do_not_show_current_time_(ptr), tickers(t) {}
};

