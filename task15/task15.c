#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define NR_SYSCALL 332
#define HI_EUDYPTULA 0x2a86
#define LO_EUDYPTULA 0xd024c0e5

int main()
{
	long int ret = 0;
	ret = syscall(NR_SYSCALL,HI_EUDYPTULA, LO_EUDYPTULA);
	printf("System call sys_eudyptula returned %ld\n", ret);

	ret = syscall(NR_SYSCALL,0xAAAA, 0xAAAAAAAA);
	printf("System call sys_eudyptula returned %ld\n", ret);

	return 0;
}
