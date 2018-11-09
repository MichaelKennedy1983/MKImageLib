#include "MKIImageFuncs.h"

#include "MKIHistogram.h"

#include <cmath>

namespace MKImage {
	namespace GS {
		short brightness(short val, short delta) {
			return val + delta;
		};

		short simpleContrast(short val, short lowThresh, short hiThresh, double lowPercent, double hiPercent) {
			if (val < lowThresh) {
				return static_cast<short>(val * lowPercent);
			}
			if (val > hiThresh) {
				return static_cast<short>(val * hiPercent);
			}

			return val;
		}

		//short linearTransformation(short val, double a, double b) {
		//	return static_cast<int>((a * val) + b);
		//}

		/*
		
		*/
		short linearTransformation(short val, short fMin, short fMax, short gMin, short gMax) {
			double a = (gMax - gMin) / (fMax - fMin);
			return static_cast<short>((a * (val - fMin)) + gMin);
		}

		short logarithmicTransformation(short val, double c) {
			return static_cast<short>(c * std::log2(val + 1));
		}

		short gammaTransformation(short val, short a, double gamma) {
			return static_cast<short>(a * std::pow(val, gamma));
		}

		short exponentialTransformation(short val, double a) {
			return static_cast<short>(std::exp(a * val) - 1);
		}

		short sigmoidTransformation(short val, double depth, double rate, double center) {
			double f = val / depth;
			return static_cast<short>((depth) / (1.0 + std::exp((-rate) * (f - center))));
		}

		short altSigmoidTransformation(short val, double c) {
			return static_cast<short>(val + (val * c * (1 / (1 + std::exp((-1) * val) ) ) ) );
		}

		short negative(short val, short depth) {
			return depth - val;
		}
		short blackAndWhite(short val, short depth) {
			if (val > (depth / 4)) {
				return depth;
			}
			return 0;
		}


		short histogramTransformation(short val, const MKIHistogram& hist, short depth) {
			return static_cast<short>(depth * hist.eqData().at(val));
		}
	}

	namespace Math {
		double logTransC(short depth) {
			return ((depth) / std::log2(depth + 1)) ;
		}

		double exponTransA(short depth) {
			return (std::log(depth + 1) / (depth));
		}
	}
	
}


