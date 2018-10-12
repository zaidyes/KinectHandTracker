#pragma once

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
	if (interfaceToRelease != nullptr) {
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}