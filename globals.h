#pragma once

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
	if (interfaceToRelease != nullptr) {
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}

enum class Commands 
{
	START = 0,
	BODY,
	ATTENTION,
	TRACKED,
	ENGAGED,
	LEFT_HAND,
	RIGHT_HAND,
	CLOSE,
	OPEN,
	UNKNOWN_STATE,
	END
};

Q_DECLARE_METATYPE(Commands);