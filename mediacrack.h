#define NO_THREADS 3

typedef struct {
	unsigned long id;
	int status;
	char current[128];
	size_t len;
	pthread_t thread;
} mc_thread;

typedef struct {
	// char md5[33];
	uint8_t md5[MD5_DIGEST_LENGTH];
	char salt[9];
	unsigned long long tries;
} mc_password;

mc_thread *threads;

