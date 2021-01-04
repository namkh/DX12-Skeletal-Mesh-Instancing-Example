#pragma once

#include <memory>

template <typename T>
class TSingleton
{
protected:
	struct token {};
	TSingleton() {}
public:
	static T& Instance();

	TSingleton(const TSingleton&) = delete;
	TSingleton& operator=(const TSingleton&) = delete;
};

template <typename T>
T& TSingleton<T>::Instance()
{
	static const std::unique_ptr<T> instance{ new T{token{}} };
	return *instance;
};


