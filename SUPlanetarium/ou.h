static void Spheroid(double radius, float xflat, float yflat, float zflat,
					 float randomness, int slices, int stacks,
					 Planet planet)
{
	static GLfloat rho, drho, theta, dtheta;
	GLfloat x, y, z;
	GLfloat s, t, ds, dt;
	int i, j, k, imin, imax, tex;
	double *rarray1, *rarray2, *tmp;


	drho = PI / (GLfloat) stacks;
	dtheta = 2.0 * PI / (GLfloat) slices;

/* Allocate memory for surface randomness arrays */
	rarray1 = (double *) malloc((slices + 1) * 3 * sizeof(double));
	rarray2 = (double *) malloc((slices + 1) * 3 * sizeof(double));

	if (stacks % texHeight) {
		cout << "Incorrect STACKS value, edit configuration file\n";
	}

	if (slices % texWidth) {
		cout << "Incorrect SLICES value, edit configuration file\n";
	}

	ds = 1.0 / slices * texWidth;
	dt = 1.0 / stacks * texHeight;
	t = 1.0;
	imin = 0;
	imax = stacks;


	/* Initialize surface randomness arrays */
	for (i = 0; i < (slices + 1) * 3; i++) {
		rarray1[i] =
			(1.0 - randomness) +
			(randomness * 2.0 * rand()) / RAND_MAX;
		rarray2[i] =
			(1.0 - randomness) +
			(randomness * 2.0 * rand()) / RAND_MAX;
	}

	for (i = imin; i < imax; i++) {
		rho = i * drho;
		if (!(i % (imax / texHeight)))
			t = 1.0;
		tmp = rarray1;
		rarray1 = rarray2;
		rarray2 = tmp;
		for (k = 0; k < (slices + 1) * 3; k++)
			rarray2[k] =
				(1.0 - randomness) +
				(randomness * 2.0 * rand()) / RAND_MAX;

		for (k = 0; k < texWidth; k++) {
			/* Find the texture map we should apply in this iteration */
			tex = imax / texHeight;
			tex =
				(texWidth * (texHeight - 1) -
				 i / tex * texWidth) + k;
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUAD_STRIP);
			s = 0.0;
			/* overwrite last element with first element data */
			rarray1[(k + 1) * slices / texWidth * 3] =
				rarray1[k * slices / texWidth * 3];
			rarray1[(k + 1) * slices / texWidth * 3 + 1] =
				rarray1[k * slices / texWidth * 3 + 1];
			rarray1[(k + 1) * slices / texWidth * 3 + 2] =
				rarray1[k * slices / texWidth * 3 + 2];
			rarray2[(k + 1) * slices / texWidth * 3] =
				rarray2[k * slices / texWidth * 3];
			rarray2[(k + 1) * slices / texWidth * 3 + 1] =
				rarray2[k * slices / texWidth * 3 + 1];
			rarray2[(k + 1) * slices / texWidth * 3 + 2] =
				rarray2[k * slices / texWidth * 3 + 2];
			for (j = (k * slices / texWidth);
				 j <= ((k + 1) * slices / texWidth); j++) {
				theta = (j == slices) ? 0.0 : j * dtheta;
				x = -sin(theta) * sin(rho) * rarray1[j * 3];
				y = cos(theta) * sin(rho) * rarray1[j * 3 + 1];
				z = cos(rho) * rarray1[j * 3 + 2];
				glNormal3f(x, y, z);
				glTexCoord2f(s, t);
				glVertex3f(x * radius * (1.0 - xflat),
						   y * radius * (1.0 - yflat),
						   z * radius * (1.0 - zflat));
				x = -sin(theta) * sin(rho + drho) * rarray2[j * 3];
				y = cos(theta) * sin(rho + drho) * rarray2[j * 3 + 1];
				z = cos(rho + drho) * rarray2[j * 3 + 2];
				glNormal3f(x, y, z);
				glTexCoord2f(s, t - dt);
				s += ds;
				glVertex3f(x * radius * (1.0 - xflat),
						   y * radius * (1.0 - yflat),
						   z * radius * (1.0 - zflat));
			}
			glEnd();
		}
		t -= dt;
	}

	free(rarray1);
	free(rarray2);
}