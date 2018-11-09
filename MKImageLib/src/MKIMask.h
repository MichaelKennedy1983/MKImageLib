#pragma once

#include <array>
#include <vector>
#include <initializer_list>

namespace MKImage {
	//using ImageData = std::vector<std::vector<short>>;
	using ImageData = std::vector<std::vector<short>>;

	class Mask {
	private:
		std::vector<std::vector<short>> m_Mask;
		int m_Weight;
		size_t m_Rows;
		size_t m_Columns;
		int m_XOffset;
		int m_YOffset;
	public:
		Mask(std::initializer_list<std::initializer_list<short>> values);
		Mask(int weight, std::initializer_list<std::initializer_list<short>> values);
		
		short operator ()(const ImageData& image, size_t imageXCord, size_t imageYCord) const { return apply(image, imageXCord, imageYCord); }
		short apply(const ImageData& neighborhood) const;
		short apply(const ImageData& image, size_t imageXCord, size_t imageYCord) const;

		size_t rows() const { return m_Rows; }
		size_t columns() const { return m_Columns; }

	private:
		size_t findBoundedX(const ImageData& image, int offsetX) const;
		size_t findBoundedY(const ImageData& image, int offsetY, size_t smartX) const;

	public:
		static const Mask SMOOTH_3X3;
		static const Mask SMOOTH_5X5;
		static const Mask SMOOTH_9X9;
		static const Mask BLUR_5X5;
		static const Mask GAUSSIAN_BLUR_3X3;
		static const Mask GAUSSIAN_BLUR_5X5;
		static const Mask HEDGED_LAPLACIAN_3X3;
		static const Mask HEDGED_LAPLACIAN_5X5;
		static const Mask GAUSSIAN_HEDGED_LAPLACIAN_5X5;
		static const Mask EDGE_LAPLACIAN_3X3;
		static const Mask EDGE_LAPLACIAN_5X5;
		static const Mask HARD_EDGE_LAPLACIAN_5X5;
		static const Mask HARD_EDGE_LAPLACIAN_9X9;
	};
}

