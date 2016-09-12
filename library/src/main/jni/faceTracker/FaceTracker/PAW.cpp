///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010, Jason Mora Saragih, all rights reserved.
//
// This file is part of FaceTracker.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       J. M. Saragih, S. Lucey, and J. F. Cohn. Face Alignment through 
//       Subspace Constrained Mean-Shifts. International Conference of Computer 
//       Vision (ICCV), September, 2009.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include <FaceTracker/PAW.h>
#define it at<int>
#define db at<double>
using namespace FACETRACKER;
using namespace std;
//=============================================================================
bool sameSide(double x0, double y0, double x1, double y1,
	      double x2, double y2, double x3, double y3)
{
  double x = (x3-x2)*(y0-y2) - (x0-x2)*(y3-y2);
  double y = (x3-x2)*(y1-y2) - (x1-x2)*(y3-y2);
  if(x*y >= 0)return true; else return false;
}
//=============================================================================
int isWithinTri(double x,double y,cv::Mat &tri,cv::Mat &shape)
{
  int i,j,k,t,n = tri.rows,p = shape.rows/2; double s11,s12,s21,s22,s31,s32;
  for(t = 0; t < n; t++){
    i = tri.it(t,0); j = tri.it(t,1); k = tri.it(t,2);
    s11 = shape.db(i  ,0); s21 = shape.db(j  ,0); s31 = shape.db(k  ,0);
    s12 = shape.db(i+p,0); s22 = shape.db(j+p,0); s32 = shape.db(k+p,0);
    if(sameSide(x,y,s11,s12,s21,s22,s31,s32) &&
       sameSide(x,y,s21,s22,s11,s12,s31,s32) &&
       sameSide(x,y,s31,s32,s11,s12,s21,s22))return t;
  }return -1;
}
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
PAW& PAW::operator= (PAW const& rhs)
{   
  this->_nPix = rhs._nPix;
  this->_xmin = rhs._xmin;
  this->_ymin = rhs._ymin;
  this->_src  = rhs._src.clone();
  this->_tri  = rhs._tri.clone();
  this->_tridx  = rhs._tridx.clone();
  this->_mask  = rhs._mask.clone();
  this->_alpha  = rhs._alpha.clone();
  this->_beta  = rhs._beta.clone();
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F);
  _dst = _src; return *this;
}
//===========================================================================
void PAW::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//===========================================================================
void PAW::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
void PAW::Write(ofstream &s)
{
  s << IO::PAW << " " << _nPix << " " << _xmin << " " << _ymin << " ";
  IO::WriteMat(s,_src); IO::WriteMat(s,_tri); IO::WriteMat(s,_tridx);
  IO::WriteMat(s,_mask); IO::WriteMat(s,_alpha); IO::WriteMat(s,_beta);
  return;
}
//===========================================================================
void PAW::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::PAW);}
  s >> _nPix >> _xmin >> _ymin;
  IO::ReadMat(s,_src); IO::ReadMat(s,_tri); IO::ReadMat(s,_tridx);
  IO::ReadMat(s,_mask); IO::ReadMat(s,_alpha); IO::ReadMat(s,_beta);
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F); _dst = _src; return;
}
//===========================================================================
void PAW::Init(cv::Mat &src,cv::Mat &tri)
{
  assert((src.type() == CV_64F) && (src.cols == 1));
  assert((tri.type() == CV_32S) && (tri.cols == 3));
  _src = src.clone(); _tri = tri.clone();
  int i,j,k,l,n = this->nPoints(); double c1,c2,c3,c4,c5; 
  _alpha.create(this->nTri(),3,CV_64F); _beta.create(this->nTri(),3,CV_64F);
  for(i = 0; i < this->nTri(); i++){
    j = _tri.it(i,0); k = _tri.it(i,1); l = _tri.it(i,2);
    c1 = _src.db(l+n,0) - _src.db(j+n,0); c2 = _src.db(l,0) - _src.db(j,0);
    c4 = _src.db(k+n,0) - _src.db(j+n,0); c3 = _src.db(k,0) - _src.db(j,0); 
    c5 = c3*c1 - c2*c4;
    _alpha.db(i,0) = (_src.db(j+n,0)*c2 - _src.db(j,0)*c1)/c5;
    _alpha.db(i,1) =  c1/c5; _alpha.db(i,2) = -c2/c5; 
    _beta.db(i,0)  = (_src.db(j,0)*c4 - _src.db(j+n,0)*c3)/c5;
    _beta.db(i,1)  = -c4/c5; _beta.db(i,2)  =  c3/c5;
  }
  cv::MatIterator_<double> x = _src.begin<double>(),y =_src.begin<double>()+n;
  double vx,vy,xmax=*x,ymax=*y,xmin=*x,ymin=*y;
  for(i = 0; i < n; i++){
    vx = *x++; vy = *y++;
    xmax = std::max(xmax,vx); ymax = std::max(ymax,vy);
    xmin = std::min(xmin,vx); ymin = std::min(ymin,vy);
  }
  int w = int(xmax - xmin + 1.0),h = int(ymax - ymin + 1.0);
  _mask.create(h,w,CV_8U); _tridx = cv::Mat::zeros(h,w,CV_32S);
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  for(i = 0,_nPix = 0; i < h; i++){
    for(j = 0; j < w; j++,++mp,++tp){      
      isWithinTri(double(j)+xmin,double(i)+ymin,tri,_src);
      if((*tp = isWithinTri(double(j)+xmin,double(i)+ymin,tri,_src))==-1)*mp=0;
      else{*mp = 1; _nPix++;}
    }
  }
  _mapx.create(_mask.rows,_mask.cols,CV_32F);
  _mapy.create(_mask.rows,_mask.cols,CV_32F);
  _coeff.create(this->nTri(),6,CV_64F);
  _dst = _src; _xmin = xmin; _ymin = ymin;  return;
}
//=============================================================================
void PAW::Crop(cv::Mat &src, cv::Mat &dst, cv::Mat &s)
{
  assert((s.type() == CV_64F) && (s.rows == _src.rows) && (s.cols == 1) && 
	 (src.type() == dst.type()));
  _dst = s; this->CalcCoeff(); this->WarpRegion(_mapx,_mapy);
  cv::remap(src,dst,_mapx,_mapy,CV_INTER_LINEAR); return;
}
//=============================================================================
void PAW::CalcCoeff()
{
  int i,j,k,l,p=this->nPoints(); double c1,c2,c3,c4,c5,c6,*coeff,*alpha,*beta;
  for(l = 0; l < this->nTri(); l++){
    i = _tri.it(l,0); j = _tri.it(l,1); k = _tri.it(l,2);
    c1 = _dst.db(i  ,0); c2 = _dst.db(j  ,0) - c1; c3 = _dst.db(k  ,0) - c1;
    c4 = _dst.db(i+p,0); c5 = _dst.db(j+p,0) - c4; c6 = _dst.db(k+p,0) - c4;
    coeff = _coeff.ptr<double>(l);
    alpha = _alpha.ptr<double>(l);
    beta  = _beta.ptr<double>(l);
    coeff[0] = c1 + c2*alpha[0] + c3*beta[0];
    coeff[1] =      c2*alpha[1] + c3*beta[1];
    coeff[2] =      c2*alpha[2] + c3*beta[2];
    coeff[3] = c4 + c5*alpha[0] + c6*beta[0];
    coeff[4] =      c5*alpha[1] + c6*beta[1];
    coeff[5] =      c5*alpha[2] + c6*beta[2];
  }return;
}
//=============================================================================
void PAW::WarpRegion(cv::Mat &mapx,cv::Mat &mapy)
{
  assert((mapx.type() == CV_32F) && (mapy.type() == CV_32F));
  if((mapx.rows != _mask.rows) || (mapx.cols != _mask.cols))
    _mapx.create(_mask.rows,_mask.cols,CV_32F);
  if((mapy.rows != _mask.rows) || (mapy.cols != _mask.cols))
    _mapy.create(_mask.rows,_mask.cols,CV_32F);
  int x,y,j,k=-1; double yi,xi,xo,yo,*a=NULL,*ap;
  cv::MatIterator_<float> xp = mapx.begin<float>();
  cv::MatIterator_<float> yp = mapy.begin<float>();
  cv::MatIterator_<uchar> mp = _mask.begin<uchar>();
  cv::MatIterator_<int>   tp = _tridx.begin<int>();
  for(y = 0; y < _mask.rows; y++){   yi = double(y) + _ymin;
    for(x = 0; x < _mask.cols; x++){ xi = double(x) + _xmin;
      if(*mp == 0){*xp = -1; *yp = -1;}
      else{
	j = *tp; if(j != k){a = _coeff.ptr<double>(j); k = j;}  	
	ap = a;
	xo = *ap++; xo += *ap++ * xi; *xp = float(xo + *ap++ * yi);
	yo = *ap++; yo += *ap++ * xi; *yp = float(yo + *ap++ * yi);
      }
      mp++; tp++; xp++; yp++;
    }
  }return;
}
//===========================================================================
