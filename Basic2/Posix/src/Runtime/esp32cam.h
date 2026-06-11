/*
 * the commands and includes needed for the esp32 camera
 * - first set of runtime routines to be encapsulated
 * on POSIx this is a dummy to help with development
 */


extern void outsc(char*);


void cameraget() {
	outsc("esp32cam: camera get\n");
}

void cameraend() {
	outsc("esp32cam: camera set\n");
}
 
void camerabegin(int8_t framesize, int8_t pixelformat) {
	outsc("esp32cam: camera begin\n");
}

void camerasave(char* filename) {
	outsc("esp32cam: camera save\n");
}

void cameraset(int8_t setting, int8_t value) {
	outsc("esp32cam: camera set\n");
}


int camerafbget(int index) {
	outsc("esp32cam: camera fb get\n");
	return 0;
}

void camerafbset(int index, uint8_t value) {
	outsc("esp32cam: camera fb set\n");
}

