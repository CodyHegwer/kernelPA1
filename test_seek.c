#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>

int main(){
	
	/*
	 *First test will test SEEK_SET, by moving
	 *
	 */

	// create user buffer 
	char user_buff1[20];
	char user_buff2[10];
	char read_buff[100] = {'0'};
	char read_buff2[100] = {'0'};

	printf("created buffers...\n");

	// copy string into buffer
	memcpy( user_buff1, "how Do you do iT?", strlen("how Do you do iT?") );
	memcpy( user_buff2, "hello cody", strlen("hello cody") );
	printf("copied strings into buffers...\n");

	// open file
	FILE *fp;
	fp = fopen("/dev/simp_char_dev", "w+");

	// write first buffer into driver buffer
	fwrite(user_buff1, sizeof(char), strlen("how Do you do iT?"), fp);
	printf("successfully wrote first buffer at 0 offset.\n");

	// change file offset location at like 4 idk man lol in the middle of first string
	int new_offset = fseek(fp, 4, SEEK_SET);
	printf("new offset location worked?.\n", new_offset);

	// write second buffer in the middle of the first
	fwrite(user_buff2, sizeof(char), strlen("hello cody"), fp);
	printf("sucessfully wrote second buffer.\n");
	

	// read entire thing into a user_buff
	fseek(fp, 0, SEEK_SET);
	fread(read_buff, 99, 1, fp);
	printf("%s\n", read_buff);
	fclose(fp);

	printf("\n -- First test complete -- \n\n");



	// write after the '?' with the second buffer
	fp = fopen("/dev/simp_char_dev", "w+");
	fseek(fp, 17, SEEK_CUR);
	printf("moved offset 17 back from current position\n");
	fwrite(user_buff2, sizeof(char), strlen("hello cody"), fp);
	printf("successfully wrote second buffer.\n");

	// read entire thing into read_buff
	fseek(fp, 0, SEEK_SET);
	fread(read_buff, 99, 1, fp);
	printf("%s\n", read_buff);
	fclose(fp);

	printf("\n -- Second test complete -- \n\n");

	

	// attempt to write 4 away from end of driver buffer
	// note: will only read the characters at the end of the buffer
	fp = fopen("/dev/simp_char_dev", "w+");
	fseek(fp, 4, SEEK_END);
	printf("moved offset 4 back from end of buffer.\n");
	fwrite(user_buff2, sizeof(char), strlen("hello cody"), fp);
	printf("successfully wrote second buffer at end of driver buffer\n");

	// read last section of driver buffer
	fseek(fp, 4, SEEK_END);
	fread(read_buff2, 99, 1, fp);
	printf("%s\n", read_buff2);
	fclose(fp);

	printf("\n -- Third test complete -- \n\n");

	return 0;
}
