import std.io;

void dosomething1(int x, int y, int z);
void dosomething2(float x, float y);

int main() {

	for (int i = 0; i < 10; i++) {
		dosomething1(3, 2, 1.8);
	}

	return 1;
}