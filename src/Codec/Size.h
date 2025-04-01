/*
 * Size.h
 *
 *  Created on: 14 juin 2012
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CORE_TYPE_SIZE_H_
#define TOOLKIT_CORE_TYPE_SIZE_H_

class Size {
public:
	Size();
	Size(int width, int height);
	virtual ~Size();

	void setSize(int width, int height);

	bool equals(const Size& size) const;
	bool isNull() const;

	Size& operator= (const Size& other);
	bool operator== (const Size& other) const;
	bool operator!= (const Size& other) const;

	static void scaleToFit(Size& source, const Size& target);

	void scale(float factor);
	void scaleWidth(float factor);
	void scaleHeight(float factor);

	bool isDownScalable(int factor) const;

public:
	int width;
	int height;
};

#endif /* TOOLKIT_CORE_TYPE_SIZE_H_ */
