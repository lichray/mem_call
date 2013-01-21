#include "mem_call_wrapper.h"
#include <iostream>

using namespace stdex;

struct A {
	virtual void f(int, std::string) {
		std::cout << "base\n";
	}
};

struct B : A {
	void f(int, std::string) override {
		std::cout << "derived\n";
	}
};

template <typename F>
void call_f(F f) {
	f(3, "");
}

int main() {
	B b;
	auto f = ref(b, &A::f);
	call_f(f);
	//cref(3);
	//cref(B(), &A::f);
	static_assert(std::is_same<
	    decltype(f)::second_argument_type,
	    std::string>::value, "binary_function failed");
}
