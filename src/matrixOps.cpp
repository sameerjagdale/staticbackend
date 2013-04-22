#include "matrixOps.hpp"
using namespace std;
long* matrixMult_long(long* a, long *b, int row1, int column1, int column2) {
	long *c = new long[row1 * column2];

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

long* matrixSum_long(long *a, long *b, int row, int column) {
	long *c = new long[row * column];
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			*(c + i * column + j) = *(a + i * column + j)
					+ *(b + i * column + j);
		}
	}
	return c;
}

double* matrixMult_double(double* a, double *b, int row1, int column1,
		int column2) {
	double *c = new double[row1 * column2];

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

double* matrixSum_double(double *a, double *b, int row, int column) {
	double *c = new double[row * column];
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			*(c + i * column + j) = *(a + i * column + j)
					+ *(b + i * column + j);
		}
	}
	return c;
}
