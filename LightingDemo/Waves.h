//-------------------------------------------------------------------------------
//
// Gumshoe Framework v1.00
//   - Based on MJP's DX11 Sample Framework (http://mynameismjp.wordpress.com/)
//
// ------------------------- Waves Class -------------------------
//  Performs the calculations for the wave simulation.  After the simulation has been
//  updated, the client must copy the current solution into vertex buffers for rendering.
//  This class only does the calculations, it does not do any drawing.
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#pragma once

#include <PCH.h>

class Waves
{
public:
	Waves();
	~Waves();

	UINT RowCount()const;
	UINT ColumnCount()const;
	UINT VertexCount()const;
	UINT TriangleCount()const;
	float Width()const;
	float Depth()const;

	// Returns the solution at the ith grid point.
	const XMFLOAT3& operator[](int i)const { return mCurrSolution[i]; }

	// Returns the solution normal at the ith grid point.
	const XMFLOAT3& Normal(int i)const { return mNormals[i]; }

	// Returns the unit tangent vector at the ith grid point in the local x-axis direction.
	const XMFLOAT3& TangentX(int i)const { return mTangentX[i]; }

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);

private:
	UINT mNumRows;
	UINT mNumCols;

	UINT mVertexCount;
	UINT mTriangleCount;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	XMFLOAT3* mPrevSolution;
	XMFLOAT3* mCurrSolution;
	XMFLOAT3* mNormals;
	XMFLOAT3* mTangentX;
};