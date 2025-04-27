/*
 * the commands needed for the esp32 camera
 * - first set of runtime routines to be encapsulated
 * on POSIx this is a dummy to help with development
 */

/* the esp 32 camera include for the AI Thinker module 
	and the code to control the pins permanently*/
#include "esp_camera.h"
#include "driver/rtc_io.h"

/* 
 * This is presumably the right setting for AI Thinker.
 * 
 * Unfortunately the module board definition in the Arduino IDE
 * has no -D architecture flag for the specific board. We need to
 * define an identifiere in BASIC later.
 */
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

/* the config structure for the camera */
camera_config_t esp32_camera_config;

/* the camera frame buffer */
camera_fb_t *esp32_camera_fb = NULL;

/* the camera settings */
sensor_t *esp32_camera_sensor = NULL;

/* we set framesize and pixelformat at camera start and all other parameters later */
void camerabegin(int8_t framesize, int8_t pixelformat) {
	if (RTDEBUG) consolelog("esp32cam: camera begin\n");

	/* the camera base settings for single frames as a starting point */
	esp32_camera_config.ledc_channel = LEDC_CHANNEL_0;
	esp32_camera_config.ledc_timer = LEDC_TIMER_0;
	esp32_camera_config.pin_d0 = Y2_GPIO_NUM;
	esp32_camera_config.pin_d1 = Y3_GPIO_NUM;
	esp32_camera_config.pin_d2 = Y4_GPIO_NUM;
	esp32_camera_config.pin_d3 = Y5_GPIO_NUM;
	esp32_camera_config.pin_d4 = Y6_GPIO_NUM;
	esp32_camera_config.pin_d5 = Y7_GPIO_NUM;
	esp32_camera_config.pin_d6 = Y8_GPIO_NUM;
	esp32_camera_config.pin_d7 = Y9_GPIO_NUM;
	esp32_camera_config.pin_xclk = XCLK_GPIO_NUM;
	esp32_camera_config.pin_pclk = PCLK_GPIO_NUM;
	esp32_camera_config.pin_vsync = VSYNC_GPIO_NUM;
	esp32_camera_config.pin_href = HREF_GPIO_NUM;
	esp32_camera_config.pin_sccb_sda = SIOD_GPIO_NUM;
	esp32_camera_config.pin_sccb_scl = SIOC_GPIO_NUM;
	esp32_camera_config.pin_pwdn = PWDN_GPIO_NUM;
	esp32_camera_config.pin_reset = RESET_GPIO_NUM;
	esp32_camera_config.xclk_freq_hz = 20000000;

	/* set the framesize SVGA is the default */
	switch(framesize) {
		case 0: esp32_camera_config.frame_size = FRAMESIZE_QQVGA; break;
		case 1: esp32_camera_config.frame_size = FRAMESIZE_QVGA; break;
		case 2: esp32_camera_config.frame_size = FRAMESIZE_CIF; break;
		case 3: esp32_camera_config.frame_size = FRAMESIZE_VGA; break;
		case 4: esp32_camera_config.frame_size = FRAMESIZE_SVGA; break;
		case 5: esp32_camera_config.frame_size = FRAMESIZE_XGA; break;
		case 6: esp32_camera_config.frame_size = FRAMESIZE_SXGA; break;
		case 7: esp32_camera_config.frame_size = FRAMESIZE_UXGA; break;
		default: esp32_camera_config.frame_size = FRAMESIZE_SVGA; break;
	}

	/* set the pixel format, only JPEG is default */
	switch(pixelformat) {
		case 0: esp32_camera_config.pixel_format = PIXFORMAT_RGB565; break;
		case 1: esp32_camera_config.pixel_format = PIXFORMAT_YUV422; break;
		case 2: esp32_camera_config.pixel_format = PIXFORMAT_GRAYSCALE; break;
		case 3: esp32_camera_config.pixel_format = PIXFORMAT_JPEG; break;
		default: esp32_camera_config.pixel_format = PIXFORMAT_JPEG; break;
	}


	/* set the frame size to SVGA for now */
	if(!psramFound() && framesize > 4) {
		if (RTDEBUG) consolelog("Camera without psram - frame size too large.\n");
		esp32_camera_config.frame_size = FRAMESIZE_SVGA;
		if (RTDEBUG) consolelog("Camera without psram - SVGA set.\n");
	}

	esp32_camera_config.jpeg_quality = 12; //0-63 lower number means higher quality
	if (psramFound()) {
		esp32_camera_config.fb_count = 2; //if more than one, i2s runs in continuous mode.
	} else {
		esp32_camera_config.fb_count = 1; //if only one, i2s runs in snapshot mode.
	}

	/* initialize camera */
	esp_err_t err = esp_camera_init(&esp32_camera_config);
	if (err != ESP_OK) {
		if (RTDEBUG) consolelog("Camera init failed.\n");
		ioer=err;
		return;
	}

	/* the settings object */
	esp32_camera_sensor = esp_camera_sensor_get();

	/* switch off the flash LED and hold it even during sleep */
	// pinMode(4, OUTPUT);
	// digitalWrite(4, LOW);
	// rtc_gpio_hold_en(GPIO_NUM_4);

	/* set the camera to be the default */
	if (RTDEBUG) consolelog("Camera init success.\n");
}

