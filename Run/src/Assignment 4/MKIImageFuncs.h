#pragma once

namespace MKImage {
	class MKIHistogram;


	namespace GS {
		// adjust brightness of image
		short brightness(short val, short delta);

		short simpleContrast(short val, short lowBrake, short hiBrake, double lowPercent, double hiPercent);

		//short linearTransformation(short val, double a, double b);

		/*
			Performs a linear transformation

			fMin, fMax = min and max values from image.
			gMin, gMax = target min and max values
		*/
		short linearTransformation(short val, short fMin, short fMax, short gMin, short gMax);

		short logarithmicTransformation(short val, double c);

		short gammaTransformation(short val, short a, double gamma);

		short exponentialTransformation(short val, double a);

		short sigmoidTransformation(short val, double depth, double rate, double center);

		short altSigmoidTransformation(short val, double c);

		short negative(short val, short depth);

		short blackAndWhite(short val, short depth);

		/*
			Uses histogram equalization to perform a transformation on the image

			hist = a histogram
			depth = max gs value of image
		*/
		short histogramTransformation(short val, const MKIHistogram& hist, short depth);
	}

	namespace Math {
		double logTransC(short depth);

		double exponTransA(short depth);
	}
}