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

int main() {
	B b;
	auto f = ref(b, &A::f);
	f(3, "");
	//cref(3);
	//cref(B(), &A::f);
	static_assert(std::is_same<
	    decltype(f)::second_argument_type,
	    std::string>::value, "binary_function failed");
}
