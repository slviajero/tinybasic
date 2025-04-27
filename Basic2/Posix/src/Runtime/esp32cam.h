/*
 * the commands and includes needed for the esp32 camera
 * - first set of runtime routines to be encapsulated
 * on POSIx this is a dummy to help with development
 */


extern void outsc(char*);

void camerabegin() {
	outsc("esp32cam: camera begin\n");
}

void cameraget() {
	outsc("esp32cam: camera get\n");
}

void camerasave() {
	outsc("esp32cam: camera save\n");
}

void cameraset() {
	outsc("esp32cam: camera set\n");
}
 
