package com.sensetime.stmobileapi;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.FloatByReference;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;

public interface STMobileApiBridge extends Library {
    class st_rect_t extends Structure {
    	
        public static class ByValue extends st_rect_t implements Structure.ByValue {
        }

        public int left;
        public int top;
        public int right;
        public int bottom;

        @Override
        protected List getFieldOrder() {
            return Arrays.asList(new String[]{"left", "top", "right", "bottom"});
        }

        @Override
        public st_rect_t clone() {
        	st_rect_t copy = new st_rect_t();
            copy.left = this.left;
            copy.top = this.top;
            copy.right = this.right;
            copy.bottom = this.bottom;
            return copy;
        }

        /**
         * The jna.Structure class is passed on by reference by default,
         * however, in some cases, we need it by value.
         */
        public st_rect_t.ByValue copyToValue() {
        	st_rect_t.ByValue retObj = new st_rect_t.ByValue();
            retObj.left = this.left;
            retObj.top = this.top;
            retObj.right = this.right;
            retObj.bottom = this.bottom;
            return retObj;
        }
    }
    
    class st_mobile_106_t extends Structure {
    	public st_rect_t rect;
    	public float score;
    	public float[] points_array = new float[212];
    	public float yaw;
    	public float pitch;
    	public float roll;
    	public int eye_dist;
    	public int ID;

        public st_mobile_106_t() {
            super();
        }

        public st_mobile_106_t(Pointer p) {
            super(p);
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList(new String[]{"rect", "score", "points_array", 
            		"yaw", "pitch", "roll", "eye_dist", "ID"});
        }

        @Override
        public st_mobile_106_t clone() {
        	st_mobile_106_t copy = new st_mobile_106_t();
            copy.rect = this.rect.clone();
            copy.score = this.score;
            copy.points_array = this.points_array;
            copy.yaw = this.yaw;
            copy.pitch = this.pitch;
            copy.roll = this.roll;
            copy.eye_dist = this.eye_dist;
            copy.ID = this.ID;
            return copy;
        }

        public static st_mobile_106_t[] arrayCopy(st_mobile_106_t[] origin) {
        	st_mobile_106_t[] copy = new st_mobile_106_t[origin.length];
            for (int i = 0; i < origin.length; ++i) {
                copy[i] = origin[i].clone();
            }
            return copy;
        }
    }

    /**
     * face信息及face上的相关动作

     typedef struct st_mobile_face_action_t {
         struct st_mobile_106_t face;          /// 人脸信息，包含矩形、106点、pose信息等//
         unsigned int face_action;             /// 脸部动作
     } st_mobile_face_action_t;

     * */

    class st_mobile_face_action_t extends Structure {
        public st_mobile_106_t face;
        public int face_action;

        public st_mobile_face_action_t() {
            super();
        }

        public st_mobile_face_action_t(Pointer p) {
            super(p);
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList(new String[] {"face", "face_action"});
        }

        @Override
        protected Object clone() throws CloneNotSupportedException {
            st_mobile_face_action_t copy = new st_mobile_face_action_t();
            copy.face = this.face;
            copy.face_action = this.face_action;

            return copy;
        }

        public static st_mobile_face_action_t[] arrayCopy(st_mobile_face_action_t[] origin) {
            st_mobile_face_action_t[] copy = new st_mobile_face_action_t[origin.length];
            for(int i=0; i<origin.length; i++) {
                try {
                    copy[i] = (st_mobile_face_action_t) origin[i].clone();
                } catch (CloneNotSupportedException e) {
                    e.printStackTrace();
                }
            }
            return copy;
        }
    }

    enum ResultCode {
        ST_OK(0),
        ST_E_INVALIDARG(-1),
        ST_E_HANDLE(-2),
        ST_E_OUTOFMEMORY(-3),
        ST_E_FAIL(-4),
        ST_E_DELNOTFOUND(-5),
    	ST_E_INVALID_PIXEL_FORMAT(-6),	///< 不支持的图像格式
    	ST_E_FILE_NOT_FOUND(-10),   ///< 模型文件不存在
    	ST_E_INVALID_FILE_FORMAT(-11),	///< 模型格式不正确，导致加载失败
    	ST_E_INVALID_APPID(-12),		///< 包名错误
    	ST_E_INVALID_AUTH(-13),		///< 加密狗功能不支持
    	ST_E_AUTH_EXPIRE(-14),		///< SDK过期
    	ST_E_FILE_EXPIRE(-15),		///< 模型文件过期
    	ST_E_DONGLE_EXPIRE(-16),	///< 加密狗过期
    	ST_E_ONLINE_AUTH_FAIL(-17),		///< 在线验证失败
    	ST_E_ONLINE_AUTH_TIMEOUT(-18);

