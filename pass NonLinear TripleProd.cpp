#include <iostream>
#include "Wavelet.h"
#include "atlimage.h"
using namespace std;

#define MyRes 32 
#define N 1024
#define keepN 1024

//get Coef from Xmat, with s=(level,iIdx,jIdx), M=01/10/11
const float& GetCoef(float* Xmat, int level, int iIdx, int jIdx, int MIdx, int res)
{
	switch(MIdx)//res * row + col
	{
	case 1://M01. col: 2^l +i, row: j
		return *(Xmat + res * jIdx + (1<<level) + iIdx);
		break;
	case 2://M10. col: i, row: 2^l+j
		return *(Xmat + res * ((1<<level) + jIdx) + iIdx);
	case 3://M11. col: 2^l+i, row: 2^l+j
		return *(Xmat + res * ((1<<level) + jIdx) + (1<<level) + iIdx);
	default:
		assert(0);
		break;
	}
	return Xmat[0];
}

const float GetCoefNL(float* vBig, short int* vBigIdx, int level, int iIdx, int jIdx, int MIdx, int res)
{
	int idx;
	switch(MIdx)//res * row + col
	{
	case 1://M01. col: 2^l +i, row: j
		idx = res * jIdx + (1<<level) + iIdx;
		break;
	case 2://M10. col: i, row: 2^l+j
		idx = res * ((1<<level) + jIdx) + iIdx;
		break;
	case 3://M11. col: 2^l+i, row: 2^l+j
		idx = res * ((1<<level) + jIdx) + (1<<level) + iIdx;
		break;
	default:
		assert(0);
		break;
	}

	for(int i = 0; i < keepN; ++i)
	{
		if(vBigIdx[i] == idx)
			return vBig[i];
	}
	return 0;//this is important!
}

int SignOfQuadrant(int MIdx, int qx, int qy)
{
	if(MIdx == 1)
	{
		if(qx == 0)	return 1;
		else return -1;
	}
	if(MIdx == 2)
	{
		if(qy == 0) return 1;//might wrong
		else return -1;
	}
	if(MIdx == 3)
	{
		if(qx == 0)
		{
			if(qy == 0) return 1;
			else return -1;
		}
		else
		{
			if(qy == 0) return -1;
			else return 1;
		}
	}
	assert(0);
	return 0;
}

const float GetCoefPSum(float* Xmat, int level, int iIdx, int jIdx, int res)
{
	if((level == 0) && (iIdx == 0) && (jIdx == 0))
		return Xmat[0];
	int ol = level - 1;
	int oi = iIdx / 2;
	int oj = jIdx / 2;
	int qx = iIdx - 2 * oi; 
	int qy = jIdx - 2 * oj;
	return GetCoefPSum(Xmat, ol, oi, oj, res) + (1 << ol) *
		(GetCoef(Xmat, ol, oi, oj, 1, res) * SignOfQuadrant(1, qx, qy) +
		 GetCoef(Xmat, ol, oi, oj, 2, res) * SignOfQuadrant(2, qx, qy) +
		 GetCoef(Xmat, ol, oi, oj, 3, res) * SignOfQuadrant(3, qx, qy));
}

const float GetCoefPSumNL(float* vBig, short int* vBigIdx, int level, int iIdx, int jIdx, int res)
{
	if((level == 0) && (iIdx == 0) && (jIdx == 0))
		return vBig[0];
	int ol = level - 1;
	int oi = iIdx / 2;
	int oj = jIdx / 2;
	int qx = iIdx - 2 * oi; 
	int qy = jIdx - 2 * oj;
	return GetCoefPSumNL(vBig, vBigIdx, ol, oi, oj, res) + (1 << ol) *
		(GetCoefNL(vBig, vBigIdx, ol, oi, oj, 1, res) * SignOfQuadrant(1, qx, qy) +
		 GetCoefNL(vBig, vBigIdx, ol, oi, oj, 2, res) * SignOfQuadrant(2, qx, qy) +
		 GetCoefNL(vBig, vBigIdx, ol, oi, oj, 3, res) * SignOfQuadrant(3, qx, qy));
}


