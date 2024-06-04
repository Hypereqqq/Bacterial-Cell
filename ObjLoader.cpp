#include "ObjLoader.h"

ObjLoader::ObjLoader() // konstruktor default
{
	;
}

ObjLoader::ObjLoader(string str) // konstruktor z wczytaniem
{
	load(str);
}

void ObjLoader::load(string str)
{
	triangles_cnt = 0;

	path = str;
	ifstream f(str);

	if (f.is_open())
	{
		while (getline(f, str))
		{
			stringstream line(str); 

			string firstChars;
			line >> firstChars;

			if (strcmp(firstChars.c_str(), "v") == 0)
			{
				float a, b, c;
				line >> a >> b >> c;

				out_vertices.emplace_back(a, b, c); // nie trzeba konstruktora jak w push back
			}
			else if (strcmp(firstChars.c_str(), "vt") == 0)
			{
				float a, b;
				line >> a >> b;

				out_uvs.emplace_back(a, b);
			}
			else if (strcmp(firstChars.c_str(), "vn") == 0)
			{
				float a, b, c;
				line >> a >> b >> c;

				out_normals.emplace_back(a, b, c);
			}
			else if (strcmp(firstChars.c_str(), "f") == 0)
			{
				for (int i = 0; i < 3; ++i)
				{
					unsigned int a, b, c;
					char ch;
					line >> a >> ch >> b >> ch >> c;

					vertexIndices.push_back(a - 1);
					uvIndices.push_back(b - 1);
					normalIndices.push_back(c - 1);
				}

				++triangles_cnt;
			}
		}
	}

	f.close();

	fillTab();
}

void ObjLoader::fillTab()
{
	for (int i = 0; i < triangles_cnt * 3; ++i)
	{
		unsigned int a = vertexIndices[i], b = uvIndices[i], c = normalIndices[i];

		finalTab.push_back(out_vertices[a].x);
		finalTab.push_back(out_vertices[a].y);
		finalTab.push_back(out_vertices[a].z);

		finalTab.push_back(out_uvs[b].x);
		finalTab.push_back(out_uvs[b].y);

		finalTab.push_back(out_normals[c].x);
		finalTab.push_back(out_normals[c].y);
		finalTab.push_back(out_normals[c].z);
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO); 

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * finalTab.size(), &finalTab[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

}


void ObjLoader::draw()
{
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, triangles_cnt * 3);
	glBindVertexArray(0);
}