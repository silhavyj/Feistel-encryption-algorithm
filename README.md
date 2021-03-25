
# KIV/BIT task 03 - Feistel encryption algorithm

## Compilation

The compilation process is done thorough the `make`command that's supposed to be executed in the root folder of the project structure. Once the process has completed, a file called `feistel` will be generated. This file represents the executable file of the application.

## Execution

### help
```
> ./feistel --help
KIV/BIT task 3 - feistel encryption/decryption
Usage:
  ./feistel <input> <keys> [OPTION...]

  -b, --binary      the input file is a binary file
  -v, --verbose     print out info as the program proceeds
  -o, --output arg  name of the output file (default: output.txt)
  -p, --print       print out the binary data as well as the decrypted text
  -h, --help        print help
>
```
### input
The program takes **two required parameters**, where the first one is a **file containing data to be encrypted** and decrypted afterwards. The second compulsory parameter is a **file containing keys to be used** in the process of encryption/decryption.

### input file
There's no requirement to what the structure of the input file should be. It could be any file, including binary files, such as images, documents, etc. If the input happens to be a binary file, the user needs to specify it using the `-b` option, so the program can treat it accordingly. By default, the program assumes the input file is a text file.
```
./feistel dwarf_small.bmp keys.txt -b
```
### keys
The number of keys is not limited in any way. However, holding all the keys needs to have the following structure:
```
k0=0101
k1=0111
k2=0000
```
Since 8 bits are considered as one block of data, the key itself must be a number (in a binary format) less than 16. If the file doesn't have this structure, an error will be printed out on the screen. The total number of keys determines the number of iterating through the Feistel network.

### output
The output of the program is represented by `output.txt`, which contains 3 lines in total. The first line contains the encrypted data in a hexadecimal format. The second line is the data after decryption, also in a hexadecimal format. The last line differs by the type of input file. If a text file has been used, the third line will hold the encrypted ASCII text, which should match the original file. In the case of a binary file, the third line displays a note referring to another binary file, which has been generated after decryption. This file has a prefix of  `feistel_` and the rest matches the name of the input file.
