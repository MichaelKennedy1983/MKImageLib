#pragma once

#include "MKIFileType.h"
#include "MKIMask.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>

namespace MKImage {
	using ImageData = std::vector<std::vector<short>>;
	using Path = std::filesystem::path;
	namespace FS = std::filesystem;

	/* Data representing an image */
	class Image {
	public:
		Image();
		explicit Image(const std::string& file);
		~Image() {}
		Image(const Image& other);
		Image(const Image&& other);

		int rows() const { return m_Rows; }
		int columns() const { return m_Columns; }
		short depth() const { return m_Depth; }
		short minValue() const { return m_MinLevel; }
		short maxValue() const { return m_MaxLevel; }
		const ImageData& data() const { return m_Body; }
		bool isBadImage() const { return m_BadImage; }

		// Ensures the pixel "val" is not greater than the depth or less than 0
		void protectRange(short& val);
		// Updates m_minValue or m_maxValue if pixel "val" is less than or greater than those values.
		void updateMinMax(short val);

		void load(const std::string& file);
		void save(const std::string& file, const std::string& comment = "");
		// Appends _COPY to the end of filename
		void saveCopy(const std::string& comment = "");

		/*
			Applies a function to every pixel of the image.

			Func f = a function
			Args... values = arguments to pass to function f
		*/
		template<typename Func, typename ...Args>
		void pointProcess(Func f, Args... values);

		/*
			Applies a function to every pixel of the image.
			Uses concurency.

			Func f = a function
			Args... values = arguments to pass to function f
		*/
		template<typename Func, typename ...Args>
		void parallelPP(Func f, Args... values);

		void parallelMasking(const Mask& mask);
	private:
		/* #################### Private methods #################### */

		void readHeader(const Path& file);
		void removeComment(std::ifstream& in) const;
		void skipHeader(std::ifstream& in) const;
		void loadBin(const Path& file);
		void loadText(const Path& file);
		void saveBin(const Path& file, const std::string& comment = "");
		void saveText(const Path& file, const std::string& comment = "");

	private:
		Path m_File;
		FileType m_FileType;
		int m_Rows, m_Columns;
		short m_Depth, m_MinLevel, m_MaxLevel;
		ImageData m_Body;
		std::mutex m_MinMaxMutex;
		bool m_BadImage;

	private:
		/* #################### Functors #################### */

		/*
			A functor which performs point processing on a region of an image.
			Designed to be used in conjunction with Image::parallelPP() and std::thread
		*/
		class PointProcessFunct {
		public:
			PointProcessFunct(Image& image, ImageData& out, ImageData::iterator begin, ImageData::iterator end);

			template<typename Func, typename ...Args>
			void operator ()(Func func, Args... values) {
				int min = m_Image.depth();
				int max = 0;

				size_t countI = m_Begin - m_Image.data().begin();
				size_t countJ;
				for (m_Begin; m_Begin != m_End; ++m_Begin) {
					countJ = 0;
					for (auto j : *m_Begin) {
						short val = func(j, values...);

						m_Image.protectRange(val);

						m_Out.at(countI).at(countJ) = val;

						if (val < min)
							min = val;
						if (val > max)
							max = val;

						++countJ;
					}
					++countI;
				}

				m_Image.updateMinMax(min);
				m_Image.updateMinMax(max);
			}

		private:
			Image& m_Image;
			ImageData& m_Out;
			ImageData::iterator m_Begin;
			ImageData::iterator m_End;
		};

		class MaskProcessFunct {
		public:
			MaskProcessFunct(Image& image, ImageData& out, ImageData::iterator begin, ImageData::iterator end);
			void operator ()(const Mask& mask);
		private:
			Image& m_Image;
			ImageData& m_Out;
			ImageData::iterator m_Begin;
			ImageData::iterator m_End;
		};

		class FrameProcessFunct {
		public:
			enum class Operations { unknown = 0, add, sub, mult };

		public:
			FrameProcessFunct(Image& image, Image& otherImage, ImageData& out, ImageData::iterator begin,
							  ImageData::iterator end, Operations operation);
			void operator()();
		private:
			std::function<short(short, short)> operation();
		private:
			Image& m_Image;
			Image& m_OtherImage;
			ImageData& m_Out;
			ImageData::iterator m_Begin;
			ImageData::iterator m_End;
			Operations m_Operation;
		};

		public:
			using FrameOps = FrameProcessFunct::Operations;
			void frameProcessing(Image& otherImage, FrameOps operation);
	};
	
	/* #################### End of Image class definition #################### */

	/* #################### Template method definitions #################### */

	template <typename Func, typename ...Args>
	void Image::pointProcess(Func f, Args... values) {
		for (auto& i : m_Body) {
			for (auto& j : i) {
				for (auto& k : j) {
					k = f(k, values...);
					protectRange(k);
					updateMinMax(k);
				}
			}
		}
	}

	template<typename Func, typename ...Args>
	void Image::parallelPP(Func f, Args... values) {
		ImageData::iterator mid = m_Body.begin() + (std::distance(m_Body.begin(), m_Body.end()) / 2);
		ImageData::iterator oneQuarter = m_Body.begin() + (std::distance(m_Body.begin(), mid) / 2);
		ImageData::iterator threeQuarter = mid + (std::distance(mid, m_Body.end()) / 2);

		ImageData temp(m_Body.size(), std::vector<short>(m_Body.at(0).size()));

		Image::PointProcessFunct ppFirst(*this, temp, m_Body.begin(), oneQuarter);
		Image::PointProcessFunct ppSecond(*this, temp, oneQuarter, mid);
		Image::PointProcessFunct ppThird(*this, temp, mid, threeQuarter);
		Image::PointProcessFunct ppFourth(*this, temp, threeQuarter, m_Body.end());

		std::thread ppThreadOne(ppFirst, f, values...);
		std::thread ppThreadTwo(ppSecond, f, values...);
		std::thread ppThreadThree(ppThird, f, values...);
		std::thread ppThreadFour(ppFourth, f, values...);

		ppThreadOne.join();
		ppThreadTwo.join();
		ppThreadThree.join();
		ppThreadFour.join();

		m_Body = std::move(temp);
	}
}

