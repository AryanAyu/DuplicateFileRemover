#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include<stdlib.h>

int streaks[1024][2]; // a dictionary mapping integers to their actual streak values
// maps streak values only until 1023
// streak value of 0 and 1 are not defined

typedef struct node { // a linked list of filenames
	char *name;
	struct node *next;
} node;

typedef struct lst { // a struct to map a val to the files having that (sum of streak) value
	int val;
	struct node * start; // contains the root node of linked list with all filenames having (sum of streak val) = val
} lst;

lst list[50]; // a list of structures whick store filenames having (sum of streak values) = the indices
// max is 49 because the (sum of streak values) is always lesser than 20 (as observed). Used 50 as a cushion

void push(struct node** head_ref, char *new_data) // simple linked list push function
{
	struct node* new_node = (struct node*) malloc(sizeof(struct node));
	new_node->name  = strdup(new_data);
	if (*head_ref == NULL)
	{
		new_node->next = NULL;
		*head_ref = new_node;
		return;
	}
	new_node->next = (*head_ref);
	(*head_ref)    = new_node;
}

void printlist(struct node *node) // simple linked list print function
{
	while (node != NULL)
	{
		printf(" %s\n", node->name);
		node = node->next;
	}
}

void printlistsame(struct node *node) // simple linked list print function
{
	int i = 0;
	while (node != NULL)
	{
		i++;
		if (i == 1) printf(" %s\t", node->name);
		if (((node->next) == NULL) || (strcmp((node->next)->name, node->name) != 0)) {
			printf("%d\n", i);
			i = 0;
		}
		node = node->next;
	}
}

void checksum(struct node *node, struct node **same) // function to evaluate checksum of filenames with same sum of streak values
{
	int x;
	struct node *node1 = malloc(sizeof(struct node));
	if (node->next != NULL) { // if it is not the only file haing that (sum of streak) value
		// comparing each filename with every other filename in that linked list
		while (node != NULL) {
			node1 = node->next;
			while (node1 != NULL) {
				x = strcmp(node->name, node1->name);
				if (x == 0) push(same, node->name); // if checksum success, push it to linked list holding all possible duplicated filenames
				node1 = node1->next;
			}
			node = node->next;
		}
	}
}

int streak(int n) { // function to find streak value of given number
	n += 1;
	int streak = 1;
	int i = 2;
	while (1) {
		if (n % i == 0) {
			/* code */
			streak += 1;
			i++;
			n++;
		}
		else
			break;
	}
	return streak;
}

// Using djb2 hash function
//hash(i) = hash(i - 1) * 33 ^ str[i];
unsigned long int sfold(char *str) // function to find hash value of given string
{
	// Using djb2 hash function
	//hash(i) = hash(i - 1) * 33 ^ str[i];
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

void printdir(char *dir, int depth) { // function to traverse and list all files in the given directory and its subdirectories
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	int spaces = depth * 4; // indentation to be left before printing the filename.
	int hash, strkval;

	if ((dp = opendir(dir)) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}
	chdir(dir);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf); // getting details into stat struct
		if (S_ISDIR(statbuf.st_mode)) {
			// ignoring "." and ".."
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue;
			hash = (sfold(entry->d_name)) % 1024;
			//printf("%*s%s/\n",spaces,"",entry->d_name); // commented print
			if (hash < 2) { // bcoz streak value of 0 and 1 are not defined
				//printf("%*s%s\thash:%d\tUniq sos val:%d\n",spaces,"",entry->d_name,hash,0); // commented print
			}
			else {
				// finding (sum of streak) values of all relevant fields unique to a file.
				// considered st_mtime too because an edited copy of a file can no longer be considered a copy
				strkval = streaks[strlen(entry->d_name)][1] + streaks[statbuf.st_size % 1024][1] + streaks[hash][1] + streaks[statbuf.st_mode % 1024][1] + streaks[statbuf.st_mtime % 1024][1];
				push(&(list[strkval].start), entry->d_name);
				//printf("%*s%s\thash:%d\tUniq sos val:%d\n",spaces,"",entry->d_name,hash,strkval); // commented print
			}
			// Recurse at a new indent level
			printdir(entry->d_name, depth + 1);
		}
		else {
			//printf("%*s%s\n",spaces,"",entry->d_name);
			hash = (sfold(entry->d_name) + statbuf.st_size) % 1024;
			if (hash < 2) { // bcoz streak value of 0 and 1 are not defined
				//printf("%*s%s\thash:%d\tUniq sos val:%d\n",spaces,"",entry->d_name,hash,0); // commented print
			}
			else {
				// finding (sum of streak) values of all relevant fields unique to a file.
				// considered st_mtime too because an edited copy of a file can no longer be considered a copy
				strkval = streaks[strlen(entry->d_name)][1] + streaks[statbuf.st_size % 1024][1] + streaks[hash][1] + streaks[statbuf.st_mode % 1024][1] + streaks[statbuf.st_mtime % 1024][1];
				push(&(list[strkval].start), entry->d_name);
				//printf("%*s%s\thash:%d\tUniq sos val:%d\n",spaces,"",entry->d_name,hash,strkval); // commented print
			}
		}

	}
	chdir("..");
	closedir(dp);
}

// Now we move onto the main function
int main(int argc, char* argv[])
{
	node *same = NULL; // linked list that'll contain the filenames with duplicates
	for (int i = 0; i < 50; i++) {
		list[i].val = i;
		list[i].start = NULL;
	}
	// we use this 2d array "streaks" to find the streak values, thereby reducing computations
	// initializing it
	for (int i = 2; i < 1024; i++) {
		streaks[i][0] = i;
		streaks[i][1] = streak(i);
	}
	char *topdir, pwd[2] = ".";
	if (argc != 2) // if no arg given with ./a.out, consider the present working dir
		topdir = pwd;
	else // else, consider the given directory
		topdir = argv[1];

	printf("Directory scan of %s\nplease wait...\n", topdir);
	printdir(topdir, 0); // the most important function
	printf("done.\n\n");
	for (int i = 0; i < 50; i++) {
		if (list[i].start != NULL) { // atleast one file with (sum of streak) value = list[i].val (i.e., i itself)
			printf("With val=%d\n", i);
			printlist(list[i].start); // listing all filenames with (sum of streak) value = i
			checksum(list[i].start, &same); // applying checksum to make sure the files are actual duplicates
		}
	}
	printf("\nPossible files with duplicates:\n");
	printf("Summing up the number of duplicates for each file (easier here):\n");
	printlistsame(same); // printing the filenames having duplicates
	return 0;
}

