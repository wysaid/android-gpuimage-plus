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
#include <FaceTracker/CLM.h>
#define it at<int>
#define db at<double>
#define SQR(x) x*x
using namespace FACETRACKER;
using namespace std;
//=============================================================================
void CalcSimT(cv::Mat &src,cv::Mat &dst,
	      double &a,double &b,double &tx,double &ty)
{
  assert((src.type() == CV_64F) && (dst.type() == CV_64F) && 
	 (src.rows == dst.rows) && (src.cols == dst.cols) && (src.cols == 1));
  int i,n = src.rows/2;
  cv::Mat H(4,4,CV_64F,cv::Scalar(0));
  cv::Mat g(4,1,CV_64F,cv::Scalar(0));
  cv::Mat p(4,1,CV_64F);
  cv::MatIterator_<double> ptr1x = src.begin<double>();
  cv::MatIterator_<double> ptr1y = src.begin<double>()+n;
  cv::MatIterator_<double> ptr2x = dst.begin<double>();
  cv::MatIterator_<double> ptr2y = dst.begin<double>()+n;
  for(i = 0; i < n; i++,++ptr1x,++ptr1y,++ptr2x,++ptr2y){
    H.db(0,0) += SQR(*ptr1x) + SQR(*ptr1y);
    H.db(0,2) += *ptr1x; H.db(0,3) += *ptr1y;
    g.db(0,0) += (*ptr1x)*(*ptr2x) + (*ptr1y)*(*ptr2y);
    g.db(1,0) += (*ptr1x)*(*ptr2y) - (*ptr1y)*(*ptr2x);
    g.db(2,0) += *ptr2x; g.db(3,0) += *ptr2y;
  }
  H.db(1,1) = H.db(0,0); H.db(1,2) = H.db(2,1) = -1.0*(H.db(3,0) = H.db(0,3));
  H.db(1,3) = H.db(3,1) = H.db(2,0) = H.db(0,2); H.db(2,2) = H.db(3,3) = n;
  cv::solve(H,g,p,CV_CHOLESKY);
  a = p.db(0,0); b = p.db(1,0); tx = p.db(2,0); ty = p.db(3,0); return;
}
//=============================================================================
void invSimT(double a1,double b1,double tx1,double ty1,
	     double& a2,double& b2,double& tx2,double& ty2)
{
  cv::Mat M = (cv::Mat_<double>(2,2) << a1, -b1, b1, a1);
  cv::Mat N = M.inv(CV_SVD); a2 = N.db(0,0); b2 = N.db(1,0);
  tx2 = -1.0*(N.db(0,0)*tx1 + N.db(0,1)*ty1);
  ty2 = -1.0*(N.db(1,0)*tx1 + N.db(1,1)*ty1); return;
}
//=============================================================================
void SimT(cv::Mat &s,double a,double b,double tx,double ty)
{
  assert((s.type() == CV_64F) && (s.cols == 1));
  int i,n = s.rows/2; double x,y; 
  cv::MatIterator_<double> xp = s.begin<double>(),yp = s.begin<double>()+n;
  for(i = 0; i < n; i++,++xp,++yp){
    x = *xp; y = *yp; *xp = a*x - b*y + tx; *yp = b*x + a*y + ty;    
  }return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
CLM& CLM::operator= (CLM const& rhs)
{
  this->_pdm = rhs._pdm;
  this->_plocal = rhs._plocal.clone();
  this->_pglobl = rhs._pglobl.clone();
  this->_refs = rhs._refs.clone();
  this->_cent.resize(rhs._cent.size());
  this->_visi.resize(rhs._visi.size());
  this->_patch.resize(rhs._patch.size());
  for(int i = 0; i < (int)rhs._cent.size(); i++){
    this->_cent[i] = rhs._cent[i].clone();
    this->_visi[i] = rhs._visi[i].clone();
    this->_patch[i].resize(rhs._patch[i].size());
    for(int j = 0; j < (int)rhs._patch[i].size(); j++)
      this->_patch[i][j] = rhs._patch[i][j];
  }
  this->cshape_ = rhs.cshape_.clone();
  this->bshape_ = rhs.bshape_.clone();
  this->oshape_ = rhs.oshape_.clone();  
  this->ms_ = rhs.cshape_.clone();
  this->u_  = rhs.u_.clone();
  this->g_  = rhs.g_.clone();
  this->J_  = rhs.J_.clone();
  this->H_  = rhs.H_.clone();
  this->prob_.resize(rhs.prob_.size());
  this->pmem_.resize(rhs.pmem_.size());
  this->wmem_.resize(rhs.pmem_.size());
  for(int i = 0; i < (int)rhs.prob_.size(); i++){
    this->prob_[i] = rhs.prob_[i].clone();
    this->pmem_[i] = rhs.pmem_[i].clone();
    this->wmem_[i] = rhs.wmem_[i].clone();
  }return *this;
}
//=============================================================================
void CLM::Init(PDM &s,cv::Mat &r, vector<cv::Mat> &c,
	       vector<cv::Mat> &v,vector<vector<MPatch> > &p)
{
  int n = p.size(); assert(((int)c.size() == n) && ((int)v.size() == n));
  assert((r.type() == CV_64F) && (r.rows == 2*s.nPoints()) && (r.cols == 1));
  for(int i = 0; i < n; i++){
    assert((int)p[i].size() == s.nPoints());
    assert((c[i].type() == CV_64F) && (c[i].rows == 3) && (c[i].cols == 1));
    assert((v[i].type() == CV_32S) && (v[i].rows == s.nPoints()) && 
	   (v[i].cols == 1));
  }
  _pdm = s; _refs = r.clone();_cent.resize(n);_visi.resize(n);_patch.resize(n);
  for(int i = 0; i < n; i++){
    _cent[i] = c[i].clone(); _visi[i] = v[i].clone();
    _patch[i].resize(p[i].size());
    for(int j = 0; j < (int)p[i].size(); j++)_patch[i][j] = p[i][j];
  }
  _plocal.create(_pdm.nModes(),1,CV_64F);
  _pglobl.create(6,1,CV_64F);
  cshape_.create(2*_pdm.nPoints(),1,CV_64F);
  bshape_.create(2*_pdm.nPoints(),1,CV_64F);
  oshape_.create(2*_pdm.nPoints(),1,CV_64F);
  ms_.create(2*_pdm.nPoints(),1,CV_64F);
  u_.create(6+_pdm.nModes(),1,CV_64F);
  g_.create(6+_pdm.nModes(),1,CV_64F);
  J_.create(2*_pdm.nPoints(),6+_pdm.nModes(),CV_64F);
  H_.create(6+_pdm.nModes(),6+_pdm.nModes(),CV_64F);
  prob_.resize(_pdm.nPoints()); pmem_.resize(_pdm.nPoints()); 
  wmem_.resize(_pdm.nPoints()); return;
}
//=============================================================================
void CLM::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//=============================================================================
void CLM::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//=============================================================================
void CLM::Write(ofstream &s)
{
  s << IO::CLM << " " << _patch.size() << " "; 
  _pdm.Write(s); IO::WriteMat(s,_refs);
  for(int i = 0; i < (int)_cent.size(); i++)IO::WriteMat(s,_cent[i]);
  for(int i = 0; i < (int)_visi.size(); i++)IO::WriteMat(s,_visi[i]);
  for(int i = 0; i < (int)_patch.size(); i++){
    for(int j = 0; j < _pdm.nPoints(); j++)_patch[i][j].Write(s);
  }return;
}
//=============================================================================
void CLM::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::CLM);}
  int n; s >> n; _pdm.Read(s); _cent.resize(n);_visi.resize(n);
  _patch.resize(n); IO::ReadMat(s,_refs);
  for(int i = 0; i < (int)_cent.size(); i++)IO::ReadMat(s,_cent[i]);
  for(int i = 0; i < (int)_visi.size(); i++)IO::ReadMat(s,_visi[i]);
  for(int i = 0; i < (int)_patch.size(); i++){
    _patch[i].resize(_pdm.nPoints());
    for(int j = 0; j < _pdm.nPoints(); j++)_patch[i][j].Read(s);
  }
  _plocal.create(_pdm.nModes(),1,CV_64F);
  _pglobl.create(6,1,CV_64F);
  cshape_.create(2*_pdm.nPoints(),1,CV_64F);
  bshape_.create(2*_pdm.nPoints(),1,CV_64F);
  oshape_.create(2*_pdm.nPoints(),1,CV_64F);
  ms_.create(2*_pdm.nPoints(),1,CV_64F);
  u_.create(6+_pdm.nModes(),1,CV_64F);
  g_.create(6+_pdm.nModes(),1,CV_64F);
  J_.create(2*_pdm.nPoints(),6+_pdm.nModes(),CV_64F);
  H_.create(6+_pdm.nModes(),6+_pdm.nModes(),CV_64F);
  prob_.resize(_pdm.nPoints()); pmem_.resize(_pdm.nPoints()); 
  wmem_.resize(_pdm.nPoints()); return;
}
//=============================================================================
int CLM::GetViewIdx()
{
  int idx=0;
  if(this->nViews() == 1)return 0;
  else{
    int i; double v1,v2,v3,d,dbest = -1.0;
    for(i = 0; i < this->nViews(); i++){
      v1 = _pglobl.db(1,0) - _cent[i].db(0,0);
      v2 = _pglobl.db(2,0) - _cent[i].db(1,0);
      v3 = _pglobl.db(3,0) - _cent[i].db(2,0);
      d = v1*v1 + v2*v2 + v3*v3;
      if(dbest < 0 || d < dbest){dbest = d; idx = i;}
    }return idx;
  }
}
//=============================================================================
void CLM::Fit(cv::Mat im, vector<int> &wSize,
	      int nIter,double clamp,double fTol)
{
  assert(im.type() == CV_8U);
  int i,idx,n = _pdm.nPoints(); double a1,b1,tx1,ty1,a2,b2,tx2,ty2;
  for(int witer = 0; witer < (int)wSize.size(); witer++){
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    CalcSimT(_refs,cshape_,a1,b1,tx1,ty1);
    invSimT(a1,b1,tx1,ty1,a2,b2,tx2,ty2);
    idx = this->GetViewIdx();
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(i = 0; i < n; i++){
      if(_visi[idx].rows == n){if(_visi[idx].it(i,0) == 0)continue;}
      int w = wSize[witer]+_patch[idx][i]._w - 1; 
      int h = wSize[witer]+_patch[idx][i]._h - 1;
      cv::Mat sim = 
	(cv::Mat_<float>(2,3)<<a1,-b1,cshape_.db(i,0),b1,a1,cshape_.db(i+n,0));
      if((w>wmem_[i].cols) || (h>wmem_[i].rows))wmem_[i].create(h,w,CV_32F);
      cv::Mat wimg = wmem_[i](cv::Rect(0,0,w,h));
      CvMat wimg_o = wimg,sim_o = sim; IplImage im_o = im;
      cvGetQuadrangleSubPix(&im_o,&wimg_o,&sim_o);
      if(wSize[witer] > pmem_[i].rows)
	pmem_[i].create(wSize[witer],wSize[witer],CV_64F);
      prob_[i] = pmem_[i](cv::Rect(0,0,wSize[witer],wSize[witer]));
      _patch[idx][i].Response(wimg,prob_[i]);
    }
    SimT(cshape_,a2,b2,tx2,ty2); 
    _pdm.ApplySimT(a2,b2,tx2,ty2,_pglobl);
    cshape_.copyTo(bshape_);
    this->Optimize(idx,wSize[witer],nIter,fTol,clamp,1);
    this->Optimize(idx,wSize[witer],nIter,fTol,clamp,0);
    _pdm.ApplySimT(a1,b1,tx1,ty1,_pglobl);
  }return;
}
//=============================================================================
void CLM::Optimize(int idx,int wSize,int nIter,
		   double fTol,double clamp,bool rigid)
{
  int i,m=_pdm.nModes(),n=_pdm.nPoints();  
  double var,sigma=(wSize*wSize)/36.0; cv::Mat u,g,J,H; 
  if(rigid){
    u = u_(cv::Rect(0,0,1,6));   g = g_(cv::Rect(0,0,1,6)); 
    J = J_(cv::Rect(0,0,6,2*n)); H = H_(cv::Rect(0,0,6,6));
  }else{u = u_; g = g_; J = J_; H = H_;}
  for(int iter = 0; iter < nIter; iter++){
    _pdm.CalcShape2D(cshape_,_plocal,_pglobl);
    if(iter > 0){if(cv::norm(cshape_,oshape_) < fTol)break;}
    cshape_.copyTo(oshape_);
    if(rigid)_pdm.CalcRigidJacob(_plocal,_pglobl,J);
    else     _pdm.CalcJacob(_plocal,_pglobl,J);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(i = 0; i < n; i++){
      if(_visi[idx].rows == n){
	if(_visi[idx].it(i,0) == 0){
	  cv::Mat Jx = J.row(i  ); Jx = cvScalar(0);
	  cv::Mat Jy = J.row(i+n); Jy = cvScalar(0);
	  ms_.db(i,0) = 0.0; ms_.db(i+n,0) = 0.0; continue;
	}
      }
      double dx = cshape_.db(i  ,0) - bshape_.db(i  ,0) + (wSize-1)/2;
      double dy = cshape_.db(i+n,0) - bshape_.db(i+n,0) + (wSize-1)/2;
      int ii,jj; double v,vx,vy,mx=0.0,my=0.0,sum=0.0;      
      cv::MatIterator_<double> p = prob_[i].begin<double>();
      for(ii = 0; ii < wSize; ii++){   vx = (dy-ii)*(dy-ii);
	for(jj = 0; jj < wSize; jj++){ vy = (dx-jj)*(dx-jj);
	  v = *p++; v *= exp(-0.5*(vx+vy)/sigma);
	  sum += v;  mx += v*jj;  my += v*ii; 
	}
      }
      ms_.db(i,0) = mx/sum - dx; ms_.db(i+n,0) = my/sum - dy;
    }
    g = J.t()*ms_; H = J.t()*J;
    if(!rigid){
      for(i = 0; i < m; i++){
	var = 0.5*sigma/_pdm._E.db(0,i);
	H.db(6+i,6+i) += var; g.db(6+i,0) -= var*_plocal.db(i,0);
      }
    }
    u_ = cvScalar(0); cv::solve(H,g,u,CV_CHOLESKY);
    _pdm.CalcReferenceUpdate(u_,_plocal,_pglobl);
    if(!rigid)_pdm.Clamp(_plocal,clamp);
  }return;
}
//=============================================================================
