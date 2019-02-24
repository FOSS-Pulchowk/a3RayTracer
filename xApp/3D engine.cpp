#include "olcConsoleGameEngine.h"
#include<string>
#include<fstream>
#include<strstream>
#include<algorithm>
using namespace std;


struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
	
};
vec3d operator-(const vec3d& v1, const vec3d& v2){

		vec3d v;
		v.x = v1.x-v2.x;
		v.y = v1.y-v2.y;
		v.z = v1.z-v2.z;
		return v;
	}
struct triangle
{
	vec3d p[3];
	wchar_t sym;
	short col;
};

struct mesh
{
	vector<triangle> tris;

	bool LoadFromFile(string filename)
	{
		ifstream f(filename);
			if (!f)
				return false;

			vector<vec3d>verts;
			while (!f.eof()) {
				char line[120];
				static int num = 0;
				num++;
				f.getline(line, 120);

				strstream s;
				s << line;
				char junk;

				if (line[0] == 'v') {
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);

				}
				if (line[0] == 'f') {
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });

				}
			}
			return true;
	}
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine
{
public:
	olcEngine3D()
	{
		m_sAppName = L"3D Demo";
	}


private:
	mesh meshCube;
	mat4x4 matProj;
	vec3d vCamera;

	float fTheta;

	vec3d Matrix_MultiplyVector(mat4x4 &m, vec3d &i)
	{
		vec3d v;
		v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
		v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
		v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
		v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
		return v;
	}

	vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2){
			vec3d v;
			v.x = v1.y * v2.z - v1.z * v2.y;
			v.y = v1.z * v2.x - v1.x * v2.z;
			v.z = v1.x * v2.y - v1.y * v2.x;
			return v;
	}

	vec3d Vector_Div(vec3d &v1, float k)
	{
		return { v1.x / k, v1.y / k, v1.z / k };
	}

	float Vector_DotProduct(vec3d &v1, vec3d &v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
	}

	float Vector_Length(vec3d &v)
	{
		return sqrtf(Vector_DotProduct(v, v));
	}

	vec3d Vector_Normalise(vec3d &v)
	{
		float l = Vector_Length(v);
		v.x /= l;v.y /= l;v.z /= l;
		return v;
	}
	mat4x4 Matrix_MakeIdentity()
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 Matrix_MakeRotationX(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[1][2] = sinf(fAngleRad);
		matrix.m[2][1] = -sinf(fAngleRad);
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 Matrix_MakeRotationY(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][2] = sinf(fAngleRad);
		matrix.m[2][0] = -sinf(fAngleRad);
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 Matrix_MakeRotationZ(float fAngleRad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][1] = sinf(fAngleRad);
		matrix.m[1][0] = -sinf(fAngleRad);
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 Matrix_MakeTranslation(float x, float y, float z)
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		matrix.m[3][0] = x;
		matrix.m[3][1] = y;
		matrix.m[3][2] = z;
		return matrix;
	}

	mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
	{
		float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
		mat4x4 matrix;
		matrix.m[0][0] = fAspectRatio * fFovRad;
		matrix.m[1][1] = fFovRad;
		matrix.m[2][2] = fFar / (fFar - fNear);
		matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matrix.m[2][3] = 1.0f;
		matrix.m[3][3] = 0.0f;
		return matrix;
	}
	mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
	{
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return matrix;
	}
	
	// Taken From Command Line Webcam Video
	CHAR_INFO GetColour(float lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f*lum);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}

public:
	bool OnUserCreate() override
	{
		meshCube.LoadFromFile("teapot.obj");
		
		// Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		// float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
		matProj = Matrix_MakeProjection(fFov, fAspectRatio, fNear, fFar);
		// matProj.m[0][0] = fAspectRatio * fFovRad;
		// matProj.m[1][1] = fFovRad;
		// matProj.m[2][2] = fFar / (fFar - fNear);
		// matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		// matProj.m[2][3] = 1.0f;
		// matProj.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		// Set up rotation matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;
		// Rotation Z
		matRotZ = Matrix_MakeRotationZ(0.5*fTheta);
		// Rotation X
		matRotX = Matrix_MakeRotationX(fTheta);
		mat4x4 matTrans = Matrix_MakeTranslation(0.0, 0.0,6.0);

		//now to add matrix into world matrix for both x and y rotation
		mat4x4 worldMat = Matrix_MakeIdentity();
		worldMat = Matrix_MultiplyMatrix(matRotZ, matRotX);
		worldMat = Matrix_MultiplyMatrix(worldMat, matTrans);


		vector<triangle>trianglesToRaster;
		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTransformed, triViewed;
			//All transformation rotation and translation i.e offset too
			triTransformed.p[0] = Matrix_MultiplyVector(worldMat, tri.p[0]);
			triTransformed.p[1] = Matrix_MultiplyVector(worldMat, tri.p[1]);
			triTransformed.p[2] = Matrix_MultiplyVector(worldMat, tri.p[2]);
			/*triTransformed.p[0].z += 8.0f;
			triTransformed.p[1].z += 8.0f;
			triTransformed.p[2].z += 8.0f;*/

			vec3d  normal, line1, line2;
			line1 = triTransformed.p[1] - triTransformed.p[0];
			line2 = triTransformed.p[2] - triTransformed.p[0];

			normal = Vector_CrossProduct(line1, line2);

			normal = Vector_Normalise(normal);
			

			// float dot = normal.x * (triTranslated.p[0].x - vCamera.x) +
			// 	normal.y * (triTranslated.p[0].y - vCamera.y) +
			// 	normal.z * (triTranslated.p[0].z - vCamera.z);
			vec3d test = triTransformed.p[0] - vCamera;

			if (Vector_DotProduct(normal, test) < 0.0f) {
				// Projection from 3D 2D
				vec3d light_direction = { 0.0, 0.0, -1.0f };
				//normalize vector  light_direction
				light_direction = Vector_Normalise(light_direction);
				// float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
				// light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				float dp;
				dp = max(0.1,Vector_DotProduct(normal, light_direction));
				CHAR_INFO c = GetColour(dp);
				triTransformed.col = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				triProjected.p[0] = Matrix_MultiplyVector(matProj, triTransformed.p[0]);
				triProjected.p[1] = Matrix_MultiplyVector(matProj, triTransformed.p[1]);
				triProjected.p[2] = Matrix_MultiplyVector(matProj, triTransformed.p[2]);

				// MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				// MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				// MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);
				triProjected.col = triTransformed.col;
				triProjected.sym = triTransformed.sym;

				triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

				// Scale into view
				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
				triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

				trianglesToRaster.push_back(triProjected);

			}
		}

		sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle& t1, triangle& t2) {
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;

		});


		for (auto &tri :trianglesToRaster) {
			// Rasterize triangle
			FillTriangle(tri.p[0].x, tri.p[0].y,
				tri.p[1].x, tri.p[1].y,
				tri.p[2].x, tri.p[2].y, tri.sym, tri.col);

			/*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_WHITE);*/


		}
		return true;
	}

};




int main()
{
	olcEngine3D myEngine;
	if (myEngine.ConstructConsole(300, 280, 2, 2))
		myEngine.Start();
	return 0;
}
