#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <dirent.h>

int main () {
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( "foo2" , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */
  FILE *file;
	file=fopen("foo1_trans","wb");
	int data= fwrite(buffer,1, lSize,file);
  // terminate
	fclose (file);
  fclose (pFile);
  free (buffer);
  // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
  size_t i = 0, j;
  size_t size = 1;
  char *names , **tmp;
  DIR *directory;
  struct dirent *dir;

  names = malloc(size * sizeof *names); //Start with 1

  directory = opendir(".");
  if (!directory) { puts("opendir failed"); exit(1); }

  while ((dir = readdir(directory)) != NULL) {
     names[i]=strdup(dir->d_name);
     if(!names[i]) { puts("strdup failed."); exit(1); }
     i++;
     if (i>=size) { // Double the number of pointers
        tmp = realloc(names, size*2*sizeof *names );
        if(!tmp) { puts("realloc failed."); exit(1); }
        else { names = tmp; size*=2;  }
     }
  }

  for ( j=0 ; j<i; j++)
  printf("Entry %zu: %s\n", j+1, names[j]);

  closedir(directory);
      
    return 0;
}
