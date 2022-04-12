#include "demos.h"
#define DEMO(id) demo##id::demo()

int main() {
	DEMO(1);
	return 0;
}