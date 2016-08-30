#include <linux/unistd.h> 
#include <linux/types.h> 
#include <linux/sysctl.h> 
#include <stdio.h> 

#define SIZE(x) sizeof(x)/sizeof(x[0]) 
#define OSNAMESZ 100 
#define OSRELEASESZ 256 

_syscall1(int, _sysctl, struct __sysctl_args *, args); 
int sysctl(int *name, int nlen, void *oldval, size_t *oldlenp, 
void *newval, size_t newlen) 
{ 
struct __sysctl_args args={name,nlen,oldval,oldlenp,newval,newlen}; 
return _sysctl(&args); 
} 


static char osname[OSNAMESZ]; 
static char osrelease[OSRELEASESZ]; 
int osnamelth; 
int name[] = { CTL_KERN, KERN_OSTYPE }; 
int osreleaseth; 
int release[] = { CTL_KERN, KERN_OSRELEASE }; 

int main(){ 
	osnamelth = sizeof(osname); 

	if (sysctl(name, SIZE(name), osname, &osnamelth, 0, 0)){ 
		perror("sysctl"); 
		return -1; 
	} 

	osreleaseth = sizeof(osrelease); 
	if(sysctl(release, SIZE(release), osrelease, &osreleaseth, 0, 0)){ 
		perror("sysctl"); 
		return -1; 
	} 
	printf("This machine is running %s %s\n", osname, osrelease); 
	return 0; 
} 
