# MediaCrack
MediaCrack is a password revorery tool for [MediaWiki "B" passwords](http://www.mediawiki.org/wiki/Manual_talk:User_table#.22B.22_type_password_.28current_default.29). "B" type passwords consist of the following components:

*	A colon and the capital letter "B", followed by
*	A colon and a pseudo-random eight-digit hex salt, followed by
*	A colon and the MD5 hash of a concatenation of:
	- The eight-digit hex salt,
	- A dash ("-"), and
	- The MD5 hash of the password

## Requirements
*	OpenSSL library
*	POSIX threads

## Usage
To compile, use ```make```.

MediaCrack takes words from standard input and attempts them in order. You can run MediaCack alongside John the Ripper:

```
% john --incremental=alpha --stdout | ./mediacrack :B:838c83e1:e4ab7024509eef084cdabd03d8b2972c
Salt: 838c83e1, MD5: e4ab7024509eef084cdabd03d8b2972c
Success! The password was "password"
```

...or provide an actual wordlist:

```
% cat wordlist | ./mediacrack :B:838c83e1:e4ab7024509eef084cdabd03d8b2972c
Salt: 838c83e1, MD5: e4ab7024509eef084cdabd03d8b2972c
Success! The password was "password"
```

## License
I give permission for you to do whatever you want with this software.

