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
	constexpr char FRANKIE[] = "Frankie.pgm";
	constexpr char SHUTTLE[] = "shuttle_pgm.pgm";

	constexpr char LENA_ZOOM_NN[] = "lena256_PGM_resizeNN.pgm";
	constexpr char LENA_ZOOM_BL[] = "lena256_PGM_resizeBL.pgm";
	constexpr char LENA_ZOOM_SAVEBC[] = "lena256_PGM_resizeBC.pgm";
	constexpr char LENA_ZOOM_SAVELA[] = "lena256_PGM_resizeLA.pgm";

	constexpr char SHUTTLE_ZOOM_SAVENN[] = "shuttle_pgm_resizeNN.pgm";
	constexpr char SHUTTLE_ZOOM_SAVEBL[] = "shuttle_pgm_resizeBL.pgm";
	constexpr char SHUTTLE_ZOOM_SAVEBC[] = "shuttle_pgm_resizeBC.pgm";
	constexpr char SHUTTLE_ZOOM_SAVELA[] = "shuttle_pgm_resizeLA.pgm";

	constexpr char COMMENT[] = "# Modified using a program written by Michael Kennedy";

	constexpr double RATIO = 3.0;

	auto timeStart = std::chrono::high_resolution_clock().now();

	MKImage::Image lena1(LENA256);
	lena1.scalingProcessing(lena1.columns() * RATIO, lena1.rows() * RATIO, MKImage::Image::ScalingOps::nearestNeighbor);
	lena1.save(LENA_ZOOM_NN, COMMENT);

	MKImage::Image lena2(LENA256);
	lena2.scalingProcessing(lena2.columns() * RATIO, lena2.rows() * RATIO, MKImage::Image::ScalingOps::bilinear);
	lena2.save(LENA_ZOOM_BL, COMMENT);

	MKImage::Image lena3(LENA256);
	lena3.scalingProcessing(lena3.columns() * RATIO, lena3.rows() * RATIO, MKImage::Image::ScalingOps::bicubic);
	lena3.save(LENA_ZOOM_SAVEBC, COMMENT);

	MKImage::Image lena4(LENA256);
	lena4.scalingProcessing(lena4.columns() * RATIO, lena4.rows() * RATIO, MKImage::Image::ScalingOps::lanczos2);
	lena4.save(LENA_ZOOM_SAVELA, COMMENT);

	MKImage::Image shuttle1(SHUTTLE);
	shuttle1.scalingProcessing(shuttle1.columns() * RATIO, shuttle1.rows() * RATIO, MKImage::Image::ScalingOps::nearestNeighbor);
	shuttle1.save(SHUTTLE_ZOOM_SAVENN, COMMENT);

	MKImage::Image shuttle2(SHUTTLE);
	shuttle2.scalingProcessing(shuttle2.columns() * RATIO, shuttle2.rows() * RATIO, MKImage::Image::ScalingOps::bilinear);
	shuttle2.save(SHUTTLE_ZOOM_SAVEBL, COMMENT);

	MKImage::Image shuttle3(SHUTTLE);
	shuttle3.scalingProcessing(shuttle3.columns() * RATIO, shuttle3.rows() * RATIO, MKImage::Image::ScalingOps::bicubic);
	shuttle3.save(SHUTTLE_ZOOM_SAVEBC, COMMENT);

	MKImage::Image shuttle4(SHUTTLE);
	shuttle4.scalingProcessing(shuttle4.columns() * RATIO, shuttle4.rows() * RATIO, MKImage::Image::ScalingOps::lanczos2);
	shuttle4.save(SHUTTLE_ZOOM_SAVELA, COMMENT);
	
	auto timeStop = std::chrono::high_resolution_clock().now();

	std::chrono::duration<double> runTime = timeStop - timeStart;
	std::cout << "\nProgram runtime(seconds): " << runTime.count() << std::endl;

	//size_t threads = std::thread::hardware_concurrency();
	//std::cout << "Number of threads supported on this computer is: " << threads << std::endl;

	std::cout << "Press enter to continue\n";
	std::cin.get();

	return 0;
}