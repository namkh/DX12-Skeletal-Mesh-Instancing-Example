#pragma once

#include "Singleton.h"

class GlobalSystemValues : public TSingleton<GlobalSystemValues>
{
public:
	GlobalSystemValues(token) {};

public:

	//TODO:
	bool LoadFromFile() {};
	bool SaveFromFile() {};
	bool Reload() {};

public:

	uint32_t ScreenWidth		= 1280;
	uint32_t ScreenHeight		= 720;
	float FovAngleY				= 0.785398163375f;
	float ViewportNearDistance	= 1.0f;
	float ViewportFarDistance	= 1000000.0f;
};