#include "demos.h"
#define DEMO(id) demo##id::demo()

int main() {
	DEMO(2);
	return 0;
}