#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void print_directory_contents(const char *path, int depth,
                              long long int *total_size);

int main(int argc, char *argv[]) {
  const char *directory_path;
  long long int total_size = 0;
  if (argc == 1) {
    directory_path = ".";
  } else if (argc == 2) {
    directory_path = argv[1];
  } else {
    fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Directory %s\n", directory_path);
  print_directory_contents(directory_path, 1, &total_size);

  printf("\nTotal file space used: %lld\n", total_size);
  return 0;
}

void print_directory_contents(const char *path, int depth,
                              long long int *total_size) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char *prefix =
      malloc(sizeof(char) * (depth + 3)); // 2 spaces + null terminator
  memset(prefix, ' ', depth * 2);
  prefix[depth * 2] = '\0';
  if (!(dir = opendir(path))) {
    fprintf(stderr, "Error opening %s\n", path);
    return;
  }
  while ((entry = readdir(dir)) != NULL) {
    char *fullpath = malloc(strlen(path) + strlen(entry->d_name) + 2);
    sprintf(fullpath, "%s/%s", path, entry->d_name);
    if (lstat(fullpath, &statbuf) == -1) {
      fprintf(stderr, "Error reading file %s\n", fullpath);
      continue;
    }
    if (S_ISDIR(statbuf.st_mode)) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      printf("%s%s\n", prefix, fullpath);
      print_directory_contents(fullpath, depth + 1, total_size);
    } else {
      printf("%s%lld:%s\n", prefix, statbuf.st_size, entry->d_name);
      *total_size += statbuf.st_size;
    }
    free(fullpath);
  }
  closedir(dir);
  free(prefix);
}
