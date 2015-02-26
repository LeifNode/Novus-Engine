#include "OBJLoader.h"


namespace novus
{

OBJLoader::OBJLoader()
	: mpFile(NULL),
	mpScene(NULL)
{}

OBJLoader::~OBJLoader()
{
	NE_DELETE(mpFile);
	NE_DELETE(mpScene);
}

bool OBJLoader::Load(const std::wstring& path)
{
	NE_DELETE(mpFile);
	NE_DELETE(mpScene);

	mpFile = NE_NEW std::ifstream(path);

	if (!mpFile->good())
	{
		NE_DELETE(mpFile);
		return false;
	}

	char line[128];

	mpScene = NE_NEW assettypes::Scene;
	assettypes::Mesh* currentMesh = NE_NEW assettypes::Mesh();
	unsigned int vertexCount = 0;
	bool newMesh = false;
	bool readingFaces = false;

	std::vector<Vector3> vertices;
	std::vector<Vector2> vertexTexCoords;
	std::vector<Vector3> vertexNormals;

	std::vector<OBJFace> faces;

	while (mpFile->good())
	{
		mpFile->getline(line, 128);

		if (line[0] == 'v')
		{
			//Done with previous mesh so clear vertex data and start again
			if (readingFaces)
			{
				readingFaces = false;
				vertexCount = 0;

				StoreMesh(vertices, vertexTexCoords, vertexNormals, faces, vertexCount, currentMesh);

				vertices.clear();
				vertexTexCoords.clear();
				vertexNormals.clear();
				faces.clear();

				mpScene->mMeshes.push_back(currentMesh);

				currentMesh = NE_NEW assettypes::Mesh();
			}

			if (line[1] == ' ')
			{
				vertices.push_back(ParseVector3(line));
			}
			else if (line[1] == 't')
			{
				vertexTexCoords.push_back(ParseVector2(line));
			}
			else if (line[1] == 'n')
			{
				vertexNormals.push_back(ParseVector3(line));
			}
		}
		else if (line[0] == 'f')
		{
			readingFaces = true;

			faces.push_back(ParseFace(line));
			vertexCount += faces.back().VertexCount;
		}

	}

	NE_DELETE(mpFile);

	return true;
}

Vector2 OBJLoader::ParseVector2(const char* str)
{
	Vector2 parsedVec;

	while (*str != ' ') str++;
	parsedVec.x = static_cast<float>(atof(str));
	str++;
	while (*str != ' ') str++;
	parsedVec.y = static_cast<float>(atof(str));

	return parsedVec;
}

Vector3 OBJLoader::ParseVector3(const char* str)
{
	Vector3 parsedVec;

	while (*str != ' ') str++;
	parsedVec.x = static_cast<float>(atof(str));
	str++;
	while (*str != ' ') str++;
	parsedVec.y = static_cast<float>(atof(str));
	str++;
	while (*str != ' ') str++;
	parsedVec.z = static_cast<float>(atof(str));

	return parsedVec;
}

OBJLoader::OBJFace OBJLoader::ParseFace(const char* str)
{
	OBJFace face;
	face.HasNormals = false;
	face.HasTexCoords = false;

	const char* strPtr = str;

	strPtr++;
	while (*strPtr == ' ') strPtr++;

	int count = 0;

	while (*strPtr != '\0')
	{
		//std::cout << "Char: "<< strPtr << std::endl;

		face.Vertices[count].x = atoi(strPtr);//Get vertex position index
		while (*strPtr != ' ' && *strPtr != '\0' && *strPtr != '/') strPtr++;

		if (*strPtr == '/')
		{
			strPtr++;
			
			if (*strPtr != '/') //If we have a texture coordinate
			{
				face.HasTexCoords = true;
				face.Vertices[count].y = atoi(strPtr);

				while (*strPtr != ' ' && *strPtr != '\0' && *strPtr != '/') strPtr++;
				if (*strPtr == '/')
					strPtr++;
			}
			else if (*strPtr == '/')//If we have a second slash indicating a normal
			{
				strPtr++;
			}

			if (*strPtr != ' ')//If there's no space then we have a normal index
			{
				face.HasNormals = true;
				face.Vertices[count].z = atoi(strPtr);
			}
		}

		while (*strPtr != ' ' && *strPtr != '\0') strPtr++;
		while (*strPtr == ' ') strPtr++;

		count++;
	}

	face.VertexCount = count;

	return face;
}

void OBJLoader::StoreMesh(const std::vector<Vector3>& pos,
						  const std::vector<Vector2>& tex,
						  const std::vector<Vector3>& norm,
						  const std::vector<OBJFace>& faces,
						  unsigned int vertexCount,
						  assettypes::Mesh* meshOut)
{
	meshOut->mFaceCount = faces.size();
	meshOut->mFaces = NE_NEW assettypes::Face[faces.size()];
	meshOut->mVertices = NE_NEW Vector3[vertexCount];
	meshOut->mVertexCount = vertexCount;

	if (tex.size() > 0)
	{
		meshOut->mTextureCoords = NE_NEW Vector2[vertexCount];
	}
	if (norm.size() > 0)
	{
		meshOut->mNormals = NE_NEW Vector3[vertexCount];
	}

	unsigned int currentVertex;

	for (size_t i = 0; i < faces.size(); ++i)
	{
		for (int v = 0; v < faces[i].VertexCount; v++)
		{
			int vertIndex = faces[i].Vertices[v].x;

			if (vertIndex < 0)
				vertIndex = pos.size() + vertIndex;

			meshOut->mVertices[currentVertex] = pos[vertIndex];

			if (faces[i].HasTexCoords)
			{
				int texIndex = faces[i].Vertices[v].y;
				if (texIndex < 0)
					texIndex = tex.size() + texIndex;

				meshOut->mTextureCoords[currentVertex] = tex[texIndex];
			}

			if (faces[i].HasNormals)
			{

				int normIndex = faces[i].Vertices[v].z;
				if (normIndex < 0)
					normIndex = norm.size() + normIndex;

				meshOut->mNormals[currentVertex] = norm[normIndex];
			}

			currentVertex++;
		}
	}
}

assettypes::Scene* OBJLoader::getScene() const
{
	return mpScene;
}

}