a simple implementation of the mini-aes encryption algorithm
---

### usage
- run `make`
- run `./target/miniaes.bin <options>`

### command line options
- `-v` verbose mode to print interim results
- `-i <int>` number of iterations to run the encryption algorithm (maximum is currently 3)
- `-m <string>` the message as a string of hex values (f.ex. `9C63F`). passing an empty message or a message not matching `([0-9][a-z][A-Z])+` will result in the termination of the programm
- `-k <string>` the key as a string of hex values (f.ex. `9C63F`). passing an empty message or a message not matching `([0-9][a-z][A-Z])+` will result in the termination of the programm. if len(key) < `blkSize` the program will terminate. if len(key) > `blkSize` only the bytes with index < `blkSize` will be used
