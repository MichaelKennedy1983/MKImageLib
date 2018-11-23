#include "MKIImage.h"
#include "MKIHistogram.h"
#include "MKIImageFuncs.h"
#include "MKIMask.h"

#include <thread>
#include <chrono>
#include <iostream>


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
	constexpr char LENA_ZOOM_SAVE[] = "lena256_PGM_zoom.pgm";
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

	MKImage::Image lena(LENA256);
	lena.scalingProcessing(lena.columns() * 3.5, lena.rows() * 3.5, MKImage::Image::ScalingOps::scaling);
	lena.save(LENA_ZOOM_SAVE);

	//MKImage::Image lena(LENA256);
	//lena.maskProcessing(MKImage::Mask::EDGE_LAPLACIAN_3X3);
	//lena.save(LENA_EDGE_SAVE);

	//MKImage::Image lena256(LENA256);
	//MKImage::Image lena256WBlock(LENA256_W_BLOCK);
	//lena256.frameProcessing(lena256WBlock, MKImage::Image::FrameOps::sub);
	//lena256.save(LENA_BLOCK_SAVE, COMMENT);

	//MKImage::Image lena256(LENA256);
	//MKImage::Image lena256Sharp(lena256);
	//MKImage::Image lena256Edge(lena256);

	//std::thread lSmoothThread([&LENA_SMOOTH_SAVE, &COMMENT, &lena256, &LENA256]() {
	//	lena256.maskProcessing(MKImage::Mask::SMOOTH_3X3);
	//	lena256.save(LENA_SMOOTH_SAVE, COMMENT);
	//});
	//
	//std::thread lSharpThread([&LENA_SHARP_SAVE, &COMMENT, &lena256Sharp]() {
	//	lena256Sharp.maskProcessing(MKImage::Mask::GAUSSIAN_HEDGED_LAPLACIAN_5X5);
	//	lena256Sharp.save(LENA_SHARP_SAVE, COMMENT);
	//});

	//std::thread lEdgeThread([&LENA_EDGE_SAVE, &COMMENT, &HIST_SAVE, &HISTEQ_SAVE, &lena256Edge]() {
	//	lena256Edge.maskProcessing(MKImage::Mask::SMOOTH_3X3);
	//	lena256Edge.maskProcessing(MKImage::Mask::EDGE_LAPLACIAN_3X3);
	//	lena256Edge.save(LENA_EDGE_SAVE, COMMENT);
	//});

	//lSmoothThread.join();
	//lSharpThread.join();
	//lEdgeThread.join();

	//MKImage::Image lena512(LENA512);
	//MKImage::Image lena512Log(lena512);
	//MKImage::Image lena512Hist(lena512);
	//MKImage::Image lena512LinHist(lena512);
	//MKImage::Image lena512Mask(lena512);

	//lena512.save(LENA512);
	//lena512.pointProcessing(MKImage::GS::linearTransformation, lena512.minValue(), lena512.maxValue(), 0i16, 255i16);
	//lena512.save(LENA512_LIN_SAVE, COMMENT);

	//lena512Log.pointProcessing(MKImage::GS::logarithmicTransformation, MKImage::Math::logTransC(lena512Log.depth()));
	//lena512Log.save(LENA512_LOG_SAVE, COMMENT);

	//MKImage::MKIHistogram lena512Histogram(lena512Hist);
	//lena512Histogram.makeEqualized();
	//lena512Hist.pointProcessing(MKImage::GS::histogramTransformation, lena512Histogram, lena512Hist.depth());
	//lena512Hist.save(LENA512_HIST_SAVE, COMMENT);
	//lena512Histogram.save(HIST512_SAVE);
	//lena512Histogram.saveEqualized(HISTEQ512_SAVE);

	//lena512LinHist.pointProcessing(MKImage::GS::linearTransformation, lena512LinHist.minValue(), lena512LinHist.maxValue(), 0, 255);
	//MKImage::MKIHistogram linHist(lena512LinHist);
	//linHist.makeEqualized();
	//lena512LinHist.pointProcessing(MKImage::GS::histogramTransformation, linHist, lena512LinHist.depth());
	//lena512LinHist.save(LENA512_LINHIST_SAVE, COMMENT);

	//lena512Mask.maskProcessing(MKImage::Mask::GAUSSIAN_BLUR_5X5);
	//lena512Mask.maskProcessing(MKImage::Mask::HARD_EDGE_LAPLACIAN_5X5);
	//lena512Mask.save(LENA512_MASK_SAVE, COMMENT);

	//MKImage::Image frankie(FRANKIE_COUCH);
	//frankie.saveCopy();
	//frankie.pointProcessing(MKImage::GS::linearTransformation, frankie.minValue(), frankie.maxValue(), 0, 255);
	//MKImage::MKIHistogram frankieHist(frankie);
	//frankieHist.makeEqualized();
	//frankie.pointProcessing(MKImage::GS::histogramTransformation, frankieHist, frankie.depth());
	//frankie.maskProcessing(MKImage::Mask::BLUR_5X5);
	//frankie.maskProcessing(MKImage::Mask::HARD_EDGE_LAPLACIAN_5X5);
	//frankie.pointProcessing(MKImage::GS::negative, frankie.depth());
	//frankie.save(FRANKIE_COUCH_SAVE);

	//MKImage::Image peppers(PEPPERS);
	//peppers.saveCopy();
	//peppers.maskProcessing(MKImage::Mask::BLUR_5X5);
	//peppers.maskProcessing(MKImage::Mask::HARD_EDGE_LAPLACIAN_5X5);
	//peppers.save(PEPPERS_SAVE, COMMENT);
	
	auto timeStop = std::chrono::high_resolution_clock().now();

	std::chrono::duration<double> runTime = timeStop - timeStart;
	std::cout << "\nProgram runtime(seconds): " << runTime.count() << std::endl;

	//size_t threads = std::thread::hardware_concurrency();
	//std::cout << "Number of threads supported on this computer is: " << threads << std::endl;

	std::cin.get();

	return 0;
}