#include "Geometry.h"

namespace novus
{

void GeometryGenerator::CreateBox(float width, float height, float depth, Mesh& mesh)
{
	//
	// Create the vertices.
	//

	Vertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(Vector3(-w2, -h2, -d2), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f));
	v[1] = Vertex(Vector3(-w2, +h2, -d2), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f));
	v[2] = Vertex(Vector3(+w2, +h2, -d2), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f));
	v[3] = Vertex(Vector3(+w2, -h2, -d2), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f));

	// Fill in the back face vertex data.
	v[4] = Vertex(Vector3(-w2, -h2, +d2), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f));
	v[5] = Vertex(Vector3(+w2, -h2, +d2), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f));
	v[6] = Vertex(Vector3(+w2, +h2, +d2), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f));
	v[7] = Vertex(Vector3(-w2, +h2, +d2), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f));

	// Fill in the top face vertex data.
	v[8] = Vertex(Vector3(-w2, +h2, -d2), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
	v[9] = Vertex(Vector3(-w2, +h2, +d2), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
	v[10] = Vertex(Vector3(+w2, +h2, +d2), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));
	v[11] = Vertex(Vector3(+w2, +h2, -d2), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));

	// Fill in the bottom face vertex data.
	v[12] = Vertex(Vector3(-w2, -h2, -d2), Vector3(0.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f));
	v[13] = Vertex(Vector3(+w2, -h2, -d2), Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f));
	v[14] = Vertex(Vector3(+w2, -h2, +d2), Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 0.0f));
	v[15] = Vertex(Vector3(-w2, -h2, +d2), Vector3(0.0f, -1.0f, 0.0f), Vector2(1.0f, 0.0f));

	// Fill in the left face vertex data.
	v[16] = Vertex(Vector3(-w2, -h2, +d2), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f));
	v[17] = Vertex(Vector3(-w2, +h2, +d2), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f));
	v[18] = Vertex(Vector3(-w2, +h2, -d2), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f));
	v[19] = Vertex(Vector3(-w2, -h2, -d2), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f));

	// Fill in the right face vertex data.
	v[20] = Vertex(Vector3(+w2, -h2, -d2), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f));
	v[21] = Vertex(Vector3(+w2, +h2, -d2), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f));
	v[22] = Vertex(Vector3(+w2, +h2, +d2), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f));
	v[23] = Vertex(Vector3(+w2, -h2, +d2), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f));

	mesh.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh.Indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateBox(float width, float height, float depth, SimpleMesh& mesh)
{
	//
	// Create the vertices.
	//

	SimpleVertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

	// Fill in the front face SimpleVertex data.
	v[0] = SimpleVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f);
	v[1] = SimpleVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f);
	v[2] = SimpleVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f);
	v[3] = SimpleVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f);

	// Fill in the back face SimpleVertex data.
	v[4] = SimpleVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f);
	v[5] = SimpleVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f);
	v[6] = SimpleVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f);
	v[7] = SimpleVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f);

	// Fill in the top face SimpleVertex data.
	v[8] = SimpleVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f);
	v[9] = SimpleVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f);
	v[10] = SimpleVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f);
	v[11] = SimpleVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f);

	// Fill in the bottom face SimpleVertex data.
	v[12] = SimpleVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f);
	v[13] = SimpleVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f);
	v[14] = SimpleVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f);
	v[15] = SimpleVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f);

	// Fill in the left face SimpleVertex data.
	v[16] = SimpleVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f);
	v[17] = SimpleVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f);
	v[18] = SimpleVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f);
	v[19] = SimpleVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f);

	// Fill in the right face SimpleVertex data.
	v[20] = SimpleVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f);
	v[21] = SimpleVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f);
	v[22] = SimpleVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f);
	v[23] = SimpleVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f);

	mesh.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh.Indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateSphere(float radius, UINT sliceCount, UINT stackCount, Mesh& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(Vector3(0.0f, +radius, 0.0f), Vector3(0.0f, +1.0f, 0.0f), Vector2(0.0f, 0.0f));
	Vertex bottomVertex(Vector3(0.0f, -radius, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f));

	mesh.Vertices.push_back(topVertex);

	float phiStep = Math::Pi / stackCount;
	float thetaStep = 2.0f*Math::Pi / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Position.x = radius*sinf(phi)*cosf(theta);
			v.Position.y = radius*cosf(phi);
			v.Position.z = radius*sinf(phi)*sinf(theta);

			v.Normal = Normalize(v.Position);

			// Partial derivative of P with respect to theta
			v.Tangent.x = -radius*sinf(phi)*sinf(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = +radius*sinf(phi)*cosf(theta);

			v.Bitangent.x = 0.0f;
			v.Bitangent.y = radius*-sinf(phi);
			v.Bitangent.z = +radius*sinf(phi)*cosf(theta);

			v.Tangent = Normalize(v.Tangent);

			v.Bitangent = Normalize(v.Bitangent);

			v.Position = Normalize(v.Position);

			v.TexCoord.x = theta / Math::TwoPi;
			v.TexCoord.y = phi / Math::Pi;

			mesh.Vertices.push_back(v);
		}
	}

	mesh.Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= sliceCount; ++i)
	{
		mesh.Indices.push_back(0);
		mesh.Indices.push_back(i + 1);
		mesh.Indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			mesh.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)mesh.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		mesh.Indices.push_back(southPoleIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::Subdivide(Mesh& mesh)
{
	// Save a copy of the input geometry.
	Mesh inputCopy = mesh;


	mesh.Vertices.resize(0);
	mesh.Indices.resize(0);

	//       v1
	//       *
	//      / \
			//     /   \
			//  m0*-----*m1
	//   / \   / \
			//  /   \ /   \
			// *-----*-----*
	// v0    m2     v2

	UINT numTris = (UINT)inputCopy.Indices.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.Position = Vector3(
			0.5f*(v0.Position.x + v1.Position.x),
			0.5f*(v0.Position.y + v1.Position.y),
			0.5f*(v0.Position.z + v1.Position.z));

		m1.Position = Vector3(
			0.5f*(v1.Position.x + v2.Position.x),
			0.5f*(v1.Position.y + v2.Position.y),
			0.5f*(v1.Position.z + v2.Position.z));

		m2.Position = Vector3(
			0.5f*(v0.Position.x + v2.Position.x),
			0.5f*(v0.Position.y + v2.Position.y),
			0.5f*(v0.Position.z + v2.Position.z));

		//
		// Add new geometry.
		//

		mesh.Vertices.push_back(v0); // 0
		mesh.Vertices.push_back(v1); // 1
		mesh.Vertices.push_back(v2); // 2
		mesh.Vertices.push_back(m0); // 3
		mesh.Vertices.push_back(m1); // 4
		mesh.Vertices.push_back(m2); // 5

		mesh.Indices.push_back(i * 6 + 0);
		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 5);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 2);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 1);
		mesh.Indices.push_back(i * 6 + 4);
	}
}