        private final int resultCode;

        ResultCode(int resultCode) {
            this.resultCode = resultCode;
        }

        public int getResultCode() {
            return resultCode;
        }
    }

    /**
     * The instance of stmobile DLL
     */
    STMobileApiBridge FACESDK_INSTANCE = (STMobileApiBridge) Native.loadLibrary("st_mobile", STMobileApiBridge.class);

    //cv_mobile_face_106
  /// @brief 创建实时人脸106关键点跟踪句柄
  /// @param[in] model_path 模型文件的绝对路径或相对路径，若不指定模型可为NULL; 模型中包含detect+align+pose模型
  /// @param[in] config 配置选项 默认使用双线程跟踪，可选择使用单线程，实时视频预览建议使用双线程，图片或视频后处理建议使用单线程
  /// @parma[out] handle 人脸跟踪句柄，失败返回NULL
  /// @return 成功返回CV_OK, 失败返回其他错误信息
    int st_mobile_tracker_106_create(String model_path, int config, PointerByReference handle);

  /// @brief 设置检测到的最大人脸数目N，持续track已检测到的N个人脸直到人脸数小于N再继续做detect.
  /// @param[in] handle 已初始化的关键点跟踪句柄
  /// @param[in] max_facecount 设置为1即是单脸跟踪，有效范围为[1, 32]
  /// @return 成功返回CV_OK, 错误则返回错误码
    int st_mobile_tracker_106_set_facelimit(Pointer handle,int max_facecount);

  /// @brief 重置实时人脸106关键点跟踪
  /// @param [in] handle 已初始化的实时目标人脸106关键点跟踪句柄
  /// @return 成功返回CV_OK, 错误则返回错误码
    int st_mobile_tracker_106_reset(Pointer handle);

  /// @brief 设置tracker每多少帧进行一次detect.
  /// @param[in] handle 已初始化的关键点跟踪句柄
  /// @param[in] val  有效范围[1, -)
  /// @return 成功返回CV_OK, 错误则返回错误码
    int st_mobile_tracker_106_set_detect_interval(Pointer handle,int val);

  /// @brief 对连续视频帧进行实时快速人脸106关键点跟踪
  /// @param handle 已初始化的实时人脸跟踪句柄
  /// @param image 用于检测的图像数据
  /// @param pixel_format 用于检测的图像数据的像素格式,都支持，不推荐BGRA和BGR，会慢
  /// @param image_width 用于检测的图像的宽度(以像素为单位)
  /// @param image_height 用于检测的图像的高度(以像素为单位)
  /// @param orientation 视频中人脸的方向
  /// @param p_faces_array 检测到的人脸信息数组，api负责分配内存，需要调用st_mobile_release_tracker_result函数释放
  /// @param p_faces_count 检测到的人脸数量
  /// @return 成功返回CV_OK，否则返回错误类型
    int st_mobile_tracker_106_track(
    	Pointer handle,
    	byte[] image,
    	int pixel_format,
    	int image_width,
    	int image_height,
    	int image_stride,
    	int orientation,
    	PointerByReference p_faces_array,
    	IntByReference p_faces_count
    );

    int st_mobile_tracker_106_track(
        	Pointer handle,
        	int[] image,
        	int pixel_format,
        	int image_width,
        	int image_height,
        	int image_stride,
        	int orientation,
        	PointerByReference p_faces_array,
        	IntByReference p_faces_count
        );

