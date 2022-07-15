#include "cleanup.hh"

cleanup::cleanup(std::function<void()> p)
	: f {p}
{}

cleanup::~cleanup() {
	f();
}

optional_cleanup::optional_cleanup(std::function<void()> p)
	: r {true}, f {p}
{}

optional_cleanup::optional_cleanup(std::function<void()> p, bool e)
	: r{e}, f {p}
{}

optional_cleanup::~optional_cleanup() {
	if (r)
		f();
}

void optional_cleanup::enable() {
	r = true;
}

void optional_cleanup::disable() {
	r = false;
}
