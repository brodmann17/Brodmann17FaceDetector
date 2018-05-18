"""
python wrapper to Brodmann17 face detector
"""
import numpy as np
import os
import platform
from ctypes import *


class Bd17Assert(Exception):
    pass


class Detector(object):
    def __init__(self):
        self._lib = _bd17lib()
        self._inner_detector = self._lib.CreateDetector()

    def rotate(self, angle):
        """
        Rotate model.
        take care that this method does not save state
        so if you rotate(90) and the rotate(90) the state will be 180
        The user is responsible to manage it.

        Args:
            angle: angle to rotate the model, should be one of [ 0, 90, 180, 270, ]

        Returns: True if succeed, else False

        """
        if angle not in [0, 90, 180, 270, ]:
            raise ValueError("Angle degree is not supported, should be one of [0, 90, 180, 270,]")
        angle_idx = angle / 90;
        return self._lib.Rotate(self._inner_detector, angle_idx)

    def detect(self, img, max_detections=10000):
        """
        Detect faces on given image
        Args:
            img: image to detect
            max_detections: Maximum detection to output, optional

        Returns: numpy list of detections ([x, y, w, h, score]*#detections)

        """
        return self._lib.Detect(self._inner_detector, img, max_detections)

    def release(self):
        """
        Release detector memory
        """
        self._lib.DestroyDetector(self._inner_detector)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.release()


class _bd17lib(object):

    @staticmethod
    def bd17assert(cond, message=''):
        if not cond:
            raise Bd17Assert(message)

    SUPPORTED_ARCH = ['x86_64', ]

    def __init__(self):
        # import library using abs path
        absp = os.path.abspath(os.path.dirname(__file__))
        cur_platform = platform.machine()
        if cur_platform not in self.SUPPORTED_ARCH:
            raise RuntimeError("Current architecture does not supported in brodmann17 library")
        self._lib = cdll.LoadLibrary(os.path.join(absp, "..", "lib", cur_platform, "libbrodmann17.so"))
        _bd17lib.bd17assert(self._lib.Init(), "failed to init library")

    def CreateDetector(self):
        self._lib.CreateDetector.restype = c_void_p
        det_ptr = cast(self._lib.CreateDetector(), c_void_p)
        return det_ptr


    def Detect(self, detector, img_uint8_hwc, max_detections):
        """
        :param detector:
        :param img_uint8_chw:
        :param max_detections: maximal number of detections
        :return: for each detection ([x, y, w, h, confidence, ])
        """
        _bd17lib.bd17assert(img_uint8_hwc.dtype==np.uint8,
                           "image must be of type uint8. got {}".format(img_uint8_hwc.dtype))
        # get type (according to num of channels
        if len(img_uint8_hwc.shape) == 3:
            h, w, c = img_uint8_hwc.shape
        else:
            h, w = img_uint8_hwc.shape
            c = 1
        
        if c == 1:
            image_format = 2
        elif c == 3:
            image_format = 1
        else:
            _bd17lib.bd17assert("image with {} channels is currently not supported".format(c))

        detections = np.ones((max_detections, 7), dtype=np.float32)
        detections_ptr = detections.ctypes.data_as(POINTER(c_float))

        detections_num = np.zeros((1,), dtype=np.uint32)
        detections_num_ptr = detections_num.ctypes.data_as(POINTER(c_uint32))

        class_id = np.zeros((max_detections,), dtype=np.uint32)
        class_id_ptr = class_id.ctypes.data_as(POINTER(c_uint32))

        _bd17lib.bd17assert(self._lib.Detect(detector, detections_ptr, detections_num_ptr, max_detections,
                                            img_uint8_hwc.ctypes.data_as(POINTER(c_uint8)),
                                            w, h, image_format, class_id_ptr,
                                            None), "detection failed")
        detections_num = detections_num[0]
        out = detections[:detections_num, :5]
        return out

    def DestroyDetector(self, detector):
        _bd17lib.bd17assert(self._lib.DestroyDetector(detector), "failed to de-allocate detector")

    def Rotate(self, detector, rot):
        """
        Rotating the detector
        :param detector:
        :param rot: 0 - no rotation
                    1 - 90 deg
                    2 - 180 deg
                    3 - 270 deg
        :return: None
        """
        _bd17lib.bd17assert(self._lib.Rotate(detector, int(rot) % 4), "rotation failed")

