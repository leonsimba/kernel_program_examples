#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>     //Definition of uint64_t
#include <sys/eventfd.h>

#define SYS_FILE 	"/sys/kernel/my_eventfd"

int main(int argc, char *argv[])
{
	int efd, ret = 0;
	uint64_t efd_count;
	FILE *fp;

	//Create eventfd
	efd = eventfd(0,0);
    	if (efd == -1){
        	printf("\nUnable to create eventfd! Exiting...\n");
        	exit(EXIT_FAILURE);
    	}

	fp = fopen(SYS_FILE, "r+");
	if (!fp) {
		ret = -errno;
		return ret;	
	}

	fprintf(fp, "%d", efd);
	fflush(fp);

	/* read eventfd */
        ret = read(efd, &efd_count, sizeof(uint64_t));
        if (ret != sizeof(uint64_t)){
		printf("eventfd read error\n");
        } else {
		ret = 0;
		printf("eventfd count = %lld\n", efd_count);
	}

    	close(efd);
    	fclose(fp);
    	return ret;
}
