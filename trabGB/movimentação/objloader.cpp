#pragma once

#include "objloader.h"

#include "stb_image.h"

int meshSize;

int getMeshSize() {
	return meshSize;
}

vector <string> tokenize(string buffer, string delim) {
	vector <string> tokens;
	string strbuffer(buffer);
	string token;
	int pos;

	do {
		pos = strbuffer.find(delim);
		if (pos > 0) {
			token = strbuffer.substr(0, pos);
			strbuffer = strbuffer.substr(pos + delim.size());
			tokens.push_back(token);
		}
		else tokens.push_back(strbuffer);
	} while (pos > 0);

	return tokens;
}

int setupMesh(string path)
{
	vector <glm::vec3> vertices, normals;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> vertIndices, normIndices, texIndices; //indices são ints, mas são 3

	ifstream inputFile;
	inputFile.open(path);

	//bool iniciouGrupo = true;

	if (inputFile.is_open())
	{
		char line[100];
		string sline;
		int cont = 1;
		//Objeto novobjeto;
		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			if (!inputFile.eof())
			{
				//cout << cont << " --> *" << line << "*" << endl;
				cont++;
				sline = line;
				string word;
				istringstream ssline(line);
				ssline >> word;
				if (word == "v" || word == "vn" || word == "vt" || word == "f")
				{
					glm::vec3 v3;
					if (word == "v")
					{


						//if (iniciouGrupo == false)
						//{
						//	iniciouGrupo = true;
						//	//....puxar do array de m ateriais, pelo nome
						//	
						//	//chama a funcao que cria os buffers - ou trecho de codigo]

						//	//novogrupo.VAO = VAO;
						//	
						//	novobjeto.grupos.push_back(novogrupo);
						//	

						//}

						ssline >> v3.x >> v3.y >> v3.z;
						vertices.push_back(v3);
					}
					if (word == "vn")
					{
						ssline >> v3.x >> v3.y >> v3.z;
						normals.push_back(v3);
					}
					if (word == "vt")
					{
						glm::vec2 v2;
						ssline >> v2.x >> v2.y;
						texCoords.push_back(v2);
					}
					if (word == "f")
					{
						string i1, i2, i3;
						glm::vec3 iv, ivn, ivt;
						ssline >> i1 >> i2 >> i3;
						vector <string> tokens;

						//Pegando os primeiros índices (considerando que tem todos)
						tokens = tokenize(i1, "/");
						iv[0] = atof(tokens[0].c_str());
						ivt[0] = atof(tokens[1].c_str());
						ivn[0] = atof(tokens[2].c_str());

						//Pegando os segundos índices (considerando que tem todos)
						tokens = tokenize(i2, "/");
						iv[1] = atof(tokens[0].c_str());
						ivt[1] = atof(tokens[1].c_str());
						ivn[1] = atof(tokens[2].c_str());

						//Pegando os terceiros índices (considerando que tem todos)
						tokens = tokenize(i3, "/");
						iv[2] = atof(tokens[0].c_str());
						ivt[2] = atof(tokens[1].c_str());
						ivn[2] = atof(tokens[2].c_str());

						vertIndices.push_back(iv);
						normIndices.push_back(ivn);
						texIndices.push_back(ivt);
					}

				}
				/*else
				{*/
				//if (word == "mtllib") //leirura do mtl
				//{

				//}

				//if (word == "usemtl")
				//{
				//	//criacao de um novo grupo no obj]
				//	Grupo novogrupo;
				//	iniciouGrupo = false;
				//}
			//}
			//ignorando o resto por enquanto
			}
			else
			{
				cout << "Foi até o fim!" << endl;
			}
		}//if (!inputFile.eof())
	
	}
	inputFile.close();


	//======================================================================
	vector <float> geometryBuffer;

	meshSize = vertIndices.size() * 3;

	for (int i = 0; i < vertIndices.size(); i++)
	{
		for (int j = 0; j < 3; j++) //triangulo
		{
			//recupera x, y, z (posicao)
			float x = vertices[(int)vertIndices[i][j] - 1].x;
			float y = vertices[(int)vertIndices[i][j] - 1].y;
			float z = vertices[(int)vertIndices[i][j] - 1].z;
			geometryBuffer.push_back(x);
			geometryBuffer.push_back(y);
			geometryBuffer.push_back(z);

			//recupera nx, ny, nz (normal)
			float nx = normals[(int)normIndices[i][j] - 1].x;
			float ny = normals[(int)normIndices[i][j] - 1].y;
			float nz = normals[(int)normIndices[i][j] - 1].z;
			geometryBuffer.push_back(nx);
			geometryBuffer.push_back(ny);
			geometryBuffer.push_back(nz);

			//recupera s, t (coord textura)
			float s = texCoords[(int)texIndices[i][j] - 1].x;
			float t = texCoords[(int)texIndices[i][j] - 1].y;
			geometryBuffer.push_back(s);
			geometryBuffer.push_back(t);
		}
	}
	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat), geometryBuffer.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);


	//return novobjeto;
	return VAO;
}

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

	


