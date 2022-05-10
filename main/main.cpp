#include "demos.h"
#define DEMO(id) demo##id::Demo()

int main() {
	DEMO(3);
	return 0;
}