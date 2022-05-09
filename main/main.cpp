#include "demos.h"
#define DEMO(id) demo##id::Demo()

int main() {
	DEMO(6);
	return 0;
}