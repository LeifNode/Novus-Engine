#include "OBJLoader.h"
#include "Utils/GameTimer.h"
#include "Application/Common.h"
#include <algorithm>

namespace novus
{

OBJLoader::OBJLoader()
	: mpFile(NULL)
{}

OBJLoader::~OBJLoader()
{
	NE_DELETE(mpFile);
	NE_DELETE(mpScene);
}

bool OBJLoader::Load(File* file)
{
	NE_DELETE(mpScene);

	GameTimer timer;
	timer.Start();
	float totalTime = 0.0f;

	if (!file->good())
	{
		return false;
	}

	std::wstring::const_iterator endIt;
	for (auto it = file->getPath().cbegin(); it != file->getPath().cend(); ++it) //Get path to OBJ file for relative path
	{
		if ((*it) == '\\' || (*it) == '/')
		{
			endIt = ++it;
		}
	}

	std::wstring relativePath(file->getPath().cbegin(), endIt);

	const int maxLineLength = 128;
	char line[maxLineLength];

	mpScene = NE_NEW assettypes::Scene;
	assettypes::Mesh* currentMesh = NE_NEW assettypes::Mesh();
	unsigned int vertexCount = 0;
	bool newMesh = false;
	bool readingFaces = false;

	std::vector<Vector3> vertices;
	std::vector<Vector2> vertexTexCoords;
	std::vector<Vector3> vertexNormals;

	std::vector<OBJFace> faces;

	while (file->good())
	{
		file->ReadLine(line, maxLineLength);

		if (line[0] == 'v')
		{
			//Done with previous mesh so clear face data and start again
			if (readingFaces)
			{
				readingFaces = false;

				StoreMesh(vertices, vertexTexCoords, vertexNormals, faces, vertexCount, currentMesh);

				faces.clear();

				vertexCount = 0;

				mpScene->mMeshes.push_back(currentMesh);

				currentMesh = NE_NEW assettypes::Mesh();
			}

			if (line[1] == ' ')
			{
				vertices.push_back(ParseVector3((char*)line));
			}
			else if (line[1] == 't')
			{
				vertexTexCoords.push_back(ParseVector2((char*)line));
			}
			else if (line[1] == 'n')
			{
				vertexNormals.push_back(ParseVector3((char*)line));
			}
		}
		else if (line[0] == 'f')
		{
			readingFaces = true;

			faces.push_back(ParseFace((char*)line));
			vertexCount += faces.back().VertexCount;
		}
		else if (memcmp(&line[0], "mtllib", 6) == 0)
		{
			std::string path(&line[7]);
			std::wstring wPath;

			//Get relative path for loading model
			wPath.append(relativePath.cbegin(), relativePath.cend());
			wPath.append(path.cbegin(), path.cend());
			
			LoadMatLib(wPath);
		}
		else if (memcmp(&line[0], "usemtl", 6) == 0)
		{
			if (readingFaces)
			{
				StoreMesh(vertices, vertexTexCoords, vertexNormals, faces, vertexCount, currentMesh);
				mpScene->mMeshes.push_back(currentMesh);

				currentMesh = NE_NEW assettypes::Mesh();

				faces.clear();

				vertexCount = 0;
			}

			currentMesh->mMaterialId = mMaterialRefMap[std::string(&line[7])];

		}
		//Unhandled obj arguments
		else if (line[0] == '#' || //Comment
			line[0] == ' ' ||
			line[0] == '\0' ||
			line[0] == '\n' ||
			line[0] == 'g' || //Group
			line[0] == 's' || //Smoothing group
			line[0] == 'm' || //Materials and merging groups
			//line[0] == 'u' ||
			line[0] == 'o') //Object name
		{
		}
		else //Not a valid obj file
		{
			std::cout << static_cast<unsigned int>(line[0]) << std::endl;

			NE_DELETE(currentMesh);
			NE_DELETE(mpScene);
			return false;
		}

	}

	StoreMesh(vertices, vertexTexCoords, vertexNormals, faces, vertexCount, currentMesh);
	mpScene->mMeshes.push_back(currentMesh);

	timer.Tick();
	std::cout << "Total time: " << timer.DeltaTime() << std::endl;

	return true;
}

Vector2 OBJLoader::ParseVector2(const char* str)
{
	Vector2 parsedVec;

	while (*str != ' ') str++;
	parsedVec.x = static_cast<float>(atof(str));
	while (*str == ' ') str++;
	while (*str != ' ') str++;
	parsedVec.y = static_cast<float>(atof(str));

	return parsedVec;
}

Vector3 OBJLoader::ParseVector3(const char* str)
{
	Vector3 parsedVec;

	while (*str != ' ') str++;
	parsedVec.x = static_cast<float>(atof(str));
	while (*str == ' ') str++;
	while (*str != ' ') str++;
	parsedVec.y = static_cast<float>(atof(str));
	while (*str == ' ') str++;
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
	meshOut->mFaceCount = static_cast<unsigned int>(faces.size());

	//TODO: Fix memory tracking for faces if it's possible
	//meshOut->mFaces = reinterpret_cast<assettypes::Face*>(NE_NEW char[sizeof(assettypes::Face) * faces.size()]);
	meshOut->mFaces = new assettypes::Face[faces.size()];
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

	unsigned int currentVertex = 0;

	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		unsigned int indexCount = faces[i].VertexCount;

		for (int v = 0; v < faces[i].VertexCount; v++)
		{
			int vertIndex = faces[i].Vertices[v].x;
			meshOut->mFaces[i].mIndices[v] = currentVertex;

			if (vertIndex < 0)
				vertIndex = static_cast<int>(pos.size()) + vertIndex;
			else
				vertIndex--;

			meshOut->mVertices[currentVertex] = pos[vertIndex];

			if (faces[i].HasTexCoords)
			{
				int texIndex = faces[i].Vertices[v].y;
				if (texIndex < 0)
					texIndex = static_cast<int>(tex.size()) + texIndex;
				else
					texIndex--;

				meshOut->mTextureCoords[currentVertex] = tex[texIndex];
			}

			if (faces[i].HasNormals)
			{

				int normIndex = faces[i].Vertices[v].z;
				if (normIndex < 0)
					normIndex = static_cast<int>(norm.size()) + normIndex;
				else
					normIndex--;

				meshOut->mNormals[currentVertex] = norm[normIndex];
			}

			currentVertex++;
		}

		meshOut->mFaces[i].mIndexCount = indexCount;
	}
}