/* 
 * get generates a frame buffer object which we keep until 
 * the next photo is taken.
 */
void cameraget() {
	if (RTDEBUG) ("esp32cam: camera get\n");

	/* if there is a buffer we free it */
	if (esp32_camera_fb) {
		esp_camera_fb_return(esp32_camera_fb);
		esp32_camera_fb = NULL;
	}

	/* get a new frame buffer and take a picture */
	esp32_camera_fb = esp_camera_fb_get(); 
	if (!esp32_camera_fb) {
		if (RTDEBUG) consolelog("Camera capture failed.\n");
		ioer=1;
		return;
	} else {
		if (RTDEBUG) {
			consolelog("Camera capture success. ");
			consolelog("Frame size:");
			consolelognum(esp32_camera_fb->len);
			consolelog("\n");
		}
		ioer=0;
	}
}

/* 
 * Save the image to a file. The file has to be a valid 
 * full path name on the undelying filesystem. The file
 * management of BASIC is bypassed here and the file is
 * created directly. Currently only the SD_MMC interface
 * is supported. 
 */

void camerasave(char* filename) {
	char* tmpfilename;
	consolelog("esp32cam: camera save\n");
	tmpfilename = mkfilename(filename); /* create the root prefix if needed for MMC not important */
	File camfile = SD_MMC.open(filename, FILE_WRITE);
	if (!camfile) {
		if (RTDEBUG) consolelog("Camera file open failed.\n");
		ioer=1;
		return;
	} else {
		if (RTDEBUG) consolelog("Camera file open success.\n");
	}
	if (!esp32_camera_fb) {
		if (RTDEBUG) consolelog("Camera frame buffer not available.\n");
		ioer=1;
		return;
	}
	camfile.write(esp32_camera_fb->buf, esp32_camera_fb->len);
	camfile.close();
}

void cameraset(int8_t setting, int8_t value) {
	consolelog("esp32cam: camera set\n");

	/* check if the camera is initialized */
	if (!esp32_camera_sensor) {
		if (RTDEBUG) consolelog("Camera not initialized.\n");
		ioer=1;
		return;
	}

	/* this is a proposal of Copilot, keep this for a start */
	switch (setting) {
		case 0: /* set the brightness */
			if (RTDEBUG) consolelog("Camera brightness set.\n");
			esp32_camera_sensor->set_brightness(esp32_camera_sensor, value);
			break;
		case 1: /* set the contrast */
			if (RTDEBUG) consolelog("Camera contrast set.\n");
			esp32_camera_sensor->set_contrast(esp32_camera_sensor, value);
			break;
		case 2: /* set the saturation */
			if (RTDEBUG) consolelog("Camera saturation set.\n");
			esp32_camera_sensor->set_saturation(esp32_camera_sensor, value);
			break;
		case 3: /* set the sharpness */
			if (RTDEBUG) consolelog("Camera sharpness set.\n");
			esp32_camera_sensor->set_sharpness(esp32_camera_sensor, value);
			break;
		case 4: /* set the white balance */
			if (RTDEBUG) consolelog("Camera white balance set.\n");
			esp32_camera_sensor->set_whitebal(esp32_camera_sensor, value);
			break;
		case 5: /* set the gain control */
			if (RTDEBUG) consolelog("Camera gain control set.\n");
			esp32_camera_sensor->set_gainceiling(esp32_camera_sensor, static_cast<gainceiling_t>(value));
			break;
		default:
			if (RTDEBUG) consolelog("Camera setting not supported.\n");
			ioer=1;
			return;
	}
}

void cameraend() {
	if (RTDEBUG) consolelog("esp32cam: camera end\n");
	if (esp32_camera_fb) {
		esp_camera_fb_return(esp32_camera_fb);
		esp32_camera_fb = NULL;
	}
	if (esp32_camera_sensor) {
		esp32_camera_sensor = NULL;
	}
	esp_camera_deinit();
}
 
