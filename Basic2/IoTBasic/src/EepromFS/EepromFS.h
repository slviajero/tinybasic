/* this version of the library used the code from the BASIC runtime environment 
it cannot be used standalone and will be included in the toolkit for small systems */
#include <Arduino.h>

#define EFS_HEADERSIZE 4
#define EFS_FILENAMELENGTH 12
#define EFS_FILEHEADERSIZE (EFS_FILENAMELENGTH+2+2)
#define EFS_PAGESIZE 16
#define EFSWRITEDELAY 10

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

	// error flag not used, error handling goes through the runtime 
	// environments error flag ioer
	// uint8_t ferror;

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


// lazy constructor - has the size as argument
EepromFS::EepromFS(uint8_t e, unsigned long sz) {
	eepromsize=sz;
	eepromaddr=e;
	pagenumber=-1;
	pagechanged=false;
};

// even lazier constructor - no size
EepromFS::EepromFS(uint8_t e) {
	eepromsize=0;
	eepromaddr=e;
	pagenumber=-1;
	pagechanged=false;
};

uint8_t EepromFS::readbyte(unsigned int a) {

	// clear the error flag
	ioer=0;

	// send the address
    wirestart(eepromaddr, 0);
    wirewritebyte((int)a/256);
    wirewritebyte((int)a%256);
    ioer=wirestop();

	// get a byte if there was no error
    if (ioer) return 0;
	wirestart(eepromaddr, 1);
    return wirereadbyte();
};

void EepromFS::writebyte(unsigned int a, uint8_t v) {

	// clear the error flag
	ioer=0;

	// send the address and the byte
    wirestart(eepromaddr, 0);
    wirewritebyte((int)a/256);
    wirewritebyte((int)a%256);
    wirewritebyte((int)v);
    ioer=wirestop();

	// typical update time of one cell in the EEPROM
    bdelay(5);
};


// the raw sequential read method 
uint8_t EepromFS::rawread(unsigned int a){

	// clear the error flag
	ioer=0;

	// the new page needs to be loaded
	int p=a/EFS_PAGESIZE;
	if (p != pagenumber) {
		// on page fault flush the page before reloading
		rawflush();
		// load the page at the address pa
		unsigned int pa=p*EFS_PAGESIZE;

		/* send the address */
		wirestart(eepromaddr, 0);
		wirewritebyte((int)pa/256);
		wirewritebyte((int)pa%256);
		ioer=wirestop();

		if (ioer) return 0;

		wirestart(eepromaddr, EFS_PAGESIZE);
        // readbyte sets the runtime environments ioer flag
        for (uint8_t i=0; i<EFS_PAGESIZE; i++) {
			pagebuffer[i]=wirereadbyte();
			if (ioer) return 0;
		}

        pagenumber=p;
        pagechanged=false;
	}
	return pagebuffer[a%EFS_PAGESIZE];
}

// the raw sequential write method - write only works on the buffer - rawflush does the work
void EepromFS::rawwrite(unsigned int a, uint8_t d){
	uint8_t b;
	int p=a/EFS_PAGESIZE;
	if (p != pagenumber) rawflush();
	b=rawread(a);
	if (d == b) return;
	pagebuffer[a%EFS_PAGESIZE]=d;
	pagechanged=true;
}

// the raw flush methods - sets ioer according to transmission status
void EepromFS::rawflush(){

	// clear the error flag
	ioer=0;

	if (pagechanged) {
		unsigned int pa=pagenumber*EFS_PAGESIZE;
        wirestart(eepromaddr, 0);
        wirewritebyte((int)pa/256);
        wirewritebyte((int)pa%256);
        for (uint8_t i=0; i<EFS_PAGESIZE; i++) wirewritebyte(pagebuffer[i]);
        ioer=wirestop();
		bdelay(10); // the write delay according to the AT24x datasheet for an entire block
		pagechanged=false;
	}
}


