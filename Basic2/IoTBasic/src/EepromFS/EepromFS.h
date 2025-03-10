/*
	Library for a basic ""slot based" file system.

	A storage array of a given size is divided into n equal 
	sized slots, each one with a file name. The slot length 
	is the maximum file size. 

	The filesystem can have one open file for read and one open
	file for write/append.

	Filenames are 12 bytes long. 2 bytes length and 2 bytes reserved.
	(default fileheader of 16 bytes)

	Everything is stores as uint8_8.

	One page of 16 bytes if buffered. The pagesize should be less than 
	30 bytes to fit into one I2C package of the wire library.

*/ 

#include <Arduino.h>
#include <Wire.h>

#define EFS_HEADERSIZE 4
#define EFS_FILENAMELENGTH 12
#define EFS_FILEHEADERSIZE (EFS_FILENAMELENGTH+2+2)
#define EFS_PAGESIZE 16
#define EFSWRITEDELAY 10

//typedef signed char uint8_t;

class EepromFS {
public:
/* 
	constructor 
*/
	EepromFS(uint8_t e, unsigned long sz);
	EepromFS(uint8_t e);

/*
	begin method - main purpose is connection to the
	Wire type eeprom
*/
	uint8_t begin();

/*
	formatting the filesystem with s slots
*/

	bool format(uint8_t s);

/*
	POSIX style interface to the filesystem	
*/
	uint8_t fopen(const char* fn, const char* m);
	uint8_t fclose(uint8_t f);
	uint8_t fclose(const char* m);
	bool eof(uint8_t f);
	uint8_t fgetc(uint8_t f);
	bool fputc(uint8_t ch, uint8_t f);
	bool fflush(uint8_t f);
	void rewind(uint8_t f);

/*
	handling the directory
*/
	uint8_t readdir();
	char* filename(uint8_t f);
	unsigned int filesize(uint8_t f);
	uint8_t remove(const char* fn);
	uint8_t rename(const char* ofn, const char* nfn);

/*
	a few more methods
*/
	int available(uint8_t f);

	// directory pointer
	uint8_t dirp;

	// error flag
	uint8_t ferror;

	// raw read and write methods - bypass the filesystem 
	// and just offer buffered eeprom access
	uint8_t rawread(unsigned int a);
	void rawwrite(unsigned int a, uint8_t d);
	void rawflush();

	// slot access interface
	uint8_t getdata(uint8_t s, unsigned int i);
	void putdata(uint8_t s, unsigned int i, uint8_t d);
	unsigned int size();
	unsigned long esize();

private:
	// the eeprom address
	uint8_t eepromaddr;
	unsigned long eepromsize;

	// the helpers for Wire
	uint8_t readbyte(unsigned int a);
	void writebyte(unsigned int a, uint8_t v);

	// headers and files
	char fnbuffer[EFS_FILENAMELENGTH];
	uint8_t pagebuffer[EFS_PAGESIZE];

	// the slot number of the input and output file
	uint8_t ifile;
	uint8_t ofile;

	// the file position
	unsigned int ofilepos;
	unsigned int ifilepos;

	// the actual file size of the open file
	unsigned int ofilesize;
	unsigned int ifilesize;

	// finding files and storage space
	uint8_t findfile(const char* fn);
	uint8_t findemptyslot();
	unsigned int findslot(uint8_t s);
	void clearslotheader(uint8_t s);
	uint8_t getfilename(uint8_t s);
	void putfilename(uint8_t s, const char* fn);
	unsigned int getsize(uint8_t s);
	void putsize(uint8_t s, unsigned int sz);
	
	// number of slots and slotsizes
	uint8_t nslots;
	unsigned int slotsize;
	unsigned int maxfilesize;

	// paging mechanism
	int  pagenumber;
	bool pagechanged;
};