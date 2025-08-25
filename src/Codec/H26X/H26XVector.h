//
// Created by ebeuque on 25/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26X_H26XVECTOR_H
#define TOOLKIT_CODEC_UTILS_H26X_H26XVECTOR_H

#include <vector>

template<typename T>
class H26XVector : public std::vector<T>
{
public:
	H26XVector<T>() = default;

	inline void add(const T& item)
	{
		this->push_back(item);
	}
};

#endif //TOOLKIT_CODEC_UTILS_H26X_H26XVECTOR_H
