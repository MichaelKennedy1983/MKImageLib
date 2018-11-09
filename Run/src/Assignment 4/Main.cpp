#pragma once

#include "MKIImage.h"
#include "MKIHistogram.h"
#include "MKIImageFuncs.h"
#include "MKIMask.h"

#include <thread>
#include <chrono>


int main() {
	constexpr char LENA256[] = "lena256_PGM.pgm";
	constexpr char LENA256_W_BLOCK[] = "lena256 with block 2.pgm";
	constexpr char LENA512[] = "lena512_PGM.pgm";
	constexpr char LENA_BLOCK_SAVE[] = "lena256WBlock.pgm";
	constexpr char LENA_LIN_SAVE[] = "lena256_PGM_linear.pgm";
	constexpr char LENA_LOG_SAVE[] = "lena256_PGM_log.pgm";
	constexpr char LENA_HIST_SAVE[] = "lena256_PGM_histeq.pgm";
	constexpr char LENA_LINHIST_SAVE[] = "lena256_PGM_linhisteq.pgm";
	constexpr char LENA_SMOOTH_SAVE[] = "lena256_PGM_smooth.pgm";
	constexpr char LENA_SHARP_SAVE[] = "lena256_PGM_sharp.pgm";
	constexpr char LENA_EDGE_SAVE[] = "lena256_PGM_edge.pgm";
	constexpr char LENA512_LIN_SAVE[] = "lena512_PGM_linear.pgm";
	constexpr char LENA512_LOG_SAVE[] = "lena512_PGM_log.pgm";
	constexpr char LENA512_HIST_SAVE[] = "lena512_PGM_histeq.pgm";
	constexpr char LENA512_LINHIST_SAVE[] = "lena512_PGM_linhisteq.pgm";
	constexpr char LENA512_MASK_SAVE[] = "lena512_PGM_mask.pgm";
	constexpr char COMMENT[] = "# Modified using a program written by Michael Kennedy";
	constexpr char HIST_SAVE[] = "hist.csv";
	constexpr char HISTEQ_SAVE[] = "histeq.csv";
	constexpr char HIST512_SAVE[] = "hist512.csv";
	constexpr char HISTEQ512_SAVE[] = "histeq512.csv";
	constexpr char FRANKIE[] = "Frankie.pgm";
	constexpr char FRANKIE_SAVE[] = "Frankie_mask.pgm";
	constexpr char FRANKIE_COUCH[] = "Frankie_Couch_PGM.pgm";
	constexpr char FRANKIE_COUCH_SAVE[] = "Frankie_Couch_mask.pgm";
	constexpr char SHUTTLE[] = "shuttle_pgm.pgm";
	constexpr char SHUTTLE_SAVE[] = "shuttle_pgm_mask.pgm";
	constexpr char PEPPERS[] = "peppers512_PGM.pgm";
	constexpr char PEPPERS_SAVE[] = "peppers512_PGM_mask.pgm";

	auto timeStart = std::chrono::high_resolution_clock().now();

	MKImage::Image lena256(LENA256);
	MKImage::Image lena256WBlock(LENA256_W_BLOCK);
	lena256.frameProcessing(lena256WBlock, MKImage::Image::FrameOps::sub);
	lena256.save(LENA_BLOCK_SAVE, COMMENT);
	
	auto timeStop = std::chrono::high_resolution_clock().now();

	std::chrono::duration<double> runTime = timeStop - timeStart;
	std::cout << "Program runtime(seconds): " << runTime.count() << std::endl;

	size_t threads = std::thread::hardware_concurrency();

	std::cout << "Number of threads supported on this computer is: " << threads << std::endl;

	system("pause");

	return 0;
}