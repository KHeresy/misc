#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ocl/ocl.hpp>

#pragma comment( lib, "opencv_core249.lib" )
#pragma comment( lib, "opencv_gpu249.lib" )
#pragma comment( lib, "opencv_highgui249.lib" )
#pragma comment( lib, "opencv_imgproc249.lib" )
#pragma comment( lib, "opencv_ocl249.lib" )

template<class _FUNC>
size_t benchmark(_FUNC func, unsigned int uTimes = 1)
{
	auto tpNow = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < uTimes; ++ i )
		func();
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tpNow).count();
}

void WriteFile(const std::string& rFile, const cv::Mat& mResult)
{
	static int iCounter = 0;

	std::stringstream ss;
	ss << rFile << "." << ++iCounter << ".tif";
	std::string sOutFile = ss.str();

	std::cout << "Save File " << benchmark([&mResult, &sOutFile](){
		cv::imwrite(sOutFile, mResult);
	}) << " milliseconds" << std::endl;
}

int main(int argc, char** argv)
{
	cv::Mat mSource, mResult;
	std::string sFile = argv[1], sOutFile;
	unsigned int uTimes = std::atoi( argv[2] );
	unsigned int uTimes2 = std::atoi(argv[3]);

	std::cout << "Load File " << benchmark([&mSource, &sFile](){
		mSource = cv::imread(sFile, CV_LOAD_IMAGE_UNCHANGED);
	} ) << " milliseconds" << std::endl;

	std::cout << "\nBuild rotate matrix" << std::endl;
	cv::Point2f ptCenter(mSource.cols / 2,mSource.rows / 2);
	double dAngle = 30;
	cv::Mat mRotate = cv::getRotationMatrix2D(ptCenter, dAngle, 1.0);

	std::cout << "\n============\nCPU Test" << std::endl;
	cv::Mat cSource = mSource.clone();
	std::cout << "Rotate in " << benchmark([&cSource, &mResult, &mRotate, &uTimes2](){
		for (int i = 0; i < uTimes2; ++i)
		{
			cv::warpAffine(cSource, mResult, mRotate, cSource.size(), CV_INTER_CUBIC);
			std::swap(cSource, mResult);
		}
	}, uTimes) << " milliseconds" << std::endl;

	WriteFile( sFile + ".cpu",mResult );

	cv::ocl::PlatformsInfo vPInfo;
	cv::ocl::getOpenCLPlatforms(vPInfo);
	std::cout << "\n============\nOpenCL Test : " << vPInfo.size() << " platforms" << std::endl;
	for (const cv::ocl::PlatformInfo* pInfo : vPInfo)
	{
		std::cout << "\n>>>>>>>>>\n" << pInfo->platformName << " : " << pInfo->devices.size() << " devices" << std::endl;
		for (const cv::ocl::DeviceInfo* pDevice : pInfo->devices)
		{
			std::cout << "\n-----------\n" << pDevice->deviceName << std::endl;

			std::cout << " > Initial in " << benchmark([&pDevice](){
				cv::ocl::setDevice(pDevice);
			},1) << " milliseconds" << std::endl;

			std::cout << " > dummy upload " << benchmark([](){
				cv::Mat mM1(16, 16, CV_8UC1);
				cv::ocl::oclMat mMat;
				mMat.upload(mM1);
			}, 1) << " milliseconds" << std::endl;

			cv::ocl::oclMat mGpuSource, mGpuResult;
			std::cout << " > Upload to GPU in " << benchmark([&mSource, &mGpuSource](){
				mGpuSource.upload(mSource);
			}, uTimes) << " milliseconds" << std::endl;

			std::cout << " > Rotate in " << benchmark([&mGpuSource, &mGpuResult, &mRotate, &uTimes2](){
				for (int i = 0; i < uTimes2; ++i)
				{
					cv::ocl::warpAffine(mGpuSource, mGpuResult, mRotate, mGpuSource.size(), CV_INTER_CUBIC);
					mGpuResult.swap(mGpuSource);
				}
			}, uTimes) << " milliseconds" << std::endl;

			std::cout << " > Download from GPU in " << benchmark([&mResult, &mGpuResult](){
				mGpuResult.download(mResult);
			}, uTimes) << " milliseconds" << std::endl;

			WriteFile(sFile + ".ocl", mResult);
		}
	}

	int iNum = cv::gpu::getCudaEnabledDeviceCount();
	std::cout << "\n============\nCUDA Test : " << iNum << " devices" << std::endl;
	for (int i = 0; i < iNum; ++i)
	{
		std::cout << " > Initial in " << benchmark([&i](){
			cv::gpu::setDevice(i);
		}, 1) << " milliseconds" << std::endl;

		std::cout << " > dummy upload " << benchmark([](){
			cv::Mat mM1(16, 16, CV_8UC1);
			cv::gpu::GpuMat mMat;
			mMat.upload(mM1);
		}, 1) << " milliseconds" << std::endl;

		cv::gpu::printCudaDeviceInfo(i);

		cv::gpu::GpuMat mGpuSource, mGpuResult;
		std::cout << " > Upload to GPU in " << benchmark([&mSource, &mGpuSource](){
			mGpuSource.upload(mSource);
		}, uTimes) << " milliseconds" << std::endl;

		std::cout << " > Rotate in " << benchmark([&mGpuSource, &mGpuResult, &mRotate, &uTimes2](){
			for (int i = 0; i < uTimes2; ++i)
			{
				cv::gpu::warpAffine(mGpuSource, mGpuResult, mRotate, mGpuSource.size(), CV_INTER_CUBIC);
				mGpuResult.swap(mGpuSource);
			}
		}, uTimes) << " milliseconds" << std::endl;

		std::cout << " > Download from GPU in " << benchmark([&mResult, &mGpuResult](){
			mGpuResult.download(mResult);
		}, uTimes) << " milliseconds" << std::endl;

		WriteFile(sFile + ".cuda", mResult);
	}
}
