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
#include <FaceTracker/FCheck.h>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
FCheck& FCheck::operator= (FCheck const& rhs)
{
  this->_b = rhs._b; this->_w = rhs._w.clone(); this->_paw = rhs._paw;
  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); return *this;
}
//===========================================================================
void FCheck::Init(double b, cv::Mat &w, PAW &paw)
{
  assert((w.type() == CV_64F) && (w.rows == paw._nPix));
  _b = b; _w = w.clone(); _paw = paw;
  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); return;
}
//===========================================================================
void FCheck::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//===========================================================================
void FCheck::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
void FCheck::Write(ofstream &s)
{
  s << IO::FCHECK << " " << _b << " ";
  IO::WriteMat(s,_w); _paw.Write(s); return;
}
//===========================================================================
void FCheck::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::FCHECK);}
  s >> _b; IO::ReadMat(s,_w); _paw.Read(s); 
  crop_.create(_paw._mask.rows,_paw._mask.cols,CV_8U);
  vec_.create(_paw._nPix,1,CV_64F); return;
}
//===========================================================================
bool FCheck::Check(cv::Mat &im,cv::Mat &s)
{
  assert((im.type() == CV_8U) && (s.type() == CV_64F) &&
	 (s.rows/2 == _paw.nPoints()) && (s.cols == 1));
  _paw.Crop(im,crop_,s);
  if((vec_.rows!=_paw._nPix)||(vec_.cols!=1))vec_.create(_paw._nPix,1,CV_64F);
  int i,j,w = crop_.cols,h = crop_.rows;
  cv::MatIterator_<double> vp = vec_.begin<double>();
  cv::MatIterator_<uchar>  cp = crop_.begin<uchar>();
  cv::MatIterator_<uchar>  mp = _paw._mask.begin<uchar>();
  for(i=0;i<h;i++){for(j=0;j<w;j++,++mp,++cp){if(*mp)*vp++ = (double)*cp;}}
  double var,mean=cv::sum(vec_)[0]/vec_.rows; vec_-=mean; var = vec_.dot(vec_); 
  if(var < 1.0e-10)vec_ = cvScalar(0); else vec_ /= sqrt(var);
  if((_w.dot(vec_)+ _b) > 0)return true; else return false;
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
//===========================================================================
void MFCheck::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//===========================================================================
void MFCheck::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
void MFCheck::Write(ofstream &s)
{
  s << IO::MFCHECK << " " << _fcheck.size() << " ";
  for(int i = 0; i < (int)_fcheck.size(); i++)_fcheck[i].Write(s); return;
}
//===========================================================================
void MFCheck::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::MFCHECK);}
  int n; s >> n; _fcheck.resize(n);
  for(int i = 0; i < n; i++)_fcheck[i].Read(s); return;
}
//===========================================================================
bool MFCheck::Check(int idx,cv::Mat &im,cv::Mat &s)
{
  assert((idx >= 0) && (idx < (int)_fcheck.size()));
  return _fcheck[idx].Check(im,s);
}
//===========================================================================
