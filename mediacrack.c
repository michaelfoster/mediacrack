#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <openssl/md5.h>
#include <pthread.h>
#include <unistd.h>

#include "mediacrack.h"

mc_password target;

void make_hash(uint8_t *digest, char *password, size_t len) {
	MD5_CTX c;
	char buffer[41];
	
	memcpy(buffer, target.salt, 8);
	buffer[8] = '-';
	
	MD5_Init(&c);
	MD5_Update(&c, password, len);
	MD5_Final(&digest[0], &c);
	
	// add first md5
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		buffer[9 + i * 2] = "0123456789abcdef"[(digest[i] / 16) % 16];
		buffer[10 + i * 2] = "0123456789abcdef"[digest[i] % 16];
	}
	
	buffer[41] = 0;
	
	MD5_Init(&c);
	MD5_Update(&c, buffer, 41);
	MD5_Final(&digest[0], &c);
}

void *worker(void *_self) {
	mc_thread *self = (mc_thread*)_self;
	
	uint8_t hash[MD5_DIGEST_LENGTH];
	while(1) {
		if(self->status == 0)
			continue; // idle
		
		make_hash(hash, self->current, self->len);
		
		if(memcmp(hash, target.md5, 16) == 0) {
			printf("Success! The password was \"%s\".\n", self->current);
			exit(0);
		}
		
		self->status = 0;
		
		target.tries++;
	}
	
	return NULL;
}

void *status() {
	unsigned long long previous = 0;
	while(1) {
		sleep(3);
		printf("Current: %lld\t\t%lld/s\n",  target.tries, (target.tries - previous) / 3);
		previous = target.tries;
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: cat <wordlist> | mediacrack <password>\n");
		return 1;
	}
	
	char *pass = argv[1];
	if(strlen(pass) != 44 || pass[0] != ':' || pass[1] != 'B' || pass[2] != ':' || pass[11] != ':') {
		fprintf(stderr, "Invalid password: %s\n", pass);
		return 1;
	}
	
	memcpy(target.salt, pass + 3, 8);
	
	target.salt[8] = 0;
	target.md5[32] = 0;
	
	char *junk;
	for(unsigned int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		target.md5[i] = strtol(junk = strndup((pass + 12) + i * 2, 2), NULL, 16);
		free(junk);
	}
	
	printf("Salt: %s, MD5: %s\n", target.salt, pass + 12);
	
	// start threads
	threads = malloc(sizeof(mc_thread) * NO_THREADS);
	for(unsigned int i = 0; i < NO_THREADS; i++) {
		threads[i].status = 0;
		threads[i].id = i;
		
		if(pthread_create(&threads[i].thread, NULL, worker, (void*)&threads[i]) != 0) {
			fprintf(stderr, "Error creating thread %d!\n", i);
			exit(1);
		}
	}
	
	pthread_t status_t;
	pthread_create(&status_t, NULL, status, NULL);
	
	target.tries = 0;
	
	char try[128];
	
	int thread = -1;
	while(fgets(try, 128, stdin) != NULL) {
		size_t len = strlen(try) - 1;
		
		// remove trailing \n
		try[len] = 0;
		
		while(1) {
			thread++;
			if(thread == NO_THREADS)
				thread = 0;
			
			// check if thread is busy
			if(threads[thread].status == 1)
				continue;
			
			// copy current string
			memcpy(threads[thread].current, try, len);
			threads[thread].len = len;
			
			// tell the thread it has data waiting
			threads[thread].status = 1;
			
			break;
		}
	}
	
	printf("Could not find the password!\n");
	
	return 1;
}

