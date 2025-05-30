#include "CatmullRom.h"

// Constructor
CCatmullRom::CCatmullRom() {
    angle = 2.0;
}

// Destructor
CCatmullRom::~CCatmullRom() {
    
}

glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    glm::vec3 a = p1;
    glm::vec3 b = 0.5f * (-p0 + p2);
    glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
    glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);
    glm::vec3 x = a + b * t + c * t2 + d * t3;
    return x;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(-350, 0.5, -350));
	m_controlPoints.push_back(glm::vec3(150, 0.5, -250));
	m_controlPoints.push_back(glm::vec3(350, 0.5, -10));
	m_controlPoints.push_back(glm::vec3(100, 0.5, 50));
	m_controlPoints.push_back(glm::vec3(50, 0.5, 300));
	m_controlPoints.push_back(glm::vec3(-350, 0.5, 250));
	m_controlPoints.push_back(glm::vec3(-150, 0.5, -200));
	m_controlPoints.push_back(glm::vec3(-400, 0.5, -200));

	/*	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));*/


	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();
	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	 // Generate VAO
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	// Create VBO
	CVertexBufferObject vboo;
	vboo.Create();
	vboo.Bind();

	// Iterate through centreline points and add them to the VBO
	for (size_t i = 0; i < m_centrelinePoints.size(); ++i) {
		glm::vec3& point = m_centrelinePoints[i];

		// Texture coordinate (set to (0, 0))
		glm::vec2 texCoord(0.0f, 0.0f);

		// Normal (set to (0, 1, 0))
		glm::vec3 normal(0.0f, 1.0f, 0.0f);

		// Add position, texture coordinate, and normal to VBO
		vboo.AddData(&point, sizeof(glm::vec3));
		vboo.AddData(&texCoord, sizeof(glm::vec2));
		vboo.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload VBO data to GPU
	vboo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set vertex attribute pointers
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	// Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Unbind VBO and VAO
	
	glBindVertexArray(0);

}


void CCatmullRom::CreateOffsetCurves()
{

	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 y(0, 1, 0);
		glm::vec3 N = glm::normalize(glm::cross(T, y));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		float spacing = 15.0f;

	
		glm::vec3 leftPoint = p - (spacing) * N;
		glm::vec3 rightPoint = p + (spacing) * N;

		// Texture coordinate (set to (0, 0))
		glm::vec2 texCoord(0.0f, 0.0f);

		// Normal (set to (0, 1, 0))
		glm::vec3 normal(0.0f, 1.0f, 0.0f);


		vbo.AddData(&leftPoint, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
		m_leftOffsetPoints.push_back(leftPoint);
	}

	// Upload VBO data to GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set vertex attribute pointers
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	// Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Unbind VBO and VAO
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();
	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 y(0, 1, 0);
		glm::vec3 N = glm::normalize(glm::cross(T, y));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		float spacing = 15.0f;


		glm::vec3 leftPoint = p - (spacing)*N;
		glm::vec3 rightPoint = p + (spacing)*N;

		// Texture coordinate (set to (0, 0))
		glm::vec2 texCoord(0.0f, 0.0f);

		// Normal (set to (0, 1, 0))
		glm::vec3 normal(0.0f, 1.0f, 0.0f);


		vbo2.AddData(&rightPoint, sizeof(glm::vec3));
		vbo2.AddData(&texCoord, sizeof(glm::vec2));
		vbo2.AddData(&normal, sizeof(glm::vec3));
		m_rightOffsetPoints.push_back(rightPoint);
	}
	vbo2.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	// Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Unbind VBO and VAO
	glBindVertexArray(0);

// Generate VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card

// Note it is possible to only use one VAO / VBO with all the points instead.

}


void CCatmullRom::CreateTrack()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	//glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	glPointSize(10.0f); // Set point size
	//glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());

	// Render the centreline as GL_LINE_LOOP
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());

	// Unbind the VAO
	glBindVertexArray(0);

}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	//glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glPointSize(5.0f); // Set point size
	glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());

	// Render the centreline as GL_LINE_LOOP
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());

	// Unbind the VAO
	glBindVertexArray(0);

	glBindVertexArray(m_vaoRightOffsetCurve);
	glPointSize(5.0f); // Set point size
	glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());

	// Render the centreline as GL_LINE_LOOP
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());

	// Unbind the VAO
	glBindVertexArray(0);

	// Bind the VAO m_vaoRightOffsetCurve and render it
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);










void CCatmullRom::CreatePath(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::vector<glm::vec3> pathVertices;

    // Use VAO to store state associated with vertices
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    // Create a VBO
    CVertexBufferObject vbo;
    vbo.Create();
    vbo.Bind();
	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(2, 0);
	glm::vec2 t2 = glm::vec2(2, 2);
	glm::vec2 t3 = glm::vec2(0, 2);
    glm::vec3 normal(0.0f, 3.0f, 0.0f);
    for (unsigned int i = 0; i < m_rightOffsetPoints.size()-3; i=i+3) {
       
        vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
        vbo.AddData(&t0, sizeof(glm::vec2));
        vbo.AddData(&normal, sizeof(glm::vec3));

		vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbo.AddData(&t1, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		pathVertices.push_back(m_leftOffsetPoints[i]);
		pathVertices.push_back(m_rightOffsetPoints[i]);

		vbo.AddData(&m_leftOffsetPoints[i+3], sizeof(glm::vec3));
		vbo.AddData(&t2, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		vbo.AddData(&m_rightOffsetPoints[i+3], sizeof(glm::vec3));
		vbo.AddData(&t3, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		pathVertices.push_back(m_leftOffsetPoints[i+3]);
		pathVertices.push_back(m_rightOffsetPoints[i+3]);


		//maby make new vector to store left and right points

    }

	vbo.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
	vbo.AddData(&t0, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));

	vbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
	vbo.AddData(&t1, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));


	m_vertexCount = pathVertices.size();

    // Upload the VBO to the GPU
    vbo.UploadDataToGPU(GL_STATIC_DRAW);
    // Set the vertex attribute locations
    GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
        + sizeof(glm::vec2)));

	glBindVertexArray(0);
}



void CCatmullRom::RenderPath() {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Bind the VAO
    glBindVertexArray(m_vao);
	m_texture.Bind();

    // Set line width if supported
    glLineWidth(2.0f); // Adjust as needed
    // Draw the path using GL_LINE_STRIP primitive with 100 vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//m_texture.Release();
    // Unbind the VAO
    glBindVertexArray(0);

}
//add release destructuor like in cube.cpp

glm::vec3 CCatmullRom::RandomPos() {
	
	int random = rand() % m_leftOffsetPoints.size();
	float t = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	glm::vec3 randomPoint = m_leftOffsetPoints[random] + t * (m_rightOffsetPoints[random] - m_leftOffsetPoints[random]);
	return randomPoint;
}