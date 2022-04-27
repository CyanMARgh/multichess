#include "demos.h"
#define DEMO(id) demo##id::Demo()

int main() {
	DEMO(5);
	return 0;
}