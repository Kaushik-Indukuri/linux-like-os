#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "file_system.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
				printf("%d", i);

			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/* IDT Test - Extended
 * 
 * Asserts that 10-30IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test_custom(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 10; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
				printf("%d", i);

			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}

/* Div by 0 
 * 
 * Divides int by 0 to see if exception raised
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT exception
 * Files: x86_desc.h/S, idt.c/h, exception_linkage.h/S
 */
int idt_0_test() {
	TEST_HEADER;
	int i = 0;
	int j = 10/i;
	return j ? PASS : FAIL;

}

int syscall_test() {
	TEST_HEADER;
	asm("int $0x80");
	return FAIL;

}

/* Page Fault Pass
 * 
 * Test all pointer at bounds of page breaks. Should All pass
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging bounds set up
 * Files: paging.c/h, pagingasm.S
 */
int paging_test() {
	TEST_HEADER;
	char res;
	char* ptr = (char*)0x400000;
	res = *ptr;
	ptr = (char*)0xb8000;
	res = *ptr;
	ptr = (char*)0x7FFFFF;
	res = *ptr;
	ptr = (char*)0xb8FFF;
	res = *ptr;

	return PASS;

}



/* Page Fault Fail1
 * 
 * Test pointer that should fail
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging bounds set up
 * Files: paging.c/h, pagingasm.S
 */
int paging_test_f1() {
	TEST_HEADER;
	char res;
	char* ptr = (char*)0xb7FFF;
	res = *ptr;
	return FAIL;
}
int paging_test_f2() {
	TEST_HEADER;
	char res;
	char* ptr = (char*)0x3FFFFF;
	res = *ptr;
	return FAIL;
}



// add more tests here

/* Checkpoint 2 tests */

int file_system_test_1() {
	//TEST_HEADER;
	int fd = file_open((uint8_t *)"frame0.txt");
	char buf[185];
	int ret = file_read(fd, buf, 185);
	if (ret <= 0) {
		return FAIL;
	}
	clear();
	printf("%s", buf);

	return PASS;
}

int file_system_test_2() {
	dentry_t dentry;
	int ret = read_dentry_by_name ((uint8_t *)"frame0.txt", &dentry);
	if (ret != 0) {
		return FAIL;
	}
	if (strncmp((int8_t *)"frame0.txt", (int8_t*)(dentry.filename), 32) == 0) {
		return PASS;
	}
	return PASS;
}

int file_system_test_3() {
	char buf[185];
	dentry_t dentry;
	int ret = read_dentry_by_name ((uint8_t *)"frame0.txt", &dentry);
	ret = read_data (dentry.inode_num, 0, (uint8_t *)buf, 185);
	if (ret <= 0) {
		return FAIL;
	}
	clear();
	printf("%s", buf);
	return PASS;
}

int file_system_test_4() {
	//TEST_HEADER;
	int fd = file_open((uint8_t *)"frame1.txt");
	char buf[185];
	int ret = file_read(fd, buf, 185);
	if (ret <= 0) {
		return FAIL;
	}
	clear();
	printf("%s", buf);

	return PASS;
}

int file_system_test_5() {
	//TEST_HEADER;
	int fd = file_open((uint8_t *)"grep");
	char buf[185];
	int ret = file_read(fd, buf, 185);
	if (ret <= 0) {
		return FAIL;
	}
	clear();
	printf("%s", buf);

	return PASS;
}

int file_system_test_5() {
	//TEST_HEADER;
	int fd = file_open((uint8_t *)"grep");
	char buf[185];
	int ret = file_read(fd, buf, 185);
	if (ret <= 0) {
		return FAIL;
	}
	clear();
	printf("%s", buf);

	return PASS;
}

int file_system_test_6() {
	//TEST_HEADER;
	char buf[32];
	directory_read(int32_t fd, buf, 32)
	return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());

	// launch your tests here
	// TEST_OUTPUT("syscall_test", syscall_test());
	// TEST_OUTPUT("idt_0_test", idt_0_test());
	//  TEST_OUTPUT("paging_test", paging_test());
	//TEST_OUTPUT("paging_test_f1", paging_test_f1());
	//  TEST_OUTPUT("paging_test_f2", paging_test_f2());
	//NOT WORKING:
	// TEST_OUTPUT("idt_test_custom", idt_test_custom());


	//TEST_OUTPUT("file_system_test_1", file_system_test_1());
	//TEST_OUTPUT("file_system_test_2", file_system_test_2());
	//TEST_OUTPUT("file_system_test_3", file_system_test_3());
	TEST_OUTPUT("file_system_test_5", file_system_test_5());

}
