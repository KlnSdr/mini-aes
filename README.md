a simple implementation of the mini-aes encryption algorithm
---

### usage
- set your message and key in the code file (reading from stdin will be added later)
- run `make`
- run `./target/miniaes.bin <options>`

### command line options
- `-v` verbose mode to print interim results
- `-i <int>` number of iterations to run the encryption algorithm (maximum is currently 3)
- `-m <string>` the message as a string of hex values (f.ex. `9C63F`). passing an empty message or a message not matching `([0-9][a-z][A-Z])+` will result in the termination of the programm
