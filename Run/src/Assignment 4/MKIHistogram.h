#pragma once

#include "MKIImage.h"

#include <vector>
#include <string>

namespace MKImage {
	class MKIHistogram {
	private:
		std::vector<double> m_Data;
		std::vector<double> m_EQData;
		double m_Avg, m_Var;
	public:
		MKIHistogram();
		MKIHistogram(const Image& image);

		const std::vector<double> data() const { return m_Data; }
		const std::vector<double> eqData() const { return m_EQData; }

		void make(const Image& image);
		void calcAvg();
		void calcVar();

		void save(const std::string& file);
		void saveEqualized(const std::string& file);

		void makeEqualized();
	};
}

