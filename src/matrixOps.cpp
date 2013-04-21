#include <iostream>

int* matrixMult(int* a, int *b, int row1, int column1, int column2) {
	int *c = new int[row1 * column2];

	for (int i = 0; i < row1; i++) {
		for (int j = 0; j < column2; j++) {
			int sum = 0;
			for (int k = 0; k < column1; k++) {
				sum += *(a + i * row1 + j) * (*(b + i * column1 + column2));
			}
			*(c + i * row1 + column2) = sum;
		}
	}
	return c;
}
