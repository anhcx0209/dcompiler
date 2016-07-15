import std.io;

void dosomething1(int x, int y, int z);
void dosomething2(float x, float y);

int main() {
	int x;

	switch (x) {
		case 1:
			dosomething1(4, 5, 6);
			break;			
		case 2:
			dosomething2(2.3, 4.7);
			break;
		default:
			break;
	}

	return 1;
}