// begin wants a formated filesystem
uint8_t EepromFS::begin() { 

	// clear the error flag
	ioer=0;

	// find the size this only works for 4096 and 32768 eeproms 
	if (eepromsize == 0) {
		uint8_t c4 = readbyte(4094);
		uint8_t c32 = readbyte(32766);
		writebyte(4094, 42);
    	writebyte(32766, 84);
  		if (ioer !=0 ) return 0;
  		if (readbyte(32766) == 84 && readbyte(4094) == 42) eepromsize = 32767; else eepromsize = 4096;
  		writebyte(4094, c4);
 		writebyte(32766, c32);	
	}

	slotsize=0;
	maxfilesize=0;
	nslots=0;
	if (rawread(0) == 'E') {
		nslots=rawread(1);
		if (nslots>0) slotsize=(eepromsize-EFS_HEADERSIZE)/nslots;
	} 
	if (slotsize>EFS_FILEHEADERSIZE) { maxfilesize=(slotsize - EFS_FILEHEADERSIZE); return nslots; } else return 0;
}

// zero the entire eeprom 
bool EepromFS::format(uint8_t s) {
	if (s>0) nslots=s; else return false;
	for(uint8_t a=0; a<EFS_HEADERSIZE; a++) rawwrite(a, 0);
	rawwrite(0, 'E'); 
	rawwrite(1, s); 
	slotsize=(eepromsize-EFS_HEADERSIZE)/nslots;
	for(uint8_t s=1; s<=nslots; s++) clearslotheader(s);
	if (rawread(0) == 'E') return true; else return false;
}

// open and create a file 
uint8_t EepromFS::fopen(const char* fn, const char* m) {
	if (*m == 'r') {
		if(ifile=findfile(fn)) {
		 	ifilesize=getsize(ifile);
		 	ifilepos=0;
		}
		return ifile;
	}
	if (*m == 'w' || *m == 'a') {
		if (ofile=findfile(fn)) {
			ofilesize=getsize(ofile);
		} else {
			ofile=findemptyslot();
			ofilesize=0;
			if (ofile != 0) {
				putfilename(ofile, fn);
				putsize(ofile, 0);
			}
		}
		if (*m == 'w') ofilepos=0;
		if (*m == 'a') ofilepos=ofilesize;
		return ofile;
	}
	return 0;
}

// close a file - and write the correct filesize 
uint8_t EepromFS::fclose (uint8_t f){
	if (ifile == f) {
		ifile=0;
		ifilepos=0;
		return ifilesize;
	}
	if (ofile == f) {
		ofile=0;
		ofilesize=ofilepos;
		ofilepos=0;
		putsize(f, ofilesize);
		return ofilesize;
	}


	return 0;
}

uint8_t EepromFS::fclose (const char* m){
	if (*m == 'r') return fclose(ifile); 
	if (*m == 'w' || *m == 'a') return fclose(ofile);
	return 0;
}

// end of file on read 
bool EepromFS::eof(uint8_t f){
	return available(f)<=0;
}

// get a character from the file 
uint8_t EepromFS::fgetc(uint8_t f) {
	if (f == ifile && ifilepos<ifilesize) 
		return getdata(ifile, ifilepos++);
	else 
		return -1;
}

// write a character to the file
bool EepromFS::fputc(uint8_t ch, uint8_t f){
	if (f == ofile && ofilepos<maxfilesize-1) {
		putdata(ofile, ofilepos++, ch);
		if (ofilepos>ofilesize) ofilesize++;
		return true;
	} else 
		return false;
}	

// flush the file - putsize always does flush!
bool EepromFS::fflush(uint8_t f){
	if (ofile) putsize(ofile, ofilesize);
	return true;
}	

// rewind the file for read
void EepromFS::rewind(uint8_t f) {
	if (ifile == f) {
		ifilepos=0;
	}
	if (ofile == f) {
		ofilepos=0;
		ofilesize=0;
	}	
}

// increment the dirpointer and read 
uint8_t EepromFS::readdir(){
	while(++dirp <= nslots) {
		if (getfilename(dirp)) return dirp;
	}
	return 0;
}

// the filename
char* EepromFS::filename(uint8_t f){
	if (getfilename(f)) return fnbuffer; 
	return 0;
}

// the filesize
unsigned int EepromFS::filesize(uint8_t f){
	return getsize(f);
}

