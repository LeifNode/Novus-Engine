//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Based off of code by Jukka Jylänki and http://www.blackpawn.com/texts/lightmaps/default.html
#pragma once


namespace novus
{
class RectangleBinPacker
{
public:
	struct Node
	{
		Node();
		~Node();

		Node* left;
		Node* right;

		int x, y;
		int width, height;
	};

public:
	RectangleBinPacker();
	~RectangleBinPacker();

	void Init(int width, int height);

	Node* Insert(int width, int height);

	int getWidth() const { return mBaseWidth; }
	int getHeight() const { return mBaseHeight; }

	int getFilledArea();

	float getFillPercent();

private:
	Node* Insert(Node* node, int width, int height);

private:
	Node mRoot;

	int mFilledArea;
	int mBaseWidth;
	int mBaseHeight;
};
}