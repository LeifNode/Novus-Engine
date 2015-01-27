#include "RectangleBinPacker.h"
#include "Memory/Memory.h"
#include <Windows.h>

using novus::RectangleBinPacker;

RectangleBinPacker::Node::Node()
{
	ZeroMemory(this, sizeof(RectangleBinPacker::Node));
}

RectangleBinPacker::Node::~Node()
{
	NE_DELETE(left);
	NE_DELETE(right);
}

RectangleBinPacker::RectangleBinPacker()
{
	mBaseWidth = 0;
	mBaseHeight = 0;
	mFilledArea = 0;
}
RectangleBinPacker::~RectangleBinPacker()
{
	NE_DELETE(mRoot.left);
	NE_DELETE(mRoot.right);
}

void RectangleBinPacker::Init(int width, int height)
{
	NE_DELETE(mRoot.left);
	NE_DELETE(mRoot.right);

	mFilledArea = 0;
	mRoot.x = mRoot.y = 0;
	mRoot.width = width;
	mRoot.height = height;
	mBaseWidth = width;
	mBaseHeight = height;
}

RectangleBinPacker::Node* RectangleBinPacker::Insert(int width, int height)
{
	return Insert(&mRoot, width, height);
}

RectangleBinPacker::Node* RectangleBinPacker::Insert(Node* node, int width, int height)
{
	if (node->left || node->right)
	{
		if (node->left)
		{
			Node* newNode = Insert(node->left, width, height);
			if (newNode)
				return newNode;
		}
		if (node->right)
		{
			Node* newNode = Insert(node->right, width, height);
			if (newNode)
				return newNode;
		}
		return 0;
	}

	//Is node too small?
	if (width > node->width || height > node->height)
		return NULL;

	//Split remaining space along the shorter axis
	int w = node->width - width;
	int h = node->height - height;
	node->left = NE_NEW Node();
	node->right = NE_NEW Node();
	if (w <= h) //Horizontal split
	{
		node->left->x = node->x + width;
		node->left->y = node->y;
		node->left->width = w;
		node->left->height = height;

		node->right->x = node->x;
		node->right->y = node->y + height;
		node->right->width = node->width;
		node->right->height = h;
	}
	else //Vertical split
	{
		node->left->x = node->x;
		node->left->y = node->y + height;
		node->left->width = width;
		node->left->height = h;

		node->right->x = node->x + width;
		node->right->y = node->y;
		node->right->width = w;
		node->right->height = node->height;
	}

	mFilledArea += width * height;

	node->width = width;
	node->height = height;
	return node;
}

int RectangleBinPacker::getFilledArea()
{
	/*std::stack<RectangleBinPacker::Node*> stack;
	int totalArea = 0;

	stack.push(&mRoot);

	while (stack.size() > 0)
	{
	RectangleBinPacker::Node* node = stack.top();
	stack.pop();

	if (node == NULL)
	continue;

	if (node->left || node->right)
	{
	totalArea += node->width * node->height;

	stack.push(node->left);
	stack.push(node->right);
	}
	}

	return totalArea;*/

	return mFilledArea;
}

float RectangleBinPacker::getFillPercent()
{
	return getFilledArea() * 100.0f / (float)(mBaseWidth * mBaseHeight);
}