void main()
{
	CWavelet m_wavelet;
	const res = MyRes;

	float Lmat[N];
	for(int i = 0; i < N; ++i)
		Lmat[i] = float(rand()%100);//(float)i;
	float Pmat[N];
	for(int i = 0; i < N; ++i)
		Pmat[i] = float(rand()%201);//Lmat[i] * 10;
	float Tmat[N];
	for(int i = 0; i < N; ++i)
		Tmat[i] = float(rand()%301);//Lmat[i] * 100;

	float sum1=0, sum2=0, sum3=0, sum4=0;
	for(int i = 0; i < N; ++i)
	{
		sum1 += Lmat[i] * Pmat[i];
		sum2 += Lmat[i] * Tmat[i];
		sum3 += Pmat[i] * Tmat[i];
		sum4 += Lmat[i] * Pmat[i] * Tmat[i];
	}

	//m_wavelet.Haar2D(Lmat, res,res);
	//m_wavelet.Haar2D(Pmat, res,res);
	//m_wavelet.Haar2D(Tmat, res,res);
	m_wavelet.NonstandDecomposition(Lmat, res, res);
	m_wavelet.NonstandDecomposition(Pmat, res, res);
	m_wavelet.NonstandDecomposition(Tmat, res, res);

	vector<float> vBigL(keepN, 0.f);
	vector<float> vBigP(keepN, 0.f);
	vector<float> vBigT(keepN, 0.f);
	vector<short int> vBigLIdx(keepN, 0);
	vector<short int> vBigPIdx(keepN, 0);
	vector<short int> vBigTIdx(keepN, 0);
	KeepLoss(Lmat, N, keepN, &vBigL[0], &vBigLIdx[0]);
	KeepLoss(Pmat, N, keepN, &vBigP[0], &vBigPIdx[0]);
	KeepLoss(Tmat, N, keepN, &vBigT[0], &vBigTIdx[0]);

	float sum1c=0, sum2c=0, sum3c=0, sum4c=0;
	for(int i = 0; i < N; ++i)
	{
		sum1c += Lmat[i] * Pmat[i];//using Nonstand, 0.00001 precision holds for double product:)
		sum2c += Lmat[i] * Tmat[i];
		sum3c += Pmat[i] * Tmat[i];
		sum4c += Lmat[i] * Pmat[i] * Tmat[i];
	}

	
	float integral = vBigL[0] * vBigP[0] * vBigT[0];
	float Cuvw = 0;
	int tmp = 1, levelNum = 0;
	while(tmp != N)
	{
		tmp *= 4;
		++levelNum;
	}
	float psumL, psumP, psumT;
	for(int level = 0; level < levelNum; ++level)
	{
		for(int iIdx = 0; iIdx < (1<<level); ++iIdx)
			for(int jIdx = 0; jIdx < (1<<level); ++jIdx)
			{
				Cuvw = float(1 << level);
				float P1 = GetCoefNL(&vBigP[0], &vBigPIdx[0], level, iIdx, jIdx, 1, res);
				float P2 = GetCoefNL(&vBigP[0], &vBigPIdx[0], level, iIdx, jIdx, 2, res);
				float P3 = GetCoefNL(&vBigP[0], &vBigPIdx[0], level, iIdx, jIdx, 3, res);
				float L1 = GetCoefNL(&vBigL[0], &vBigLIdx[0], level, iIdx, jIdx, 1, res);
				float L2 = GetCoefNL(&vBigL[0], &vBigLIdx[0], level, iIdx, jIdx, 2, res);
				float L3 = GetCoefNL(&vBigL[0], &vBigLIdx[0], level, iIdx, jIdx, 3, res);
				float T1 = GetCoefNL(&vBigT[0], &vBigTIdx[0], level, iIdx, jIdx, 1, res);
				float T2 = GetCoefNL(&vBigT[0], &vBigTIdx[0], level, iIdx, jIdx, 2, res);
				float T3 = GetCoefNL(&vBigT[0], &vBigTIdx[0], level, iIdx, jIdx, 3, res);
				//case 2
				integral += Cuvw * (
					P1 * L2 * T3 +
					P1 * L3 * T2 +
					P2 * L3 * T1 +
					P2 * L1 * T3 +
					P3 * L1 * T2 +
					P3 * L2 * T1);
				//case 3
				psumL = GetCoefPSumNL(&vBigL[0], &vBigLIdx[0], level, iIdx, jIdx, res);
				psumP = GetCoefPSumNL(&vBigP[0], &vBigPIdx[0], level, iIdx, jIdx, res);
				psumT = GetCoefPSumNL(&vBigT[0], &vBigTIdx[0], level, iIdx, jIdx, res);
				integral +=
					(P1 * L1 * psumT +
					 L1 * T1 * psumP +
					 T1 * P1 * psumL);
				integral +=
					(P2 * L2 * psumT +
					L2 * T2 * psumP +
					T2 * P2 * psumL);
				integral +=
					(P3 * L3 * psumT +
					L3 * T3 * psumP +
					T3 * P3 * psumL);
			}
	}
	integral /= res;
	cout<<"hello"<<endl;	
}
/*

float integral = Lmat[0] * Pmat[0] * Tmat[0];
float Cuvw = 0;
int tmp = 1, levelNum = 0;
while(tmp != N)
{
tmp *= 4;
++levelNum;
}
float psumL, psumP, psumT;
for(int level = 0; level < levelNum; ++level)
{
for(int iIdx = 0; iIdx < (1<<level); ++iIdx)
for(int jIdx = 0; jIdx < (1<<level); ++jIdx)
{
Cuvw = float(1 << level);
float P1 = GetCoef(Pmat, level, iIdx, jIdx, 1, res);
float P2 = GetCoef(Pmat, level, iIdx, jIdx, 2, res);
float P3 = GetCoef(Pmat, level, iIdx, jIdx, 3, res);
float L1 = GetCoef(Lmat, level, iIdx, jIdx, 1, res);
float L2 = GetCoef(Lmat, level, iIdx, jIdx, 2, res);
float L3 = GetCoef(Lmat, level, iIdx, jIdx, 3, res);
float T1 = GetCoef(Tmat, level, iIdx, jIdx, 1, res);
float T2 = GetCoef(Tmat, level, iIdx, jIdx, 2, res);
float T3 = GetCoef(Tmat, level, iIdx, jIdx, 3, res);
//case 2
integral += Cuvw * (
P1 * L2 * T3 +
P1 * L3 * T2 +
P2 * L3 * T1 +
P2 * L1 * T3 +
P3 * L1 * T2 +
P3 * L2 * T1);
//case 3
psumL = GetCoefPSum(Lmat, level, iIdx, jIdx, res);
psumP = GetCoefPSum(Pmat, level, iIdx, jIdx, res);
psumT = GetCoefPSum(Tmat, level, iIdx, jIdx, res);
integral +=
(P1 * L1 * psumT +
L1 * T1 * psumP +
T1 * P1 * psumL);
integral +=
(P2 * L2 * psumT +
L2 * T2 * psumP +
T2 * P2 * psumL);
integral +=
(P3 * L3 * psumT +
L3 * T3 * psumP +
T3 * P3 * psumL);
}
}
integral /= res;
*/