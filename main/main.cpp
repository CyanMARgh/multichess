#include "demos.h"
#define DEMO(id) demo##id::Demo()

int main() {
	DEMO(2);
	return 0;
}