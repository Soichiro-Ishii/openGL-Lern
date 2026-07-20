#pragma once

class CHRONO
{
	double m_start, m_end;
public:
	double timeStart();
	double timeEnd();
	double getElapsed();
};