#include "meshLoader.h"
#include "obj\object.h"
#include "mesh\mesh.h"
#include "material\material.h"

#include <string>
#include<vector>
using namespace std;
static char* parseRow(char* buf, char* bufEnd, char* row, int len)
{
	bool start = true;
	bool done = false;
	int n = 0;
	while (!done && buf < bufEnd)
	{
		char c = *buf;
		buf++;
		// multirow
		switch (c)
		{
		case '\\':
			break;
		case '\n':
			if (start) break;
			done = true;
			break;
		case '\r':
			break;
		case '\t':
		case ' ':
			if (start) break;
			// else falls through
		default:
			start = false;
			row[n++] = c;
			if (n >= len - 1)
				done = true;
			break;
		}
	}
	row[n] = '\0';
	return buf;
}

static int parseFace(char* row, int* data, int n, int vcnt)
{
	int j = 0;
	while (*row != '\0')
	{
		// Skip initial white space
		while (*row != '\0' && (*row == ' ' || *row == '\t'))
			row++;
		char* s = row;
		// Find vertex delimiter and terminated the string there for conversion.
		while (*row != '\0' && *row != ' ' && *row != '\t')
		{
			if (*row == '/') *row = '\0';
			row++;
		}
		if (*s == '\0')
			continue;
		int vi = atoi(s);
		data[j++] = vi < 0 ? vi + vcnt : vi - 1;
		if (j >= n) return j;
	}
	return j;
}
vector<Object*> loadObj(const std::string& filename) {
	vector<Object*> objs;
	char* buf = 0;
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp)
		return objs;
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		fclose(fp);
		return objs;
	}
	long bufSize = ftell(fp);
	if (bufSize < 0)
	{
		fclose(fp);
		return objs;
	}
	if (fseek(fp, 0, SEEK_SET) != 0)
	{
		fclose(fp);
		return objs;
	}
	buf = new char[bufSize];
	if (!buf)
	{
		fclose(fp);
		return objs;
	}
	size_t readLen = fread(buf, bufSize, 1, fp);
	fclose(fp);

	if (readLen != 1)
	{
		delete[] buf;
		return objs;
	}

	char* src = buf;
	char* srcEnd = buf + bufSize;
	char row[512];
	int face[32];
	float x, y, z;
	int nv;
	int vcap = 0;
	int tcap = 0;

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::ivec3> tris;

	std::string objName;
	while (src < srcEnd)
	{
		// Parse one row
		row[0] = '\0';
		src = parseRow(src, srcEnd, row, sizeof(row) / sizeof(char));
		// Skip comments
		if (row[0] == '#') continue;

		if (row[0] == 'g') {
			//read name
			
			if (verts.size() > 0) {
				Mesh* mesh = new Mesh(verts, normals, uvs, tris);
				Object* obj =new Object(mesh, new Material("simple"));
				obj->setName(objName);
				objs.push_back(obj);
				verts.clear();
				normals.clear();
				uvs.clear();
				tris.clear();

			}

			objName = row[2];
		}

		if (row[0] == 'v' && row[1] != 'n' && row[1] != 't')
		{
			// Vertex pos
			sscanf(row + 1, "%f %f %f", &x, &y, &z);
			//addVertex(x, y, z, vcap);
			verts.push_back(glm::vec3(x, y, z));
		}

		if (row[0] == 'f')
		{
			// Faces
			size_t vertCount = verts.size();
			nv = parseFace(row + 1, face, 32, vertCount);
			for (int i = 2; i < nv; ++i)
			{
				const int a = face[0];
				const int b = face[i - 1];
				const int c = face[i];
				if (a < 0 || a >= vertCount || b < 0 || b >= vertCount || c < 0 || c >= vertCount)
					continue;
				//addTriangle(a, b, c, tcap);
				tris.push_back(glm::ivec3(a, b, c));
			}
		}
	}
	if (verts.size() > 0) {
		Mesh* mesh = new Mesh(verts, normals, uvs, tris);
		Object* obj = new Object(mesh, new Material("simple"));
		obj->setName(objName);
		objs.push_back(obj);
		verts.clear();
		normals.clear();
		uvs.clear();
		tris.clear();

	}
	delete[] buf;

	// Calculate normals.
	//m_normals = new float[m_triCount * 3];
	//for (int i = 0; i < m_triCount * 3; i += 3)
	//{
	//	const float* v0 = &m_verts[m_tris[i] * 3];
	//	const float* v1 = &m_verts[m_tris[i + 1] * 3];
	//	const float* v2 = &m_verts[m_tris[i + 2] * 3];
	//	float e0[3], e1[3];
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		e0[j] = v1[j] - v0[j];
	//		e1[j] = v2[j] - v0[j];
	//	}
	//	float* n = &m_normals[i];
	//	n[0] = e0[1] * e1[2] - e0[2] * e1[1];
	//	n[1] = e0[2] * e1[0] - e0[0] * e1[2];
	//	n[2] = e0[0] * e1[1] - e0[1] * e1[0];
	//	float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	//	if (d > 0)
	//	{
	//		d = 1.0f / d;
	//		n[0] *= d;
	//		n[1] *= d;
	//		n[2] *= d;
	//	}
	//}

	//m_filename = filename;

	return objs;
}