void GeometryGenerator::CreateGeosphere(float radius, UINT numSubdivisions, Mesh& mesh)
{
	// Put a cap on the number of subdivisions.
	numSubdivisions = Math::Min(numSubdivisions, 5u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X), Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f), Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f), Vector3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
		1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
		3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
		10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
	};

	mesh.Vertices.resize(12);
	mesh.Indices.resize(60);

	for (UINT i = 0; i < 12; ++i)
		mesh.Vertices[i].Position = pos[i];

	for (UINT i = 0; i < 60; ++i)
		mesh.Indices[i] = k[i];

	for (UINT i = 0; i < numSubdivisions; ++i)
		Subdivide(mesh);

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < mesh.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		Vector3 n = Normalize(mesh.Vertices[i].Position);

		// Project onto sphere.
		Vector3 p = radius*n;

		mesh.Vertices[i].Position = p;
		mesh.Vertices[i].Normal = n;

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2(
			mesh.Vertices[i].Position.z,
			mesh.Vertices[i].Position.x);

		float phi = acosf(mesh.Vertices[i].Position.y / radius);

		mesh.Vertices[i].TexCoord.x = theta / Math::TwoPi;
		mesh.Vertices[i].TexCoord.y = phi / Math::Pi;

		// Partial derivative of P with respect to theta
		mesh.Vertices[i].Tangent.x = -radius*sinf(phi)*sinf(theta);
		mesh.Vertices[i].Tangent.y = 0.0f;
		mesh.Vertices[i].Tangent.z = +radius*sinf(phi)*cosf(theta);

		mesh.Vertices[i].Bitangent.x = 0.0f;
		mesh.Vertices[i].Bitangent.y = radius*-sinf(phi);
		mesh.Vertices[i].Bitangent.z = +radius*sinf(phi)*cosf(theta);

		mesh.Vertices[i].Tangent = Normalize(mesh.Vertices[i].Tangent);

		mesh.Vertices[i].Bitangent = Normalize(mesh.Vertices[i].Bitangent);
	}
}

void GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = Math::TwoPi / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Position = Vector3(r*c, y, r*s);

			vertex.TexCoord.x = (float)j / sliceCount;
			vertex.TexCoord.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			vertex.Tangent = Vector3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			Vector3 bitangent(dr*c, -height, dr*s);

			vertex.Normal = Normalize(Cross(vertex.Tangent, bitangent));

			mesh.Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			mesh.Indices.push_back(i*ringVertexCount + j);
			mesh.Indices.push_back((i + 1)*ringVertexCount + j);
			mesh.Indices.push_back((i + 1)*ringVertexCount + j + 1);

			mesh.Indices.push_back(i*ringVertexCount + j);
			mesh.Indices.push_back((i + 1)*ringVertexCount + j + 1);
			mesh.Indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, Mesh& mesh)
{
	UINT baseIndex = (UINT)mesh.Vertices.size();

	float y = 0.5f*height;
	float dTheta = Math::TwoPi / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Index of center vertex.
	UINT centerIndex = (UINT)mesh.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i + 1);
		mesh.Indices.push_back(baseIndex + i);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, Mesh& mesh)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)mesh.Vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = Math::TwoPi / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)mesh.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::CreateGrid(float width, float depth, UINT m, UINT n, Mesh& mesh)
{
	UINT vertexCount = m*n;
	UINT faceCount = (m - 1)*(n - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	mesh.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			mesh.Vertices[i*n + j].Position = Vector3(x, 0.0f, z);
			mesh.Vertices[i*n + j].Normal = Vector3(0.0f, 1.0f, 0.0f);
			mesh.Vertices[i*n + j].Tangent = Vector3(1.0f, 0.0f, 0.0f);
			mesh.Vertices[i*n + j].Bitangent = Vector3(0.0f, 1.0f, 0.0f);

			// Stretch texture over grid.
			mesh.Vertices[i*n + j].TexCoord.x = j*du;
			mesh.Vertices[i*n + j].TexCoord.y = i*dv;
		}
	}

	//
	// Create the indices.
	//

	mesh.Indices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			mesh.Indices[k] = i*n + j;
			mesh.Indices[k + 1] = i*n + j + 1;
			mesh.Indices[k + 2] = (i + 1)*n + j;

			mesh.Indices[k + 3] = (i + 1)*n + j;
			mesh.Indices[k + 4] = i*n + j + 1;
			mesh.Indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}
}

void GeometryGenerator::CreateFullscreenQuad(Mesh& mesh)
{
	mesh.Vertices.resize(4);
	mesh.Indices.resize(6);

	// Position coordinates specified in NDC space.
	mesh.Vertices[0] = Vertex(
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f);

	mesh.Vertices[1] = Vertex(
		-1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f);

	mesh.Vertices[2] = Vertex(
		+1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f);

	mesh.Vertices[3] = Vertex(
		+1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f);

	mesh.Indices[0] = 0;
	mesh.Indices[1] = 1;
	mesh.Indices[2] = 2;

	mesh.Indices[3] = 0;
	mesh.Indices[4] = 2;
	mesh.Indices[5] = 3;
}

}//namespace novus