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
#include <FaceTracker/Tracker.h>
#define db at<double>
#define TSCALE 0.3
using namespace FACETRACKER;
using namespace std;
//===========================================================================
void Tracker::Init(CLM &clm,FDet &fdet,MFCheck &fcheck,
		     cv::Mat &rshape,cv::Scalar &simil)
{
  _clm = clm; _fdet = fdet; _fcheck = fcheck; 
  _rshape = rshape.clone(); _simil = simil;
  _shape.create(2*_clm._pdm.nPoints(),1,CV_64F);
  _rect.x = 0; _rect.y = 0; _rect.width = 0; _rect.height = 0; 
  _frame = -1; _clm._pdm.Identity(_clm._plocal,_clm._pglobl);
  return;
}
//===========================================================================
void Tracker::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//Add by wysaid. ==========================================================
void Tracker::LoadFromData(const char* data)
{
    assert(data != nullptr);
    istringstream stream(data);
    this->Read(stream);
    stream.clear();
}
//===========================================================================
void Tracker::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
void Tracker::Write(ofstream &s)
{
  s << IO::TRACKER << " "; _clm.Write(s); _fdet.Write(s); _fcheck.Write(s); 
  IO::WriteMat(s,_rshape); 
  s << _simil[0] << " " << _simil[1] << " " 
    << _simil[2] << " " << _simil[3] << " "; return;
}
//===========================================================================
void Tracker::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::TRACKER);}
  _clm.Read(s); _fdet.Read(s); _fcheck.Read(s); IO::ReadMat(s,_rshape); 
  s >> _simil[0] >> _simil[1] >> _simil[2] >> _simil[3]; 
  _shape.create(2*_clm._pdm.nPoints(),1,CV_64F);
  _rect.x = 0; _rect.y = 0; _rect.width = 0; _rect.height = 0;
  _frame = -1; _clm._pdm.Identity(_clm._plocal,_clm._pglobl); return;
}
//===========================================================================
int Tracker::Track(cv::Mat im,vector<int> &wSize, const int  fpd,
		   const int  nIter, const double clamp,const double fTol,
		   const bool fcheck)
{ 
  assert(im.type() == CV_8U);
  if(im.channels() == 1)gray_ = im;
  else{
    if((gray_.rows != im.rows) || (gray_.cols != im.cols))
      gray_.create(im.rows,im.cols,CV_8U);
    cv::cvtColor(im,gray_,CV_BGR2GRAY);
  }
  bool gen,rsize=true; cv::Rect R;
  if((_frame < 0) || (fpd >= 0 && fpd < _frame)){
    _frame = 0; R = _fdet.Detect(gray_); gen = true;
  }else{R = this->ReDetect(gray_); gen = false;}
  if((R.width == 0) || (R.height == 0)){_frame = -1; return -1;}
  _frame++;
  if(gen){
    this->InitShape(R,_shape);
    _clm._pdm.CalcParams(_shape,_clm._plocal,_clm._pglobl);
  }else{
    double tx = R.x - _rect.x,ty = R.y - _rect.y;
    _clm._pglobl.db(4,0) += tx; _clm._pglobl.db(5,0) += ty; rsize = false;
  }
  _clm.Fit(gray_,wSize,nIter,clamp,fTol);
  _clm._pdm.CalcShape2D(_shape,_clm._plocal,_clm._pglobl);
  if(fcheck){if(!_fcheck.Check(_clm.GetViewIdx(),gray_,_shape))return -1;}
  _rect = this->UpdateTemplate(gray_,_shape,rsize); 
  if((_rect.width == 0) || (_rect.height == 0))return -1; else return 0;
}
//===========================================================================
void Tracker::InitShape(cv::Rect &r,cv::Mat &shape)
{
  assert((shape.rows == _rshape.rows) && (shape.cols == _rshape.cols) &&
	 (shape.type() == CV_64F));
  int i,n = _rshape.rows/2; double a,b,tx,ty;
  a = r.width*cos(_simil[1])*_simil[0] + 1;
  b = r.width*sin(_simil[1])*_simil[0];
  tx = r.x + r.width/2  + r.width *_simil[2];
  ty = r.y + r.height/2 + r.height*_simil[3];
  cv::MatIterator_<double> sx = _rshape.begin<double>();
  cv::MatIterator_<double> sy = _rshape.begin<double>()+n;
  cv::MatIterator_<double> dx =   shape.begin<double>();
  cv::MatIterator_<double> dy =   shape.begin<double>()+n;
  for(i = 0; i < n; i++,++sx,++sy,++dx,++dy){
    *dx = a*(*sx) - b*(*sy) + tx; *dy = b*(*sx) + a*(*sy) + ty;
  }return;
}
//===========================================================================
cv::Rect Tracker::ReDetect(cv::Mat &im)
{
  int x,y; float v,vb=-2;
  int ww = im.cols,hh = im.rows;
  int w = TSCALE*ww-temp_.cols+1,h = TSCALE*hh-temp_.rows+1;
  if((small_.rows != TSCALE*hh) || (small_.cols != TSCALE*ww))
    small_.create(TSCALE*hh,TSCALE*ww,CV_8U);
  cv::resize(im,small_,cv::Size(TSCALE*ww,TSCALE*hh),0,0,CV_INTER_LINEAR);
  if((ncc_.rows != h) || (ncc_.cols != w))ncc_.create(h,w,CV_32F);
  IplImage im_o = small_,temp_o = temp_,ncc_o = ncc_;
  cvMatchTemplate(&im_o,&temp_o,&ncc_o,CV_TM_CCOEFF_NORMED);
  cv::MatIterator_<float> p = ncc_.begin<float>(); cv::Rect R;  
  R.width = temp_.cols; R.height = temp_.rows;
  for(y = 0; y < h; y++){
    for(x = 0; x < w; x++){
      v = *p++; if(v > vb){vb = v; R.x = x; R.y = y;}
    }
  }
  R.x *= 1.0/TSCALE; R.y *= 1.0/TSCALE; 
  R.width *= 1.0/TSCALE; R.height *= 1.0/TSCALE; return R;
}
//===========================================================================
cv::Rect Tracker::UpdateTemplate(cv::Mat &im,cv::Mat &s,bool rsize)
{
  int i,n = s.rows/2; double vx,vy;
  cv::MatIterator_<double> x = s.begin<double>(),y = s.begin<double>()+n;
  double xmax=*x,ymax=*y,xmin=*x,ymin=*y;
  for(i = 0; i < n; i++){
    vx = *x++; vy = *y++;
    xmax = std::max(xmax,vx); ymax = std::max(ymax,vy);
    xmin = std::min(xmin,vx); ymin = std::min(ymin,vy);
  }
  if((xmin < 0) || (ymin < 0) || (xmax >= im.cols) || (ymax >= im.rows) ||
     cvIsNaN(xmin) || cvIsInf(xmin) || cvIsNaN(xmax) || cvIsInf(xmax) ||
     cvIsNaN(ymin) || cvIsInf(ymin) || cvIsNaN(ymax) || cvIsInf(ymax))
    return cv::Rect(0,0,0,0);
  else{
    xmin *= TSCALE; ymin *= TSCALE; xmax *= TSCALE; ymax *= TSCALE;
    cv::Rect R = cv::Rect(std::floor(xmin),std::floor(ymin),
			  std::ceil(xmax-xmin),std::ceil(ymax-ymin));
    int ww = im.cols,hh = im.rows;
    if(rsize){
      if((small_.rows != TSCALE*hh) || (small_.cols != TSCALE*ww))
	small_.create(TSCALE*hh,TSCALE*ww,CV_8U);
      cv::resize(im,small_,cv::Size(TSCALE*ww,TSCALE*hh),0,0,CV_INTER_LINEAR);
    }
		if (temp_.rows > 0)
			R.width = (((R.width) < (temp_.rows)) ? (R.width) : (temp_.rows));  
		if (temp_.cols > 0)
			R.height = (((R.height) < (temp_.cols)) ? (R.height) : (temp_.cols)); 
    temp_ = small_(R).clone(); 
    R.x *= 1.0/TSCALE; R.y *= 1.0/TSCALE; 
    R.width *= 1.0/TSCALE; R.height *= 1.0/TSCALE; return R;
  }
}
//===========================================================================
