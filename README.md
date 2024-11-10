a simple implementation of the mini-aes encryption algorithm
---

### usage
- set your message and key in the code file (reading from stdin will be added later)
- run `make`
- run `./target/miniaes.bin <options>`

### command line options
- `-v` verbose mode to print interim results
- `-i <int>` number of iterations to run the encryption algorithm (maximum is currently 3)
