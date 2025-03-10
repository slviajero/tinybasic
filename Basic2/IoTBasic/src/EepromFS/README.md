# EepromFS


Filesystem like interface to an EEPROM module. Intended for the larger clock EEPROMS like the 4kB module in many real time clocks and the 32kB AT24C256 modules. 

The library was originally written for my IoT BASIC interpreter for microcontrollers (https://github.com/slviajero/tinybasic). 

The filesystem is extremely simple. It partitions the EEPROM into n slots of identical size. A file can be maximally as big as its slot. There is only one (root) directory. Only one file for read and one file for write/append can be open at the same time.

## Filesystem API

The API to the filesystem is POSIX style. The following public methods are implemented:

	bool format(uint8_t s);
	uint8_t fopen(char* fn, char* m);
	uint8_t fclose(uint8_t f);
	uint8_t fclose(char* m);
	bool eof(uint8_t f);
	uint8_t fgetc(uint8_t f);
	bool fputc(uint8_t ch, uint8_t f);
	bool fflush(uint8_t f);
	void rewind(uint8_t f);
	uint8_t readdir();
	char* filename(uint8_t f);
	unsigned int filesize(uint8_t f);
	uint8_t remove(char* fn);
	uint8_t rename(char* ofn, char* nfn);
	int available(uint8_t f);
  
In addition to this 
  
  	uint8_t rawread(unsigned int a);
	void rawwrite(unsigned int a, uint8_t d);
	void rawflush();
  
can be used for buffered access to individual memory cells much like the EEPROM.read() and EEPROM.update() functions of the Arduino EEPROM library. 

The methods

	uint8_t getdata(uint8_t s, unsigned int i);
	void putdata(uint8_t s, unsigned int i, uint8_t d);
	unsigned int EepromFS::size(); 

are an interface to directly access individual slots. 
  
The filesystem has to be started with the 

	void begin()

method which checks the header and initializes the internal variables. An object 

	EepromEFS MyEFS(0x50, 32768)

has to be declared with the EEPROM I2C address and the size as parameters.

Alternatively 

	EepromEFS MyEFS(0x50)

will contruct and objects that tries to determine the size of the EEPROM automatically.
Currently 4k and 32k EEPROMS are implemented.

Buffer size for EEPROM access is 16 bytes. Maximum filename length is 12 bytes. All ASCII characters are allowed as a file name. 

## POSIX API

The main POSIX style file commands are implemented as methods of the filesystem object (https://www.cplusplus.com/reference/cstdio/). Typical code could look like

	EepromEFS MyEFS(0x50, 32768);
	MyEFS.format(16);
	uint8_t file=MyEFS.fopen("test.txt", "w");
	char ch=MyEFS.fgetc(file);
	MyEFS.fclose(file);

In this example, the format methods partitions the filesystem into 16 slots of approximately 2 kB each. 

File descriptors returned by the fopen() method are unit8_t integers. They are the number of the slot of the open file and can be used like file descriptors in all methods.

Only two files can be open at one time. One file can be open for read and one file can be opened for write/append. Internally they are represented by the file descriptors ofile and ifile and the respective file positions and sizes. 

All API calls work more or less like their POSIX equivalents. Main difference are the available() method and the readdir() mechanism. 

## available()

The available() method returns the number of bytes in a file open for read just like it would do for the stream class of the Arduino IDE. In a file opened for write/append it returns the number of free bytes in the filesystem slot.

## readdir()

As there is only one directory this method is extremely simple. Scanning through a directory is done like this

	MyEFS.dirp=0;
	while (f=myEFS.readdir()) {
		Serial.println(MyEFS.filename(f));
	}

readdir() iterates through all slots with a valid filename, increasing the value of dirp. This variable is public and can be set to any value to remember dir positions. 

## raw interface and buffering

The raw interface accesses individual memory cells of the EEPROM. Writing to the EEPROM with it can destroy the file system. There is no safety net. It is exposed as public because it can be used to access the EEPROM as one large array which I needed for an project this library has been build for. 

Access to the EEPROM always goes through one buffer pagebuffer[16]. The buffer length of 16 is typical minimum value EEPROMs can process in one I2C request. Data sheets indicate larger page sizes for some EEPROMs. This should not be trusted without tests. 

rawread() reads the entire buffer the reqested value can be found in and returns the value. Further reads within the buffer don't access the EEPROM any more. Reading beyond the buffer will cause and automatic reload of the right buffer.

rawwrite() will first check if the right buffer in in memory, flush the buffer if not through rawflush, and then check if the value of the buffer cell has changed. Only changed values are stored and the buffer is flagged for update through rawflush. Multiple writes are cached in the buffer. 

rawflush() will check is the current buffer is changed and flush all 16 bytes to the EEPROM. Only rawflush() does actual EEPROM write access.

Flushing is done by rawwrite() if needed. After the last write access in a program, rawflush() has to be done once directly to make sure that the last bytes written are in the EEPROM. Program which run for a long time without any write should rawflush() from time to time to avoid memory loss. 

## Byte API

The data in each slot s can be accessed through 

	uint8_t getdata(uint8_t s, unsigned int i);
	void putdata(uint8_t s, unsigned int i, uint8_t d);
	unsigned int EepromFS::size();

The integer i addresses the byte in the slot. Reading or writing outside the slotsize is ignored. size() returns the slotsize of the filesystem. This API is like the EEPROM.read/update interface of the internal EEPROM for an individual slot. 

## Timing

EEPROM are very fast on reading and delivering data. The code contains no timeout delays for reading. All the I2C bus timing is done by the Wire library. 

On write EEPROMs have a typical 10 ms delay until they can accept the next start condition on the bus. This is build into the code. In practice this means that only 100 pages of 16 bytes can be written per second. The maximum write baudrate ot the EEPROM is 12800 baud. The delay can be reduced for some EEPROMs. 10 ms is a typical save value. The update baudrate of pages can be higher if no bytes are changed and no page write is needed. 

## Limitations

There is only one buffer for read and write. Hence reading and writing different files randomly and rapidly will cause many page faults and many rawflush() operations. The update delay then limits the read and the write performance. This could be circumvented by using two seperate buffers for read and write. I don't need this for my project, therefore it is not implemented right now. 

## Next steps

I plan to add a random access file interface. 







  
