#include "MKIMask.h"

#include <exception>

namespace MKImage {
	Mask::Mask(std::initializer_list<std::initializer_list<short>> values)
		: m_Mask(values.size()) {

		auto initIter = values.begin();
		for (auto& i : m_Mask) {
			i = *initIter;
			++initIter;
		}

		m_Weight = 0;
		for (auto i : m_Mask) {
			for (auto j : i) {
				m_Weight += j;
			}
		}

		if (m_Weight == 0) m_Weight = 1;

		m_Rows = m_Mask.size();
		m_Columns = m_Mask.at(0).size();
		m_XOffset = m_Rows / 2;
		m_YOffset = m_Columns / 2;
	}

	Mask::Mask(int weight, std::initializer_list<std::initializer_list<short>> values) 
		: m_Mask( values.size()), m_Weight(weight) {

		auto initIter = values.begin();
		for (auto& i : m_Mask) {
			i = *initIter;
			++initIter;
		}
	}

	short Mask::apply(const ImageData& image, size_t imageXCord, size_t imageYCord) const {
		int xOffsetCord = static_cast<int>(imageXCord) - m_XOffset;
		int yOffsetCord = static_cast<int>(imageYCord) - m_YOffset;

		long long sum = 0;
		for (int i = 0; i < m_Mask.size(); ++i) {
			size_t boundedX = findBoundedX(image, xOffsetCord + i);

			for (int j = 0; j < m_Mask.at(i).size(); ++j) {
				int boundedY = findBoundedY(image, yOffsetCord + j, boundedX);

				sum += m_Mask.at(i).at(j) * image.at(boundedX).at(boundedY);
			}
		}
		return static_cast<short>(sum / m_Weight);
	}

	size_t Mask::findBoundedX(const ImageData& image, int offsetX) const {
		size_t boundedX;
		if (offsetX < 0) {
			boundedX = -(offsetX);
		}
		else if (offsetX > image.size() - 1) {
			boundedX = (image.size() - 1) - (offsetX - ((image.size() - 1)));
		}
		else {
			boundedX = offsetX;
		}
		return boundedX;
	}

	size_t Mask::findBoundedY(const ImageData& image, int offsetY, size_t boundedX) const {
		size_t boundedY;
		if (offsetY < 0) {
			boundedY = -(offsetY);
		}
		else if (offsetY > image.at(boundedX).size() - 1) {
			boundedY = (image.at(boundedX).size() - 1) - (offsetY - ((image.at(boundedX).size() - 1)));
		}
		else {
			boundedY = offsetY;
		}
		return boundedY;
	}

	const Mask Mask::SMOOTH_3X3{ { 0, 1, 0},
								 { 1, 2, 1},
								 { 0, 1, 0} };

	const Mask Mask::SMOOTH_5X5{ { 0,  1,  2,  1,  0},
								 { 1,  2,  4,  2,  1},
								 { 2,  4,  8,  4,  2},
								 { 1,  2,  4,  2,  1},
								 { 0,  1,  2,  1,  0} };

	const Mask Mask::SMOOTH_9X9{ {  0,   1,   2,   4,   8,   4,   2,   1,   0},
								 {  1,   2,   4,   8,  16,   8,   4,   2,   1},
								 {  2,   4,   8,  16,  32,  16,   8,   4,   2},
								 {  4,   8,  16,  32,  64,  32,  16,   8,   4},
								 {  8,  16,  32,  64, 128,  64,  32,  16,   8},
								 {  4,   8,  16,  32,  64,  32,  16,   8,   4},
								 {  2,   4,   8,  16,  32,  16,   8,   4,   2},
								 {  1,   2,   4,   8,  16,   8,   4,   2,   1},
								 {  0,   1,   2,   4,   8,   4,   2,   1,   0} };

	const Mask Mask::BLUR_5X5{ { 1,  1,  1,  1,  1},
							   { 1,  0,  0,  0,  1},
							   { 1,  0,  0,  0,  1},
							   { 1,  0,  0,  0,  1},
							   { 1,  1,  1,  1,  1} };

	const Mask Mask::GAUSSIAN_BLUR_3X3{ { 1,  2,  1},
										{ 2,  4,  1},
										{ 1,  2,  1} };

	const Mask Mask::GAUSSIAN_BLUR_5X5{ {  2,  4,  5,  4,  2},
									    {  4,  9, 12,  9,  4},
									    {  5, 12, 15, 12,  5},
									    {  4,  9, 12,  9,  4},
									    {  2,  4,  5,  4,  2}  };

	const Mask Mask::HEDGED_LAPLACIAN_3X3{ {-1, -1, -1},
										   {-1,  9, -1},
										   {-1, -1, -1} };

	const Mask Mask::HEDGED_LAPLACIAN_5X5{ { -1, -1, -1, -1, -1},
										   { -1, -1, -2, -1, -1},
										   { -1, -2, 29, -2, -1},
										   { -1, -1, -2, -1, -1},
										   { -1, -1, -1, -1, -1} };

	const Mask Mask::GAUSSIAN_HEDGED_LAPLACIAN_5X5{ { -1,  -4,  -6,  -4,  -1},
												    { -4, -16, -24, -16,  -4},
												    { -6, -24, 476, -24,  -6},
												    { -4, -16, -24, -16,  -4},
												    { -1,  -4,  -6,  -4,  -1} };

	const Mask Mask::EDGE_LAPLACIAN_3X3{ {-1, -1, -1},
										 {-1,  8, -1},
										 {-1, -1, -1} };

	const Mask Mask::EDGE_LAPLACIAN_5X5{ {  0,  0, -1,  0,  0},
										 {  0, -1, -1, -1,  0},
										 { -1, -1, 12, -1, -1},
										 {  0, -1, -1, -1,  0},
										 {  0,  0, -1,  0,  0} };

	const Mask Mask::HARD_EDGE_LAPLACIAN_5X5{ {  0, -1, -1, -1,  0},
											  { -1, -1, -1, -1, -1},
											  { -1, -1, 20, -1, -1},
											  { -1, -1, -1, -1, -1},
											  {  0, -1, -1, -1,  0} };

	const Mask Mask::HARD_EDGE_LAPLACIAN_9X9{ {  0, -1, -1, -1, -1, -1, -1, -1,  0},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  { -1, -1, -1, -1, 76, -1, -1, -1, -1},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  { -1, -1, -1, -1, -1, -1, -1, -1, -1},
											  {  0, -1, -1, -1, -1, -1, -1, -1,  0} };
}

