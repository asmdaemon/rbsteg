#Steganography with CRB (Choice of random bits in image for hide data, based on key)
This is code not using text AES encryption same some another projects! This project hide data in random bits, based on key.
Not same LSB Steganography, can not reversively get text back from image, if you don`t know a key.

#Installation

make ;
make install

#Usage

#Help

Example output of rbsteg:

Usage: rbsteg [-t filetype] -m encryption | decryption -k encryptionkey -i inputfile [< | > datastream]

Usage example:
	Encrypt data from input.txt in png test.png: rbsteg -m encryption -k cegthgfhjkm -i test.png < input.txt

	Decrypt data from test.png and put output in output.txt: rbsteg -m decryption -k cegthgfhjkm -i test.png > output.txt

Currently supported only PNG file format.

License GPL 2.0

#END