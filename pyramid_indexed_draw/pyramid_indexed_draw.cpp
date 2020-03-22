
//

const int num_segments = 40;

	float circle_vertices[num_segments * 3];
	float circle_colors[num_segments * 4];

	int step = (int) 360 / num_segments;

	int incr_deg = 0;
	for(int i = 0; i < num_segments; i++)
	{
		if(incr_deg > 360)
			break;

		circle_vertices[i] = cosf(incr_deg * 3.14159 / 180.0);
		circle_vertices[i+1] = sinf(incr_deg * 3.14159 / 180.0);
		circle_vertices[i+2] = 0.0;

		cout.precision(2);
		cout << fixed;
		cout << circle_vertices[i] << " "
			 << circle_vertices[i+1] << " "
			 << circle_vertices[i+2] << endl;

		incr_deg += step;
		cout << i << endl;
	}


	for(i = 0; i < num_segments; i += 4)
	{
		circle_colors[i]   = 0.0f;
		circle_colors[i+1] = 1.0f;
		circle_colors[i+2] = 0.0f;
		circle_colors[i+3] = 0.0f;
	}


	GLuint circle_VBO_array[2];

	glGenVertexArrays(1, &circle_VAO);
	glBindVertexArray(circle_VAO);

		glGenBuffers(2, circle_VBO_array);
		glBindBuffer(GL_ARRAY_BUFFER, circle_VBO_array[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices), circle_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, circle_VBO_array[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle_colors), circle_colors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);