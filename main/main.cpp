#include "demos.h"
#define DEMO(id) demo##id::Demo()

int main() {
	DEMO(0);
	DEMO(1);
	DEMO(2);
	DEMO(3);
	DEMO(4);
	DEMO(5);
	DEMO(6);
	return 0;
}