/*
* cgeFrameRecorder.cpp
*
*  Created on: 2015-7-29
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifdef _CGE_USE_FFMPEG_

#include "cgeFrameRecorder.h"
#include "cgeUtilFunctions.h"
#include "cgeMultipleEffects.h"

#include <chrono>
#include <new>

#if defined(DEBUG) || defined(_DEBUG)

#define CHECK_RECORDER_STATUS \
if(m_frameHandler == nullptr)\
{\
	CGE_LOG_ERROR("CGEFrameRecorder is not initialized!!\n");\
	return;\
}

#else

#define CHECK_RECORDER_STATUS

#endif

namespace CGE
{
	///////////////////////////////////////

	ImageDataWriteThread::ImageDataWriteThread() : m_bufferSize(0), m_task(nullptr)
	{

	}

	ImageDataWriteThread::~ImageDataWriteThread()
	{
		clearBuffers();
	}

	void ImageDataWriteThread::clearBuffers()
	{
		for(auto data : m_totalCaches)
		{
			delete[] data.buffer;
		}

		m_totalCaches.clear();
		m_data4Read = decltype(m_data4Read)();
		m_data4Write = decltype(m_data4Write)();
	}

	ImageDataWriteThread::DataCache ImageDataWriteThread::getData4Read()
	{
		DataCache cache;
		m_readMutex.lock();
		if(m_data4Read.empty())
		{
			cache.buffer = nullptr;
			// CGE_LOG_INFO("read data hungry");
		}
		else
		{
			cache = m_data4Read.front();
			m_data4Read.pop();
		}
		m_readMutex.unlock();
		return cache;
	}

	void ImageDataWriteThread::putData4Read(const DataCache& data)
	{
		m_readMutex.lock();
		m_data4Read.push(data);
		m_readMutex.unlock();
		if(!isActive())
			run();
	}

	ImageDataWriteThread::DataCache ImageDataWriteThread::getData4Write()
	{
		DataCache cache;
		m_writeMutex.lock();
		if(m_data4Write.empty())
		{
			if(m_totalCaches.size() < MAX_DATA_CACHE_LEN)
			{
				assert(bufferSize != 0); // bufferSize must be set!

				//增加申请内存失败判断
				cache.buffer = new (std::nothrow) unsigned char[m_bufferSize];

				if(cache.buffer == nullptr)
				{
					CGE_LOG_ERROR("Fatal Error: 内存不足， 申请内存失败!");
				}

				m_totalCaches.push_back(cache);
				CGE_LOG_INFO("Cache grow: %d", (int)m_totalCaches.size());
			}
			else
			{
				cache.buffer = nullptr;
				CGE_LOG_INFO("write data hungry, cache size: %d", (int)m_totalCaches.size());
			}
		}
		else
		{
			cache = m_data4Write.front();
			m_data4Write.pop();
		}
		m_writeMutex.unlock();
		return cache;
	}

	void ImageDataWriteThread::putData4Write(const DataCache& data)
	{
		m_writeMutex.lock();
		m_data4Write.push(data);
		m_writeMutex.unlock();
	}

	bool ImageDataWriteThread::hasData4Read()
	{
		bool canRead;
		m_readMutex.lock();
		canRead = !m_data4Read.empty();
		m_readMutex.unlock();
		return canRead;
	}

	bool ImageDataWriteThread::hasData4Write()
	{
		if(m_totalCaches.size() < MAX_DATA_CACHE_LEN)
			return true;

		bool canWrite;
		m_writeMutex.lock();
		canWrite = !m_data4Write.empty();
		m_writeMutex.unlock();
		return canWrite;
	}

	void ImageDataWriteThread::runTask()
	{
		assert(m_task != nullptr); //必须预设好task

		while(1)
		{
			auto data = getData4Read();
			if(data.buffer == nullptr)
				break;
			
			m_task(data);

			putData4Write(data);
		}
	}

	///////////////////////////////////////

	CGEFrameRecorder::CGEFrameRecorder() : m_recordThread(nullptr), m_recordImageThread(nullptr), m_encoder(nullptr), m_offscreenContext(nullptr), m_globalFilter(nullptr)
	{

		m_recordingWork = [&](void* pts){

			CHECK_RECORDER_STATUS;

			auto bufferCache = m_recordImageThread->getData4Write();

			if(bufferCache.buffer == nullptr)
				return ;

			// auto tm = getCurrentTimeMillis();

			if(m_offscreenContext != nullptr)
				m_offscreenContext->makecurrent();

			glViewport(0, 0, m_dstSize.width, m_dstSize.height);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			m_resultMutex.lock();
			m_cacheDrawer->drawTexture(m_frameHandler->getBufferTextureID());
			// m_cacheDrawer->drawTexture(m_frameHandler.getTargetTextureID());
			glFinish();
			// CGE_LOG_ERROR("draw texture 时间: %g", (getCurrentTimeMillis() - tm));
			m_resultMutex.unlock();

			glReadPixels(0, 0, m_dstSize.width, m_dstSize.height, GL_RGBA, GL_UNSIGNED_BYTE, bufferCache.buffer);

			// CGE_LOG_ERROR("录制readpixel时间: %g", (getCurrentTimeMillis() - tm));
			bufferCache.pts = (long)pts;
			m_recordImageThread->putData4Read(bufferCache);

		};
	}

	CGEFrameRecorder::~CGEFrameRecorder()
	{
		CGE_LOG_INFO("CGEFrameRecorder::~CGEFrameRecorder");


		endRecording(false);

		if(m_recordThread != nullptr)
		{
			CGE_LOG_INFO("m_recordThread kill before...");

			bool bShoudWait = true;

			m_recordThread->run(CGEThreadPool::Work([&](void*){
				CGE_LOG_INFO("Delete offscreen context...");
				delete m_offscreenContext;
				m_offscreenContext = nullptr;
				bShoudWait = false;
			}));

			while(bShoudWait || m_recordThread->isActive())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			CGE_LOG_INFO("m_recordThread kill after...");

			m_recordThread->quit();

			delete m_recordThread;
			m_recordThread = nullptr;
		}
	}

	// void CGEFrameRecorder::update(GLuint externalTexture, float* transformMatrix)
	// {
	// 	CHECK_RECORDER_STATUS;

	// 	m_frameHandler->useImageFBO();
	// 	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

	// 	m_textureDrawerExtOES->setTransform(transformMatrix);
	// 	m_textureDrawerExtOES->drawTexture(externalTexture);
	// }

	void CGEFrameRecorder::runProc()
	{	
		//processingFilters 将可能改变 targetTextureID和bufferTextureID, lock 以保证其他线程使用
		std::unique_lock<std::mutex> uniqueLock(m_resultMutex);

		if(m_globalFilter != nullptr)
		{
			m_frameHandler->processingWithFilter(m_globalFilter);
		}

		m_frameHandler->processingFilters();

		if(isRecordingStarted() && !m_isRecordingPaused)
		{

			//第一帧必然记录
			if(m_recordingTimestamp == 0.0)
			{
				m_recordingTimestamp = 0.0001; //设置为 0.0001 ms, 表示已经开始
				m_lastRecordingTime = getCurrentTimeMillis();
				CGE_LOG_INFO("first frame...");
			}
			else
			{
				double currentTime = getCurrentTimeMillis();
				m_recordingTimestamp += currentTime - m_lastRecordingTime;
				m_lastRecordingTime = currentTime;
				// CGE_LOG_INFO("time stamp %g...", m_recordingTimestamp);
			}

			int ptsInFact = m_recordingTimestamp * (m_recordFPS / 1000.0);

			if(ptsInFact < m_currentPTS)
			{
				CGE_LOG_INFO("帧速过快， 丢弃帧...");
				return ;
			}
			else if(ptsInFact > m_currentPTS + 3)
			{
				CGE_LOG_INFO("帧速较慢， 填补帧...");
				m_currentPTS = ptsInFact;
			}
			else
			{
				// CGE_LOG_INFO("帧速合适的很...");
				if(m_currentPTS == ptsInFact)
					m_currentPTS = ptsInFact + 1;
				else
					m_currentPTS = ptsInFact;
			}

			if(m_recordThread != nullptr)
			{
				m_frameHandler->useImageFBO();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameHandler->getBufferTextureID(), 0);

				glViewport(0, 0, m_dstSize.width, m_dstSize.height);
				m_cacheDrawer->drawTexture(m_frameHandler->getTargetTextureID());
				glFinish();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameHandler->getTargetTextureID(), 0);

				if(m_recordThread->isActive() && m_recordThread->totalWorks() != 0)
					return;

				m_recordThread->run(CGEThreadPool::Work(m_recordingWork, (void*)m_currentPTS));
			}
			else
			{
				auto bufferCache = m_recordImageThread->getData4Write();

				if(bufferCache.buffer != nullptr)
				{
					// auto tm = getCurrentTimeMillis();

					m_frameHandler->useImageFBO();

					// CGE_LOG_ERROR("draw texture 时间: %g", (getCurrentTimeMillis() - tm));

					glReadPixels(0, 0, m_dstSize.width, m_dstSize.height, GL_RGBA, GL_UNSIGNED_BYTE, bufferCache.buffer);

					// CGE_LOG_ERROR("录制readpixel时间: %g", (getCurrentTimeMillis() - tm));
					bufferCache.pts = m_currentPTS;
					m_recordImageThread->putData4Read(bufferCache);
				}
			}
		}
	}

	/////////////  视频录制相关  ///////////////

	void CGEFrameRecorder::recordImageFrame()
	{
		
	}

	void CGEFrameRecorder::recordAudioFrame(const AudioSampleData& data)
	{
		m_encoder->record(data);
	}

	bool CGEFrameRecorder::startRecording(int fps, const char* filename, int bitRate)
	{
		delete m_encoder;
		m_encoder = new CGEVideoEncoderMP4();
		m_encoder->setRecordDataFormat(CGEVideoEncoderMP4::FMT_RGBA8888);
		if(!m_encoder->init(filename, fps, m_dstSize.width, m_dstSize.height, true, bitRate))
		{
			delete m_encoder;
			m_encoder = nullptr;
			CGE_LOG_ERROR("CGEFrameRecorder::startRecording - start recording failed!");
			return false;
		}

		CGE_LOG_INFO("encoder created!");

		if(m_offscreenContext == nullptr || m_recordThread == nullptr)
			_createOffscreenContext(); //offscreen context 将从另一个线程创建 

		int bufferLen = m_dstSize.width * m_dstSize.height * 4;

		m_recordImageThread = new ImageDataWriteThread();

		m_recordImageThread->setBufferAllocSize(bufferLen);

		m_recordImageThread->setTask([&](const ImageDataWriteThread::DataCache& data) {

			// auto tm = getCurrentTimeMillis();

			CGEVideoEncoderMP4::ImageData imageData;
			imageData.width = m_dstSize.width;
			imageData.height = m_dstSize.height;
			imageData.linesize[0] = m_dstSize.width * 4;
			imageData.data[0] = data.buffer;
			imageData.pts = data.pts;

			// CGE_LOG_ERROR("PTS: %d", (int)data.pts);

			if(!m_encoder->record(imageData))
			{
				CGE_LOG_ERROR("record frame failed!");
			}

			// CGE_LOG_ERROR("pts sequence: %d, time: %g", (int)data.pts, getCurrentTimeMillis() - tm);
		});

		m_recordFPS = fps;
		m_currentPTS = -1;
		m_isRecording = true;
		m_isRecordingPaused = false;
		m_recordingTimestamp = 0.0;

		CGE_LOG_INFO("CGEFrameRecorder::startRecording...");

		return true;
	}

	bool CGEFrameRecorder::isRecordingStarted()
	{
		return m_encoder != nullptr && m_isRecording;
	}

	bool CGEFrameRecorder::endRecording(bool shouldSave)
	{
		m_isRecording = false;

		CGE_LOG_INFO("Waiting for the recording threads...");

		//等待录制结束
		while((m_recordThread != nullptr && m_recordThread->isActive()) || (m_recordImageThread != nullptr && m_recordImageThread->isActive())) 
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		delete m_recordImageThread;
		m_recordImageThread = nullptr;

		CGE_LOG_INFO("threads sync.");

		if(m_encoder == nullptr)
			return false;

		bool ret = true;
		CGE_LOG_INFO("CGEFrameRecorder::endRecording...");
		
		if(shouldSave)
		{
			ret = m_encoder->save();
		}
		else
		{
			m_encoder->drop();
		}

		CGE_LOG_INFO("delete encoder...");

		delete m_encoder;
		m_encoder = nullptr;

		CGE_LOG_INFO("CGEFrameRecorder::endRecording OK...");
		return ret;
	}

	void CGEFrameRecorder::pauseRecording()
	{
		//暂时不提供
		// m_isRecordingPaused = true;
		CGE_LOG_ERROR("Pause function is not completed by now!!");
	}

	bool CGEFrameRecorder::isRecordingPaused()
	{
		return m_encoder != nullptr && m_isRecordingPaused;
	}

	bool CGEFrameRecorder::resumeRecording()
	{
		if(m_encoder == nullptr)
			return false;
		m_isRecordingPaused = false;
		return true;
	}

	double CGEFrameRecorder::getRecordingTimestamp()
	{
		return m_recordingTimestamp;
	}

	double CGEFrameRecorder::getVideoStreamTime()
	{
		return m_encoder->getVideoStreamTime();
	}

	double CGEFrameRecorder::getAudioStreamTime()
	{
		return m_encoder->getAudioStreamTime();
	}

	void CGEFrameRecorder::_createOffscreenContext()
	{
		EGLContext sharedContext = eglGetCurrentContext();

		if(sharedContext == EGL_NO_CONTEXT)
		{
			CGE_LOG_ERROR("Context creation must be in the GL thread!");
			return;
		}

		if(m_recordThread == nullptr)
			m_recordThread = new CGEThreadPool();

		m_recordThread->run(CGEThreadPool::Work([&](void*){

			delete m_offscreenContext;
			m_offscreenContext = CGESharedGLContext::create(sharedContext, m_dstSize.width, m_dstSize.height, CGESharedGLContext::RECORDABLE_ANDROID);
			if(m_offscreenContext == nullptr)
			{
				CGE_LOG_ERROR("CGESharedGLContext : RECORDABLE_ANDROID is not supported!");
				m_offscreenContext = CGESharedGLContext::create(sharedContext, m_dstSize.width, m_dstSize.height, CGESharedGLContext::PBUFFER);
				if(m_offscreenContext == nullptr)
					CGE_LOG_ERROR("Fatal Error: Create Context Failed!");
			}

			if(m_offscreenContext != nullptr)
			{
				glViewport(0, 0, m_dstSize.width, m_dstSize.height);
				CGE_LOG_INFO("Info from offscreen context thread (begin)....");
				cgePrintGLInfo();
				CGE_LOG_INFO("Info from offscreen context thread (end)....");
			}
			else
			{
				CGE_LOG_ERROR("创建 OpenGL 子线程失败, 当前设备在录制视频时可能性能较差!");
			}
		}));

		while(m_recordThread->isActive())
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		//创建shared context失败， 将不使用OpenGL子线程
		if(m_offscreenContext == nullptr)
		{
			m_recordThread->quit();
			m_recordThread = nullptr;
		}
	}

	//人脸跟踪相关

	void CGEFrameRecorder::setGlobalFilter(const char* config, CGETextureLoadFun texLoadFunc, void* loadArg)
	{
		delete m_globalFilter;
		m_globalFilter = nullptr;

		if(config == nullptr || *config == '\0')
		{
			return;
		}

		auto* filter = new CGEMutipleEffectFilter();

		filter->setTextureLoadFunction(texLoadFunc, loadArg);

		if(!filter->initWithEffectString(config))
        {
        	delete filter;
        	return;
        }
		
        if(filter->isWrapper())
        {
        	auto&& v = filter->getFilters();
        	if(!v.empty())
        		m_globalFilter = v[0];
        }
        else
        {
        	m_globalFilter = filter;
        }
	}

	void CGEFrameRecorder::setGlobalFilterIntensity(float intensity)
	{
 		if(m_globalFilter != nullptr)
        {
            m_globalFilter->setIntensity(intensity);
        }
        else
        {
            CGE_LOG_ERROR("You must set a tracking filter first!\n");
        }
	}

}

#endif








