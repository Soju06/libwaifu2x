#include <stdio.h>
#include <string>
#include <iostream>
#include "waifu2x.h"

/*
* -1		 - none			   | parameter is incorrect.
* -2		 - none			   | Invalid parameter value.
* 0x00000014 - waifu2x-lib     | The scale only accepts the following values: 1, multiples of 2
* 0x00000015 - waifu2x-lib     | The model only accepts the following values: models-cunet, models-upconv_7_anime_style_art_rgb, models-upconv_7_photo
* 0x00001003 - waifu2x-lib     | Invalid device index.
* 0x00001007 - waifu2x-lib     | The device does not have a GPU available.
* 0x0000100C - waifu2x-lib     | Invalid GPU index. The index cannot be -1.
* 0x0000101F - waifu2x-lib     | Failed to load model.
* 0x00001012 - waifu2x-lib     | spirv compile failed
* 0x00001019 - waifu2x-lib     | Pipeline creation failed
* 0x000013C0 - waifu2x-lib     | Failed to open file.
* 0x000013C1 - waifu2x-lib     | Invalid image data
* 0x000013C3 - waifu2x-lib     | Failed to create file.
*/

EXPORT const char* get_error_message(int error_code) {
	switch (error_code) {
	case -1:
		return "none			   | parameter is incorrect.";
	case -2:
		return "none			   | Invalid parameter value.";
	case 0x00000014:
		return "waifu2x-lib     | The scale only accepts the following values: 1, multiples of 2";
	case 0x00000015:
		return "waifu2x-lib     | The model only accepts the following values: models-cunet, models-upconv_7_anime_style_art_rgb, models-upconv_7_photo";
	case 0x00001003:
		return "waifu2x-lib     | Invalid device index.";
	case 0x00001007:
		return "waifu2x-lib     | The device does not have a GPU available.";
	case 0x0000100C:
		return "waifu2x-lib     | Invalid GPU index. The index cannot be -1.";
	case 0x0000101F:
		return "waifu2x-lib     | Failed to load model.";
	case 0x00001012:
		return "waifu2x-lib     | spirv compile failed.";
	case 0x00001019:
		return "waifu2x-lib     | Pipeline creation failed.";
	case 0x000013C0:
		return "waifu2x-lib     | Failed to open file.";
	case 0x000013C1:
		return "waifu2x-lib     | Invalid image data";
	case 0x000013C3:
		return "waifu2x-lib     | Failed to create file.";
	default:
		return "unknown         | unknown error";
	}
}