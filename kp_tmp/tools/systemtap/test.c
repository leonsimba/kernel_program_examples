#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int main()
{
	int fd;
	int ret;

	fd = open("/var/tmp/etilqs_xUDkLdUHq0a7j9W", O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW, 0600);
	if (fd < 0) {
		fprintf(stderr,"open failed");
		return -1;
	}

	unlink("/var/tmp/etilqs_xUDkLdUHq0a7j9W");
	//fcntl(fd, F_GETFD);
	//fcntl(fd, F_SETFD, FD_CLOEXEC);

	ret = ftruncate(fd, 0);
	if (ret < 0) {
		perror("ftruncate failed");
	}

	close(fd);
	return 0;
}
