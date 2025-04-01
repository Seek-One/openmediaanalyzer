/*
 * Size.cpp
 *
 *  Created on: 14 juin 2012
 *      Author: ebeuque
 */

#include "Size.h"

#include <cmath>


#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))

Size::Size() : width(0), height(0)
{

}

Size::Size(int width, int height) : width(width), height(height)
{

}

Size::~Size()
{

}

void Size::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

bool Size::equals(const Size& size) const
{
	return (width == size.width) && (height == size.height);
}

bool Size::isNull() const
{
	return (this->width == 0 && this->height == 0);
}

Size& Size::operator= (const Size& other)
{
	// protect against invalid self-assignment
	if (this != &other)
	{
		width = other.width;
		height = other.height;
	}
	return *this;
}

bool Size::operator== (const Size& other) const
{
	return equals(other);
}

bool Size::operator!= (const Size& other) const
{
	return !equals(other);
}

void Size::scaleToFit(Size& source, const Size& target)
{
	double ratio = MIN(((double)target.width / (double)source.width), ((double)target.height / (double)source.height));
	source.width = (int)floor(((double)source.width * ratio));
	source.height = (int)floor(((double)source.height * ratio));
}

void Size::scale(float factor)
{
	width = (int)(width*factor);
	height = (int)(height*factor);
}

void Size::scaleWidth(float factor)
{
	width = (int)(width*factor);
}

void Size::scaleHeight(float factor)
{
	height = (int)(height*factor);
}

bool Size::isDownScalable(int factor) const
{
	if((width % factor) == 0 && (height % factor) == 0){
		return true;
	}
	return false;
}