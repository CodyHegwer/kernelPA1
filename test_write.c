#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>

int main(){
	

	// create user buffer to store user string
	char user_buff[10];
	printf("created buffer...\n");

	// copy string into buffer
	memcpy( user_buff, "hello cody", strlen("hello cody"));
	printf("copied into buffer...\n");
	
	// open file
	FILE *fp;
	fp = fopen("/dev/simp_char_dev", "w");
	printf("opened device file...\n");

	// write to file
	fwrite(user_buff, sizeof(char), strlen("hello cody"), fp);
	printf("successfully wrote to file...\n");

	// close file
	fclose(fp);
	printf("closed file.\n");

	return 0;	

}
