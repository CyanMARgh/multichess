#include "demos.h"
#define DEMO(id) demo##id();
int main() {
	DEMO(1);
	return 0;
}