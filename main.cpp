#include "demos.h"
#define DEMO(id) demo##id();
int main() {
	for (int i = 0; i < 1; i++) {
		DEMO(1);
	}
	return 0;
}