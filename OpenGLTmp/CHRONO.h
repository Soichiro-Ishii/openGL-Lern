#pragma once
#include<chrono>
#include<iostream>
using namespace std::chrono;

class CHRONO
{
	system_clock::time_point start, end;
public:
	time_t timeStart();
	time_t timeEnd();
	double getElapsed();
};