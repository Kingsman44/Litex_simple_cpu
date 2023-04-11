#include <stdio.h>

void addn(int arr[]);
void subn(int arr[]);
void muln(int arr[]);
void divn(int arr[]);

void addn(int arr[]) {
  int sum=0;
  for(int i=1;i<=arr[0];i++) {
	sum+=arr[i];
  }
  printf("%d\n",sum);
}

void muln(int arr[]) {
  int mul=1;
  for(int i=1;i<=arr[0];i++) {
        mul*=arr[i];
  }
  printf("%d\n",mul);
}

void subn(int arr[]) {
  int sub=arr[1];
  for(int i=2;i<=arr[0];i++) {
        sub=sub-arr[i];
  }
  printf("%d\n",sub);
}

void divn(int arr[]) {
  float div=arr[1];
  for(int i=2;i<=arr[0];i++) {
        div=div/(float) arr[i];
  }
  printf("%f\n",div);
}