// remove a file 
uint8_t EepromFS::remove(const char* fn){
	uint8_t file=findfile(fn);
	if (file != 0) {
		for(uint8_t i=0; i<EFS_FILENAMELENGTH; i++) fnbuffer[i]=0;
		putfilename(file, fnbuffer);
		putsize(file, 0);	
	}
	return file;
}

// rename 
uint8_t EepromFS::rename(const char* ofn, const char* nfn){
	if (findfile(nfn)) return 0;
	uint8_t file=findfile(ofn);
	putfilename(file, nfn);
	return file;
}


// find a file by name or find and empty slot
uint8_t EepromFS::findfile(const char* fn) {
	for(uint8_t i=1; i<=nslots; i++) {
		if (getfilename(i)) {
			bool found = true;
			for(uint8_t j=0; j<EFS_FILENAMELENGTH && fn[j] !=0; j++) {
				if (fnbuffer[j] != fn[j] ) {found=false; break; }
			}
			if (found) return i;
		}
	}
	return 0;
}

// find a file by name or find and empty slot
uint8_t EepromFS::findemptyslot() {
	for(uint8_t i=1; i<=nslots; i++) {
		if (!getfilename(i)) return i;
	}
	return 0;
}

// find the slots address
unsigned int EepromFS::findslot(uint8_t s) {
	if (s > 0 && s <= nslots) {
		return EFS_HEADERSIZE + (s-1)*slotsize;
	} else {
		return 0;
	}
}

// get the filename into the local buffer and return the length
uint8_t EepromFS::getfilename(uint8_t s) {
	uint8_t i;
	unsigned int a=findslot(s);
	if (a == 0) return 0;
	for (i=0; i<EFS_FILENAMELENGTH; i++) if ( (fnbuffer[i]=rawread(a+i)) == 0) break;
	fnbuffer[i]=0;
	return i;
}

// put a filename into a slot and pad the data with 0
void EepromFS::putfilename(uint8_t s, const char* fn) {
	uint8_t i;
	unsigned int a=findslot(s);
	if (a == 0) return;
	for(i=0; i<EFS_FILENAMELENGTH && fn[i]!=0; i++) rawwrite(a+i, (uint8_t) fn[i]);
	while(i<EFS_FILENAMELENGTH) rawwrite(a+(i++), 0);
	rawflush();
}

// clear the header of a slot - needed for formating
void EepromFS::clearslotheader(uint8_t s) {
	unsigned int a=findslot(s);
	if (a == 0) return;
	for(uint8_t i=0; i<EFS_FILEHEADERSIZE; i++) rawwrite(a+i, 0); 
	rawflush();
}

// get the size bigendian - flush always
unsigned int EepromFS::getsize(uint8_t s) {
	unsigned int a=findslot(s);
	if (a == 0) return 0;
	return rawread(a+EFS_FILENAMELENGTH+1)+rawread(a+EFS_FILENAMELENGTH+2)*256;
}

// put the size bigendian - flush always
void EepromFS::putsize(uint8_t s, unsigned int sz) {
	unsigned int a=findslot(s);
	if (a == 0) return;
	rawwrite(a+EFS_FILENAMELENGTH+1, sz%256);
	rawwrite(a+EFS_FILENAMELENGTH+2, sz/256);
	rawflush();
}

// access to one byte of data in the slot
uint8_t EepromFS::getdata(uint8_t s, unsigned int i) {
	unsigned int a=findslot(s);
	if (a == 0) return 0;
	if (i>=0 && i<maxfilesize) return rawread(a+EFS_FILEHEADERSIZE+1+i); else return 0;
}

// put one byte of data in a slot
void EepromFS::putdata(uint8_t s, unsigned int i, uint8_t d){
	unsigned int a=findslot(s);
	if (a == 0) return;
	if (i>=0 && i<maxfilesize) rawwrite(a+EFS_FILEHEADERSIZE+1+i, d);
}

// stream class like available, for output it returns the free bytes
int EepromFS::available(uint8_t f) {
	if (f == ifile) {
		return ifilesize-ifilepos;
	}
	if (f == ofile) {
		return maxfilesize-ofilesize;
	} 
	return 0;
}

unsigned int EepromFS::size() {
	return slotsize;
}

unsigned long EepromFS::esize() {
	return eepromsize;
}