void OBJLoader::LoadMatLib(const std::wstring& path)
{
	File* matLibFile = NE_NEW File();
	matLibFile->Load(path);

	if (!matLibFile->good())
	{
		NE_WARN("MtlLib file could not be opened.", "OBJLoader");
	}

	ParseMaterials(matLibFile);

	NE_DELETE(matLibFile);
}

void OBJLoader::ParseMaterials(File* file)
{
	std::wstring::const_iterator endIt;
	for (auto it = file->getPath().cbegin(); it != file->getPath().cend(); ++it) //Get path to OBJ file for relative path
	{
		if ((*it) == '\\' || (*it) == '/')
		{
			endIt = ++it;
		}
	}

	std::wstring relativePath(file->getPath().cbegin(), endIt);

	assettypes::Material* currentMaterial = NULL;

	while (file->good())
	{
		const int maxLineSize = 128;
		char line[maxLineSize];
		int index = 0;

		std::string texPath;
		std::wstring fullTexPath;

		fullTexPath = relativePath;

		file->ReadLine(line, maxLineSize);

		while (line[index] != '\0' && (line[index] == ' ' || line[index] == '\t'))
			index++;

		if (memcmp(&line[index], "newmtl", 6) == 0)
		{
			if (currentMaterial != NULL)
			{
				mMaterialRefMap[currentMaterial->name] = mpScene->mMaterials.size();
				mpScene->mMaterials.push_back(currentMaterial);
			}

			currentMaterial = NE_NEW assettypes::Material();
			currentMaterial->name = std::string(&line[index + 7]);//newmtl + 1 space
		}
		else if (line[index] == 'N')
		{
			if (line[index + 1] == 's') //Ns is specular power
			{
				currentMaterial->specularPow = atof(&line[index + 2]);
			}
		}
		else if (line[index] == 'K')
		{
			index++;

			const char* vecStart = &line[index + 1];

			switch (line[index])
			{
			case 'a': //Ambient
				currentMaterial->ambient = ParseVector3(vecStart);
				break;
			case 'd': //Diffuse
				currentMaterial->diffuse = Vector4(ParseVector3(vecStart), 1.0f);
				break;
			case 's': //Specular
				currentMaterial->specular = ParseVector3(vecStart);
				break;
			case 'e': //Emissive
				currentMaterial->emissive = ParseVector3(vecStart);
				break;
			}
		}
		else if (line[index] == 'm') //maps
		{
			if (memcmp(&line[index], "map_Ka", 6) == 0 || 
				memcmp(&line[index], "map_Kd", 6) == 0) //Ambient and diffuse maps are normally the same
			{
				index += 7;
				texPath = std::string(&line[index]);
				fullTexPath.append(texPath.cbegin(), texPath.cend());
				std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
				currentMaterial->texturePaths[assettypes::TextureType::Diffuse] = fullTexPath;
			}
			else if (memcmp(&line[index], "map_Ks", 6) == 0)
			{
				index += 7;
				texPath = std::string(&line[index]);
				fullTexPath.append(texPath.cbegin(), texPath.cend());
				std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
				currentMaterial->texturePaths[assettypes::TextureType::Specular] = fullTexPath;
			}
			else if (memcmp(&line[index], "map_Ns", 6) == 0)
			{
				index += 7;
				texPath = std::string(&line[index]);
				fullTexPath.append(texPath.cbegin(), texPath.cend());
				std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
				currentMaterial->texturePaths[assettypes::TextureType::Shininess] = fullTexPath;
			}
			else if (memcmp(&line[index], "map_d", 5) == 0)
			{
				index += 6;
				texPath = std::string(&line[index]);
				fullTexPath.append(texPath.cbegin(), texPath.cend());
				std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
				currentMaterial->texturePaths[assettypes::TextureType::Mask] = fullTexPath;
			}
			else if (memcmp(&line[index], "map_bump", 8) == 0)
			{
				index += 9;
				texPath = std::string(&line[index]);
				fullTexPath.append(texPath.cbegin(), texPath.cend());
				std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
				currentMaterial->texturePaths[assettypes::TextureType::Normal] = fullTexPath;
			}
		}
		else if (memcmp(&line[index], "bump", 4) == 0)//Non-standard way of storing bump maps
		{
			index += 5;
			texPath = std::string(&line[index]);
			fullTexPath.append(texPath.cbegin(), texPath.cend());
			std::replace(fullTexPath.begin(), fullTexPath.end(), '\\', '/');
			currentMaterial->texturePaths[assettypes::TextureType::Normal] = fullTexPath;
		}
	}

	if (currentMaterial != NULL)
	{	
		mMaterialRefMap[currentMaterial->name] = mpScene->mMaterials.size();
		mpScene->mMaterials.push_back(currentMaterial);
	}
}

}