    /// @brief 对连续视频帧进行实时快速人脸106关键点跟踪，并检测脸部动作
    /// @param handle 已初始化的实时人脸跟踪句柄
    /// @param image 用于检测的图像数据
    /// @param pixel_format 用于检测的图像数据的像素格式,都支持，不推荐BGRA和BGR，会慢
    /// @param image_width 用于检测的图像的宽度(以像素为单位)
    /// @param image_height 用于检测的图像的高度(以像素为单位)
    /// @param[in] image_stride 用于检测的图像的跨度(以像素为单位)，即每行的字节数；目前仅支持字节对齐的padding，不支持roi
    /// @param orientation 视频中人脸的方向
    /// @param p_face_action_array 检测到的人脸106点信息和脸部动作的数组，api负责管理内存，会覆盖上一次调用获取到的数据
    /// @param p_faces_count 检测到的人脸数量
    /// @return 成功返回ST_OK，失败返回其他错误码,错误码定义在st_common.h 中，如ST_E_FAIL等
    int st_mobile_tracker_106_track_face_action(
            Pointer handle,
            byte[] image,
            int pixel_format,
            int image_width,
            int image_height,
            int image_stride,
            int orientation,
            PointerByReference p_face_action_array,
            IntByReference p_faces_count
    );

  /// @brief 释放实时人脸106关键点跟踪返回结果时分配的空间
  /// @param faces_array 跟踪到到的人脸信息数组
  /// @param faces_count 跟踪到的人脸数量
    void st_mobile_tracker_106_release_result(Pointer faces_array, int faces_count);

  /// @brief 销毁已初始化的track106句柄
  /// @param handle 已初始化的句柄
    void st_mobile_tracker_106_destroy(Pointer handle);

   //========face detection
  /// @brief 创建人脸检测句柄
  int st_mobile_face_detection_create(String model_path, int config, PointerByReference handle);

  int st_mobile_face_detection_detect(
  	Pointer handle,
  	byte[] image,
  	int pixel_format,
  	int image_width,
  	int image_height,
  	int image_stride,
  	int orientation,
  	PointerByReference p_faces_array,
  	IntByReference p_faces_count
  );

  int st_mobile_face_detection_detect(
		  	Pointer handle,
		  	int[] image,
		  	int pixel_format,
		  	int image_width,
		  	int image_height,
		  	int image_stride,
		  	int orientation,
		  	PointerByReference p_faces_array,
		  	IntByReference p_faces_count
		  );

/// @brief 释放人脸检测结果
  void st_mobile_face_detection_release_result(Pointer faces_array,int faces_count);

/// @brief 销毁已初始化的人脸检测句柄
/// @param[in] handle 已初始化的句柄
  void st_mobile_face_detection_destroy(Pointer handle);


/// @brief 创建人脸属性检测句柄
/// @param[in] model_path 模型文件的绝对路径或相对路径
/// @param[out] 人脸属性检测句柄
/// @return 成功返回ST_OK, 错误则返回错误码，错误码定义在st_common.h 中，如ST_E_FAIL等
  int st_mobile_face_attribute_create(
      String model_path,
      PointerByReference handle
  );

/// @brief 销毁已初始化的人脸属性检测句柄
/// @param[in] attribute_handle 已初始化的人脸属性检测句柄
  void st_mobile_face_attribute_destroy(Pointer handle);

/// @brief 人脸属性检测
/// @param[in] handle 已初始化的人脸属性检测句柄
/// @param[in] image 用于检测的图像数据,推荐灰度格式
/// @param[in] piexl_format 用于检测的图像数据的像素格式, 支持所有彩色图像格式，推荐ST_PIX_FMT_BGR888，不建议使用ST_PIX_FMT_GRAY8结果不准确
/// @param[in] image_width 用于检测的图像的宽度(以像素为单位)
/// @param[in] image_height 用于检测的图像的高度(以像素为单位)
/// @param[in] image_stride 用于检测的图像中每一行的跨度(以像素为单位)
/// @param[in] p_face 输入待处理的人脸信息，需要包括关键点信息
/// @param[out] p_attributes 检测到的人脸属性结果(由用户分配和释放，长度需为对应属性ST_ATTRIBUTE_LENGTH的定义)
/// @return 成功返回ST_OK, 错误则返回错误码，错误码定义在st_common.h 中，如ST_E_FAIL等
   int st_mobile_face_attribute_detect(
         Pointer handle,
         byte[] image,
         int pixel_format,
         int image_width,
         int image_height,
         int image_stride,
         PointerByReference p_face,                 //检查这里是否和st_mobile_face_action_t **p_face_action_array用法类似
         IntByReference p_attributes
   );


}
