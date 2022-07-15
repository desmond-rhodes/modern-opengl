#ifndef CLEANUP_HH
#define CLEANUP_HH

#include <functional>

class cleanup {
public:
	cleanup(std::function<void()>);
	~cleanup();

private:
	std::function<void()> f;
};

class optional_cleanup {
public:
	optional_cleanup(std::function<void()>);
	optional_cleanup(std::function<void()>, bool);
	~optional_cleanup();

	void enable();
	void disable();

private:
	bool r;
	std::function<void()> f;
};

#endif
