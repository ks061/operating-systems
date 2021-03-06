/*
 * CSCI 315 Operating Systems Design
 * Original developed by CSCI 315 staff
 * Modified by: L. Felipe Perrone
 * Date: 2011-4-21
 * Copyright (c) 2011 Bucknell University
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "Software"), to use, copy, modify, and
 * distribute without restriction, provided that this copyright and
 * permission notice is maintained, intact, in all copies and supporting
 * documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL BUCKNELL UNIVERSITY BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "read_dir.h"

// function prototypes
void process(char *name);
void enqueue(char *name, que_t *q);
void dequeue(que_t *q);
void peek_front(char *name,que_t q);
bool queue_empty(que_t q);
void initq(que_t *q);

// main function
int main(int argc,char *argv[]) {

  if (2 != argc) {
    printf(" usage: %s dirpath\n", argv[0]);
    exit(-1);
  }
  else {
    // pass in the starting directory
    process(argv[1]);       
  }
  
  return 0;
}

// traverse a directory tree applying a function when a file is found
void process(char *root)
{
  int numOfDirectories = 0;
  int numOfFiles = 0;
  int numOfRegFiles = 0;

  int smallestFileSize = -1;
  int largestFileSize = -1;
  double averageFileSize = -1;

  char mostRecentlyModifiedFilename[256];
  char leastRecentlyModifiedFilename[256];
  int mostRecentlyModifiedTime = -1;
  int leastRecentlyModifiedTime = -1;

  que_t nameq;
  char dname[MAXLENGTH];
  char cname[MAXLENGTH];
  char prefix[MAXLENGTH];

  struct dirent *dp;
  DIR *dirp;

    initq(&nameq);
    enqueue(root,&nameq);

    while (true != queue_empty(nameq)) {
      peek_front(dname,nameq); 
      dequeue(&nameq);
      
      dirp = opendir(dname);

      struct stat buf;
      lstat(dname, &buf);

      if (dirp != NULL && !S_ISLNK(buf.st_mode)) { // it is a directory
	strncpy(prefix, dname, MAXLENGTH);
	strncat(prefix,"/", MAXLENGTH);
	
	for (dp = readdir(dirp); NULL != dp; dp = readdir(dirp)) {
	  if ((strcmp(dp->d_name,"..") != 0) && 
	      (strcmp(dp->d_name,".") != 0)) {
	    
	    // prevent from infinite loop
	    strncpy(cname, prefix, MAXLENGTH);
	    
	    // concatenate the prefix
	    strncat(cname, dp->d_name, MAXLENGTH);  
	    enqueue(cname,&nameq);
	  }
	}
	closedir (dirp);
        numOfDirectories++;
      } else { 
	// test if it is a regular file and not a device or link -- TO-DO
        if (S_ISREG(buf.st_mode)) {
            // if this is a regular file, then process it -- TO-DO
	    if (smallestFileSize == -1 || buf.st_size < smallestFileSize)
	    	smallestFileSize = buf.st_size;
	    if (largestFileSize == -1 || buf.st_size > largestFileSize)
		largestFileSize = buf.st_size;
            averageFileSize += buf.st_size;
            
	     if (mostRecentlyModifiedTime == -1 || buf.st_mtime > mostRecentlyModifiedTime) {
	 	mostRecentlyModifiedTime = buf.st_mtime;
		strcpy(mostRecentlyModifiedFilename, dname);
	     }
	     if (leastRecentlyModifiedTime == -1 || buf.st_mtime < leastRecentlyModifiedTime) {
		leastRecentlyModifiedTime = buf.st_mtime;
		strcpy(leastRecentlyModifiedFilename, dname);
	     }
	    numOfRegFiles++;
        }
        printf(" processing file: %s\n", dname);
    	numOfFiles++;
      }      
    } // while
    averageFileSize /= numOfRegFiles;
    printf(" the smallest file size was %d\n", smallestFileSize);
    printf(" the largest file size was %d\n", largestFileSize);
    printf(" the average file size was %f\n", averageFileSize);
    printf(" the total number of directories was %d\n", numOfDirectories);
    printf(" the total number of regular files was %d\n", numOfRegFiles);
    printf(" %s was most recently modified\n", mostRecentlyModifiedFilename);
    printf(" %s was least recently modified\n", leastRecentlyModifiedFilename);

    printf(" a total of %d files were counted\n",numOfFiles);
}

// initialize queue data structure
void initq(que_t *q) {
  q->head = q->tail = NULL;
}

// test whether queue data structure is empty
bool queue_empty(que_t q) {
  if (NULL == q.head) {
     return true;
  } else {
    return false;
  }
}

// add an element to queue
void enqueue(char *name, que_t *q) {
  item_t *temp;
  
  temp = (item_t *)malloc(sizeof(item_t));
  strncpy(temp->name,name,MAXLENGTH);
  temp->next = NULL;
  
  if (true == queue_empty(*q)) {
    q->head = temp;
    q->tail = temp;
  } else {
    q->tail->next = temp;
    q->tail = q->tail->next;
  }
}

// remove an element from the front of the queue
void dequeue(que_t *q) {
  item_t *temp;
  
  if (true == queue_empty(*q)) {
    printf(" error in dequeue \n");
    exit(-1);
  } else {
    temp = q->head;
    q->head = q->head->next;
    free(temp);
  }
}

// find element at front of queue without removing
void peek_front(char *name, que_t q) {
  if (true == queue_empty(q)) {
    printf(" error in dequeue \n");
     exit(-1);
  } else {
    strncpy(name, q.head->name, MAXLENGTH);
  }
} 
