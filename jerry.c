#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>

void print_help();
unsigned int name2uid(char* username);

int main(int argc, char * argv[])
{
	int fd = open("/proc/mousehole", O_RDWR);
	if(fd < 0){	printf("\nmousehole no detected..\n"); return -1;}
	
	printf("Welcome to jerry!\n");
	print_help();
	char buf[256];
	char mode;
	char username[128] = {0,};
	char filename[128] = {0,};
	
	while(1){
		fd = open("/proc/mousehole", O_RDWR);
		printf("> ");
		scanf(" %[^\n]s", buf);
		sscanf(buf, " %c %s %s", &mode, username, filename);
		if(mode == 'h') {print_help();}
		else if(mode == 's'){
			read(fd, buf, sizeof(buf)-1);
			printf("currnt status\n%s", buf);
		}
		else if(mode == 'b' && username[0] != 0 && filename[0] != 0 && name2uid(username) != 0){
			sprintf(buf, "%d %u %s ", 0, name2uid(username), filename);
			write(fd, buf, strlen(buf));
		}
		else if(mode == 'u' && username[0] != 0 && filename[0] != 0 && name2uid(username) != 0){
			sprintf(buf, "%d %u %s ", 1, name2uid(username), filename);
			write(fd, buf, strlen(buf));
		}
		else if(mode == 'p' && username[0] != 0 && name2uid(username) != 0){
			sprintf(buf, "%d %u %s ", 2, name2uid(username), "");
			write(fd, buf, strlen(buf));
		}
		else if(mode == 'r' && username[0] != 0 && name2uid(username) != 0){
			sprintf(buf, "%d %u %s ", 3, name2uid(username), "");
			write(fd, buf, strlen(buf));
		}
		else if(mode == 'q'){break;}
		else{printf("Wrong command format\n");}
		close(fd);
	}


	return 0;
}



void print_help()
{
	printf("Usage: h \t\t\thelp\n");
	printf("Usage: s \t\t\tstate\n");
	printf("Usage: b username filename\tBlock file opening of user\n");
	printf("Usage: u username filename\tUnblock file opening of user\n");
	printf("Usage: p username \t\tPrevent killing of processes of user\n");
	printf("Usage: r username \t\tEnable killing of processes of user\n");
	printf("Usage: q \t\t\tquit\n");
}

unsigned int name2uid(char * username)
{
	struct passwd* user;
	if(user = getpwnam(username))
		return (unsigned int)user->pw_uid;
	else{
		printf("That user not exist\n");
		return 0;
	}
}
