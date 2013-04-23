#include"sse.hpp"
/*int main(){

 float a[16],b[16],*c;
 int i;
 for(i=0;i<16;i++){
 b[i]=1;
 a[i]=1;
 printf("\n%f",a[i]);
 }

 c=sse_add(a,b,16);
 for(i=0;i<16;i++){
 printf("%f\n",c[i]);
 }
 return 0;
 }
 */

double* sse_add(double *a, double *b, int size) {

	double *c;
	c = new double[size];
	for (int i = 0; i < (size / 2) * 2; i += 2) {
		__m128d sse_a = _mm_load_pd(&a[i]);
		__m128d sse_b = _mm_load_pd(&b[i]);
		__m128d sse_c = _mm_add_pd(sse_a, sse_b);
		_mm_store_pd(&c[i], sse_c);
	}
	for (int i = (size / 2) * 2; i < size; i++) {
		c[i] = a[i] + b[i];
	}
	return c;
}

double* sse_sub(double *a, double *b, int size) {
	double *c;
	c = new double[size];
	for (int i = 0; i < (size / 2) * 2; i += 2) {
		__m128d sse_a = _mm_load_pd(&a[i]);
		__m128d sse_b = _mm_load_pd(&b[i]);
		__m128d sse_c = _mm_sub_pd(sse_a, sse_b);
		_mm_store_pd(&c[i], sse_c);

	}
	for (int i = (size / 2) * 2; i < size; i++) {
		c[i] = a[i] - b[i];
	}
	return c;
}

double* sse_mul(double*a, double*b, int size) {
	double *c;
	c = new double[size];
	for (int i = 0; i < (size / 2) * 2; i += 2) {
		__m128d sse_a = _mm_load_pd(&a[i]);
		__m128d sse_b = _mm_load_pd(&b[i]);
		__m128d sse_c = _mm_mul_pd(sse_a, sse_b);
		_mm_store_pd(&c[i], sse_c);

	}
	for (int i = (size / 2) * 2; i < size; i++) {
		c[i] = a[i] * b[i];
	}
	return c;
}

double* sse_div(double*a, double*b, int size) {
	double *c;
	c = new double[size];
	for (int i = 0; i < (size / 2) * 2; i++) {
		__m128d sse_a = _mm_load_pd(&a[i]);
		__m128d sse_b = _mm_load_pd(&b[i]);
		__m128d sse_c = _mm_div_pd(sse_a, sse_b);
		_mm_store_pd(&c[i], sse_c);

	}
	for (int i = (size / 2) * 2; i < size; i++) {
		c[i] = a[i] * b[i];
	}
	return c;

}

