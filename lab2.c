/*
	.------------------------------------------------------------------------------------.
	| ЛАБОРАТОРНАЯ РАБОТА №2							    							 |
	| ВАРИАНТ №6									    								 |
	|------------------------------------------------------------------------------------|
	| Написать программу, находящую в заданном каталоге и всех его подкаталогах все      |
	| файлы заданного размера. Имя каталога задаетсётся пользователем в качестве первого |
	| аргумента командной строки. Диапазон от N1 до N2 задаётся в аргументах командной.  |
	| Программа выводит результаты поиска в файл(четвёртый аргумент командной строки) в  |
	| виде полный путь, имя файла, его размер(в байтах). На консоль выводится общее      |
	| число просмотренных файлов.							     						 |
	`------------------------------------------------------------------------------------`
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

int count = 0;
void ParseDir(const char *dirName, const int N1, const int N2, FILE *F, const char *exeName);

int main(int argc, char *argv[], char *envp[])
{
	char dirName[PATH_MAX];
	if (argv[1][0] == '/')
		for (int i = 0; i < strlen(argv[1]); i++)
			dirName[i] = argv[1][i];
	else
	{
		getcwd(dirName, sizeof(dirName));
		strcat(dirName, "/");
		strcat(dirName, argv[1]);
	}
	const int N1 = atoi(argv[2]);
	const int N2 = atoi(argv[3]);
	const char *fileName = argv[4];

	FILE *F = fopen(fileName, "w");
	if (F == NULL)
	{
		perror(fileName);
		fclose(F);
		return 1;
	}

	ParseDir(dirName, N1, N2, F, basename(argv[0]));
	fclose(F);
	printf("%d\n", count);
}

void ParseDir(const char *dirName, const int N1, const int N2, FILE *F, const char *exeName)
{
	DIR *dir = opendir(dirName);
	struct stat st;
	lstat(dirName, &st);
	if ((S_ISDIR(st.st_mode)) && (dir == NULL))
	{
		char *err = malloc(strlen(exeName) + strlen(dirName) + 2);
		strcpy(err, exeName);
		strcat(err, " ");
		strcat(err, dirName);
		perror(err);
		free(err);
		closedir(dir);
		return;
	}
	else if (!S_ISDIR(st.st_mode))
		return;

	struct dirent *de;
	while (de = readdir(dir))
	{
		char *fullPath = malloc(strlen(dirName) + strlen(de->d_name) + 2);
		strcpy(fullPath, dirName);
		strcat(fullPath, "/");
		strcat(fullPath, de->d_name);
		struct stat buf;
		stat(fullPath, &buf);
		if ((S_ISDIR(buf.st_mode)) && (strcmp(de->d_name, ".")) && (strcmp(de->d_name, "..")))
			ParseDir(fullPath, N1, N2, F, exeName);
		else if ((!S_ISDIR(buf.st_mode)) && (buf.st_size > N1 ) && (buf.st_size < N2))
		{
			fprintf(F, "%s %ld\n", fullPath, buf.st_size);
			count++;
		}
		free(fullPath);
	}
	closedir(dir);
}