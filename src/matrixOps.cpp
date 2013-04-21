#include <iostream>

int* matrixMult_int(int* a, int *b, int row1, int column1, int column2) {
	int *c = new int[row1 * column2];

	for (int i = 0; i < row1; i++) {
		for (int j = 0; j < column2; j++) {
			int sum = 0;
			for (int k = 0; k < column1; k++) {
				sum += *(a + i * column1 + k) * (*(b + k * column2 + j));
			}
			*(c + i * column2 + j) = sum;
		}
	}
	return c;
}

int* matrixSum_int(int *a, int *b, int row, int column) {
	int *c = new int[row * column];
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			*(c + i * column + j) = *(a + i * column + j)
					+ *(b + i * column + j);
		}
	}
	return c;
}

float* matrixMult_float(float* a, float *b, int row1, int column1,
		int column2) {
	float *c = new float[row1 * column2];

	for (int i = 0; i < row1; i++) {
		for (int j = 0; j < column2; j++) {
			int sum = 0;
			for (int k = 0; k < column1; k++) {
				sum += *(a + i * column1 + k) * (*(b + k * column2 + j));
			}
			*(c + i * column2 + j) = sum;
		}
	}
	return c;
}

float* matrixSum_float(float *a, float *b, int row, int column) {
	float *c = new float[row * column];
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			*(c + i * column + j) = *(a + i * column + j)
					+ *(b + i * column + j);
		}
	}
	return c;
}
