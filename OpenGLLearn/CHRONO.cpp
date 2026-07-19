#include "CHRONO.h"
#include<chrono>
#include<iostream>

using namespace std::chrono;

time_t CHRONO::timeStart() {
	start = system_clock::now();
	return system_clock::to_time_t(start);
}
time_t CHRONO::timeEnd() {
	end = system_clock::now();
	return system_clock::to_time_t(end);
}
double CHRONO::getElapsed() {
	double milliSec = static_cast<double>(duration_cast<milliseconds>(end - start).count());
	return milliSec / 1000;
}
