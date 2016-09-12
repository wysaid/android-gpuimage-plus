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
#include <FaceTracker/PDM.h>
#define db at<double>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
void AddOrthRow(cv::Mat &R)
{
  assert((R.rows == 3) && (R.cols == 3));
  R.db(2,0) = R.db(0,1)*R.db(1,2) - R.db(0,2)*R.db(1,1);
  R.db(2,1) = R.db(0,2)*R.db(1,0) - R.db(0,0)*R.db(1,2);
  R.db(2,2) = R.db(0,0)*R.db(1,1) - R.db(0,1)*R.db(1,0);
  return;
}
//=============================================================================
void MetricUpgrade(cv::Mat &R)
{
  assert((R.rows == 3) && (R.cols == 3));
  cv::SVD svd(R,cv::SVD::MODIFY_A);
  cv::Mat X = svd.u*svd.vt,W = cv::Mat::eye(3,3,CV_64F); 
  W.db(2,2) = determinant(X); R = svd.u*W*svd.vt; return;
}
//===========================================================================
void Euler2Rot(cv::Mat &R,const double pitch,const double yaw,const double roll,
	       bool full = true)
{
  if(full){if((R.rows != 3) || (R.cols != 3))R.create(3,3,CV_64F);}
  else{if((R.rows != 2) || (R.cols != 3))R.create(2,3,CV_64F);}
  double sina = sin(pitch), sinb = sin(yaw), sinc = sin(roll);
  double cosa = cos(pitch), cosb = cos(yaw), cosc = cos(roll);
  R.db(0,0) = cosb*cosc; R.db(0,1) = -cosb*sinc; R.db(0,2) = sinb;
  R.db(1,0) = cosa*sinc + sina*sinb*cosc;
  R.db(1,1) = cosa*cosc - sina*sinb*sinc;
  R.db(1,2) = -sina*cosb; if(full)AddOrthRow(R); return;
}
//===========================================================================
void Euler2Rot(cv::Mat &R,cv::Mat &p,bool full = true)
{
  assert((p.rows == 6) && (p.cols == 1));
  Euler2Rot(R,p.db(1,0),p.db(2,0),p.db(3,0),full); return;
}
//=============================================================================
void Rot2Euler(cv::Mat &R,double& pitch,double& yaw,double& roll)
{
  assert((R.rows == 3) && (R.cols == 3));
  double q[4];
  q[0] = sqrt(1+R.db(0,0)+R.db(1,1)+R.db(2,2))/2;
  q[1] = (R.db(2,1) - R.db(1,2)) / (4*q[0]) ;
  q[2] = (R.db(0,2) - R.db(2,0)) / (4*q[0]) ;
  q[3] = (R.db(1,0) - R.db(0,1)) / (4*q[0]) ;
  yaw  = asin(2*(q[0]*q[2] + q[1]*q[3]));
  pitch= atan2(2*(q[0]*q[1]-q[2]*q[3]),
	       q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3]); 
  roll = atan2(2*(q[0]*q[3]-q[1]*q[2]),
	       q[0]*q[0]+q[1]*q[1]-q[2]*q[2]-q[3]*q[3]);
  return;
}
//=============================================================================
void Rot2Euler(cv::Mat &R,cv::Mat &p)
{
  assert((p.rows == 6) && (p.cols == 1));
  Rot2Euler(R,p.db(1,0),p.db(2,0),p.db(3,0)); return;
}
//=============================================================================
void Align3Dto2DShapes(double& scale,double& pitch,double& yaw,double& roll,
		       double& x,double& y,cv::Mat &s2D,cv::Mat &s3D)
{
  assert((s2D.cols == 1) && (s3D.rows == 3*(s2D.rows/2)) && (s3D.cols == 1));
  int i,n = s2D.rows/2; double t2[2],t3[3];
  cv::Mat s2D_cpy = s2D.clone(),s3D_cpy = s3D.clone();
  cv::Mat X = (s2D_cpy.reshape(1,2)).t(),S = (s3D_cpy.reshape(1,3)).t();
  for(i = 0; i < 2; i++){cv::Mat v = X.col(i); t2[i] = sum(v)[0]/n; v-=t2[i];}
  for(i = 0; i < 3; i++){cv::Mat v = S.col(i); t3[i] = sum(v)[0]/n; v-=t3[i];}
  cv::Mat M = ((S.t()*S).inv(cv::DECOMP_CHOLESKY))*S.t()*X;
  cv::Mat MtM = M.t()*M; cv::SVD svd(MtM,cv::SVD::MODIFY_A);
  svd.w.db(0,0) = 1.0/sqrt(svd.w.db(0,0));
  svd.w.db(1,0) = 1.0/sqrt(svd.w.db(1,0));
  cv::Mat T(3,3,CV_64F);
  T(cv::Rect(0,0,3,2)) = svd.u*cv::Mat::diag(svd.w)*svd.vt*M.t();
  scale = 0.5*sum(T(cv::Rect(0,0,3,2)).mul(M.t()))[0];
  AddOrthRow(T); Rot2Euler(T,pitch,yaw,roll); T *= scale;
  x = t2[0] - (T.db(0,0)*t3[0] + T.db(0,1)*t3[1] + T.db(0,2)*t3[2]);
  y = t2[1] - (T.db(1,0)*t3[0] + T.db(1,1)*t3[1] + T.db(1,2)*t3[2]); return;
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
PDM& PDM::operator= (PDM const& rhs)
{   
  this->_V  = rhs._V.clone();  this->_E  = rhs._E.clone();
  this->_M  = rhs._M.clone();  this->S_  = rhs.S_.clone();
  this->R_  = rhs.R_.clone();  this->s_  = rhs.s_.clone();
  this->P_  = rhs.P_.clone();  this->Px_ = rhs.Px_.clone();
  this->Py_ = rhs.Py_.clone(); this->Pz_ = rhs.Pz_.clone();
  this->R1_ = rhs.R1_.clone(); this->R2_ = rhs.R2_.clone(); 
  this->R3_ = rhs.R3_.clone(); return *this;
}
//===========================================================================
void PDM::Init(cv::Mat &M, cv::Mat &V, cv::Mat &E)
{
  assert((M.type() == CV_64F) && (V.type() == CV_64F) && (E.type() == CV_64F));
  assert((V.rows == M.rows) && (V.cols == E.cols));
  _M = M.clone(); _V = V.clone(); _E = E.clone();
  S_.create(_M.rows,1,CV_64F);  
  R_.create(3,3,CV_64F); s_.create(_M.rows,1,CV_64F); P_.create(2,3,CV_64F);
  Px_.create(2,3,CV_64F); Py_.create(2,3,CV_64F); Pz_.create(2,3,CV_64F);
  R1_.create(3,3,CV_64F); R2_.create(3,3,CV_64F); R3_.create(3,3,CV_64F);  
  return;
}
//===========================================================================
void PDM::Clamp(cv::Mat &p,double c)
{
  assert((p.rows == _E.cols) && (p.cols == 1) && (p.type() == CV_64F));
  cv::MatIterator_<double> e  = _E.begin<double>();
  cv::MatIterator_<double> p1 =  p.begin<double>();
  cv::MatIterator_<double> p2 =  p.end<double>(); double v;
  for(; p1 != p2; ++p1,++e){
    v = c*sqrt(*e); if(fabs(*p1) > v){if(*p1 > 0.0)*p1=v; else *p1=-v;}
  }return;
}
//===========================================================================
void PDM::CalcShape3D(cv::Mat &s,cv::Mat &plocal)
{
  assert((s.type() == CV_64F) && (plocal.type() == CV_64F));
  assert((s.rows == _M.rows) && (s.cols = 1));
  assert((plocal.rows == _E.cols) && (plocal.cols == 1));
  s = _M + _V*plocal; return;
}
//===========================================================================
void PDM::CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((s.type() == CV_64F) && (plocal.type() == CV_64F) && 
	 (pglobl.type() == CV_64F));
  assert((plocal.rows == _E.cols) && (plocal.cols == 1));
  assert((pglobl.rows == 6) && (pglobl.cols == 1));
  int n = _M.rows/3; double a=pglobl.db(0,0),x=pglobl.db(4,0),y=pglobl.db(5,0);
  Euler2Rot(R_,pglobl); S_ = _M + _V*plocal;
  if((s.rows != _M.rows) || (s.cols = 1))s.create(2*n,1,CV_64F);
  for(int i = 0; i < n; i++){
    s.db(i  ,0) = a*( R_.db(0,0)*S_.db(i    ,0) + R_.db(0,1)*S_.db(i+n  ,0) +
		      R_.db(0,2)*S_.db(i+n*2,0) )+x;
    s.db(i+n,0) = a*( R_.db(1,0)*S_.db(i    ,0) + R_.db(1,1)*S_.db(i+n  ,0) +
		      R_.db(1,2)*S_.db(i+n*2,0) )+y;
  }return;
}
//===========================================================================
void PDM::CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl)
{  
  assert((s.type() == CV_64F) && (s.rows == 2*(_M.rows/3)) && (s.cols = 1));
  if((pglobl.rows != 6) || (pglobl.cols != 1) || (pglobl.type() != CV_64F))
    pglobl.create(6,1,CV_64F);
  int j,n = _M.rows/3; double si,scale,pitch,yaw,roll,tx,ty,Tx,Ty,Tz; 
  cv::Mat R(3,3,CV_64F),z(n,1,CV_64F),t(3,1,CV_64F),p(_V.cols,1,CV_64F);  
  cv::Mat r = R.row(2), S(this->nPoints(),3,CV_64F);
  plocal = cv::Mat::zeros(_V.cols,1,CV_64F); 
  for(int iter = 0; iter < 100; iter++){
    this->CalcShape3D(S_,plocal);
    Align3Dto2DShapes(scale,pitch,yaw,roll,tx,ty,s,S_);
    Euler2Rot(R,pitch,yaw,roll); S = (S_.reshape(1,3)).t();
    z = scale*S*r.t(); si = 1.0/scale; 
    Tx = -si*(R.db(0,0)*tx + R.db(1,0)*ty);
    Ty = -si*(R.db(0,1)*tx + R.db(1,1)*ty);
    Tz = -si*(R.db(0,2)*tx + R.db(1,2)*ty);
    for(j = 0; j < n; j++){
      t.db(0,0) = s.db(j,0); t.db(1,0) = s.db(j+n,0); t.db(2,0) = z.db(j,0);
      S_.db(j    ,0) = si*t.dot(R.col(0))+Tx;
      S_.db(j+n  ,0) = si*t.dot(R.col(1))+Ty;
      S_.db(j+n*2,0) = si*t.dot(R.col(2))+Tz;
    }
    plocal = _V.t()*(S_-_M); 
    if(iter > 0){if(cv::norm(plocal-p) < 1.0e-5)break;}
    plocal.copyTo(p);
  }
  pglobl.db(0,0) = scale; pglobl.db(1,0) = pitch;
  pglobl.db(2,0) = yaw;   pglobl.db(3,0) = roll;
  pglobl.db(4,0) = tx;    pglobl.db(5,0) = ty;
  return;
}
//===========================================================================
void PDM::Identity(cv::Mat &plocal,cv::Mat &pglobl)
{
  plocal = cv::Mat::zeros(_V.cols,1,CV_64F);
  pglobl = (cv::Mat_<double>(6,1) << 1, 0, 0, 0, 0, 0);
}
//===========================================================================
void PDM::CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{
  int i,n = _M.rows/3,m = _V.cols; double X,Y,Z;
  assert((plocal.rows == m)  && (plocal.cols == 1) && 
	 (pglobl.rows == 6)  && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*n) && (Jacob.cols == 6));
  double rx[3][3] = {{0,0,0},{0,0,-1},{0,1,0}}; cv::Mat Rx(3,3,CV_64F,rx);
  double ry[3][3] = {{0,0,1},{0,0,0},{-1,0,0}}; cv::Mat Ry(3,3,CV_64F,ry);
  double rz[3][3] = {{0,-1,0},{1,0,0},{0,0,0}}; cv::Mat Rz(3,3,CV_64F,rz);
  double s = pglobl.db(0,0);
  this->CalcShape3D(S_,plocal); Euler2Rot(R_,pglobl); 
  P_ = s*R_(cv::Rect(0,0,3,2)); Px_ = P_*Rx; Py_ = P_*Ry; Pz_ = P_*Rz;
  assert(R_.isContinuous() && Px_.isContinuous() && 
	 Py_.isContinuous() && Pz_.isContinuous());
  const double* px = Px_.ptr<double>(0);
  const double* py = Py_.ptr<double>(0);
  const double* pz = Pz_.ptr<double>(0);
  const double* r  =  R_.ptr<double>(0);
  cv::MatIterator_<double> Jx = Jacob.begin<double>();
  cv::MatIterator_<double> Jy = Jx + n*6;
  for(i = 0; i < n; i++){
    X=S_.db(i,0); Y=S_.db(i+n,0); Z=S_.db(i+n*2,0);    
    *Jx++ =  r[0]*X +  r[1]*Y +  r[2]*Z;
    *Jy++ =  r[3]*X +  r[4]*Y +  r[5]*Z;
    *Jx++ = px[0]*X + px[1]*Y + px[2]*Z;
    *Jy++ = px[3]*X + px[4]*Y + px[5]*Z;
    *Jx++ = py[0]*X + py[1]*Y + py[2]*Z;
    *Jy++ = py[3]*X + py[4]*Y + py[5]*Z;
    *Jx++ = pz[0]*X + pz[1]*Y + pz[2]*Z;
    *Jy++ = pz[3]*X + pz[4]*Y + pz[5]*Z;
    *Jx++ = 1.0; *Jy++ = 0.0; *Jx++ = 0.0; *Jy++ = 1.0;
  }return;
}
//===========================================================================
void PDM::CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob)
{ 
  int i,j,n = _M.rows/3,m = _V.cols; double X,Y,Z;
  assert((plocal.rows == m)  && (plocal.cols == 1) && 
	 (pglobl.rows == 6)  && (pglobl.cols == 1) &&
	 (Jacob.rows == 2*n) && (Jacob.cols == 6+m));
  double s = pglobl.db(0,0);
  double rx[3][3] = {{0,0,0},{0,0,-1},{0,1,0}}; cv::Mat Rx(3,3,CV_64F,rx);
  double ry[3][3] = {{0,0,1},{0,0,0},{-1,0,0}}; cv::Mat Ry(3,3,CV_64F,ry);
  double rz[3][3] = {{0,-1,0},{1,0,0},{0,0,0}}; cv::Mat Rz(3,3,CV_64F,rz);
  this->CalcShape3D(S_,plocal); Euler2Rot(R_,pglobl); 
  P_ = s*R_(cv::Rect(0,0,3,2)); Px_ = P_*Rx; Py_ = P_*Ry; Pz_ = P_*Rz;
  assert(R_.isContinuous() && Px_.isContinuous() && 
	 Py_.isContinuous() && Pz_.isContinuous() && P_.isContinuous());
  const double* px = Px_.ptr<double>(0);
  const double* py = Py_.ptr<double>(0);
  const double* pz = Pz_.ptr<double>(0);
  const double* p  =  P_.ptr<double>(0);
  const double* r  =  R_.ptr<double>(0);
  cv::MatIterator_<double> Jx =  Jacob.begin<double>();
  cv::MatIterator_<double> Jy =  Jx + n*(6+m);
  cv::MatIterator_<double> Vx =  _V.begin<double>();
  cv::MatIterator_<double> Vy =  Vx + n*m;
  cv::MatIterator_<double> Vz =  Vy + n*m;
  for(i = 0; i < n; i++){
    X=S_.db(i,0); Y=S_.db(i+n,0); Z=S_.db(i+n*2,0);    
    *Jx++ =  r[0]*X +  r[1]*Y +  r[2]*Z;
    *Jy++ =  r[3]*X +  r[4]*Y +  r[5]*Z;
    *Jx++ = px[0]*X + px[1]*Y + px[2]*Z;
    *Jy++ = px[3]*X + px[4]*Y + px[5]*Z;
    *Jx++ = py[0]*X + py[1]*Y + py[2]*Z;
    *Jy++ = py[3]*X + py[4]*Y + py[5]*Z;
    *Jx++ = pz[0]*X + pz[1]*Y + pz[2]*Z;
    *Jy++ = pz[3]*X + pz[4]*Y + pz[5]*Z;
    *Jx++ = 1.0; *Jy++ = 0.0; *Jx++ = 0.0; *Jy++ = 1.0;
    for(j = 0; j < m; j++,++Vx,++Vy,++Vz){
      *Jx++ = p[0]*(*Vx) + p[1]*(*Vy) + p[2]*(*Vz);
      *Jy++ = p[3]*(*Vx) + p[4]*(*Vy) + p[5]*(*Vz);
    }
  }return;
}
//===========================================================================
void PDM::CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl)
{
  assert((dp.rows == 6+_V.cols) && (dp.cols == 1));
  plocal += dp(cv::Rect(0,6,1,_V.cols));
  pglobl.db(0,0) += dp.db(0,0);
  pglobl.db(4,0) += dp.db(4,0);
  pglobl.db(5,0) += dp.db(5,0);
  Euler2Rot(R1_,pglobl); R2_ = cv::Mat::eye(3,3,CV_64F);
  R2_.db(1,2) = -1.0*(R2_.db(2,1) = dp.db(1,0));
  R2_.db(2,0) = -1.0*(R2_.db(0,2) = dp.db(2,0));
  R2_.db(0,1) = -1.0*(R2_.db(1,0) = dp.db(3,0));
  MetricUpgrade(R2_); R3_ = R1_*R2_; Rot2Euler(R3_,pglobl); return;
}
//===========================================================================
void PDM::ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl)
{
  assert((pglobl.rows == 6) && (pglobl.cols == 1) && (pglobl.type()==CV_64F));
  double angle = atan2(b,a),scale = a/cos(angle);
  double ca = cos(angle),sa = sin(angle);
  double xc = pglobl.db(4,0),yc = pglobl.db(5,0);
  R1_ = cv::Scalar(0); R1_.db(2,2) = 1.0;
  R1_.db(0,0) =  ca; R1_.db(0,1) = -sa; R1_.db(1,0) =  sa; R1_.db(1,1) =  ca;
  Euler2Rot(R2_,pglobl); R3_ = R1_*R2_; 
  pglobl.db(0,0) *= scale; Rot2Euler(R3_,pglobl);
  pglobl.db(4,0) = a*xc - b*yc + tx; pglobl.db(5,0) = b*xc + a*yc + ty;
  return;
}
//===========================================================================
void PDM::Read(istream &s,bool readType)
{
  if(readType){int type; s >> type; assert(type == IO::PDM);}
  IO::ReadMat(s,_V); IO::ReadMat(s,_E); IO::ReadMat(s,_M); 
  S_.create(_M.rows,1,CV_64F);  
  R_.create(3,3,CV_64F); s_.create(_M.rows,1,CV_64F); P_.create(2,3,CV_64F);
  Px_.create(2,3,CV_64F); Py_.create(2,3,CV_64F); Pz_.create(2,3,CV_64F);
  R1_.create(3,3,CV_64F); R2_.create(3,3,CV_64F); R3_.create(3,3,CV_64F);
  return;
}
//===========================================================================
void PDM::Write(ofstream &s)
{
  s << IO::PDM << " ";
  IO::WriteMat(s,_V); IO::WriteMat(s,_E); IO::WriteMat(s,_M); return;
}
//===========================================================================
void PDM::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//===========================================================================
void PDM::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
