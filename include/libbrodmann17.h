/*
 * libbrodmann17.h
 *
 *  Created on: May 16, 2018
 *      Author: Brodmann17
 */

#ifndef LIBBRODMANN17_H_
#define LIBBRODMANN17_H_

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace bd17 {

static const int PARAMS_PER_DETECTION = 7; //[bounding_box=[x,y,w,h], confidence, PAD, PAD]

enum bd17_image_format_t {
	__BD17_IMAGE_FORMAT_START,

	bd17_bgr_interleaved_byte, // OpenCV BGR image  bgrbgrbgr...            Supported
	bd17_gray_byte,            // Gray byte image   yyy...                  Supported
	bd17_rgb_planar_float,     // RGB planar        RR..GG..BB..            NOT Supported
	bd17_yuv_planar_float,     // YUV planar        YY..UU..VV..            NOT Supported
	bd17_gray_float,           // Gray float image  YYY...                  Supported
	bd17_nv21_byte,            // NV21/YUV420sp     yyy...(uv)(uv)(uv)...   Supported

	__BD17_IMAGE_FORMAT_END,
};

enum bd17_rotation_t {
	bd17_rot0,
	bd17_rot90,
	bd17_rot180,
	bd17_rot270
};
/*
 * Initialize library.
 * must run before using!
 */
bool Init(void);

/*
 * Return new face detector.
 * in order to release memory DestroyDetector should be used.
 *
 * @return pointer to brodmann17 face detector (TinyBrodmann)
 */
void *CreateDetector(void);

/*
 * Rotate model.
 * take care that this method does not save state
 * so if you rotate(90) and the rotate(90) the state will be 180
 * The user is responsible to manage it.
 *
 * @param rot rotation degree to rotate the model.
 */
bool Rotate(void* detector_void, bd17::bd17_rotation_t rot);

/*
 * Run detector on given image
 *
 * @param detector_void Initialized detector
 * @param detections_out [o] detections output, array of [[bounding_box=[x,y,w,h], confidence, PAD, PAD]*N] (or float[7*N]), should be given.
 * @param detections_num [o] number of detections(N)
 * @param max_detections Maximal number of detections
 * @param image_input_data input image
 * @param image_width input image width
 * @param image_height input image height
 * @param image_format desired image format
 * @param class_id [o] class id of given detection
 * @param debug_str [o] debug string attached to given detection. String should be given.
 *
 * @return True if succeed, else False - it can happen if given ptr is NULL.
 */
bool Detect(void *detector_void,
		float *detections_out, unsigned int *detections_num, unsigned int max_detections,
		void *image_input_data, int image_width, int image_height,
		bd17_image_format_t image_format,
		unsigned int *class_id, char *debug_str);

/*
 * Destroy given detector
 *
 * @param detector_void Detector
 *
 * @return True if succeed, else False - it can happen if given ptr is NULL.
 */
bool DestroyDetector(void *detector_void);

}
#ifdef __cplusplus
};
#endif

#endif /* LIBBRODMANN17_H_ */
