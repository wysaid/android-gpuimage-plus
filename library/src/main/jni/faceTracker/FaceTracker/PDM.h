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
#ifndef __PDM_h_
#define __PDM_h_
#include <FaceTracker/IO.h>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A 3D Point Distribution Model
  */
  class PDM{
  public:    
    cv::Mat _V; /**< basis of variation                            */
    cv::Mat _E; /**< vector of eigenvalues (row vector)            */
    cv::Mat _M; /**< mean 3D shape vector [x1,..,xn,y1,...yn]      */

    PDM(){;}
    PDM(const char* fname){this->Load(fname);}
    PDM(cv::Mat &M,cv::Mat &V,cv::Mat &E){this->Init(M,V,E);}
    PDM& operator=(PDM const&rhs);
    inline int nPoints(){return _M.rows/3;}
    inline int nModes(){return _V.cols;}
    inline double Var(int i){assert(i<_E.cols); return _E.at<double>(0,i);}
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    void Clamp(cv::Mat &p,double c);
    void Init(cv::Mat &M,cv::Mat &V,cv::Mat &E);
    void Identity(cv::Mat &plocal,cv::Mat &pglobl);
    void CalcShape3D(cv::Mat &s,cv::Mat &plocal);
    void CalcShape2D(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void CalcParams(cv::Mat &s,cv::Mat &plocal,cv::Mat &pglobl);
    void CalcRigidJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcJacob(cv::Mat &plocal,cv::Mat &pglobl,cv::Mat &Jacob);
    void CalcReferenceUpdate(cv::Mat &dp,cv::Mat &plocal,cv::Mat &pglobl);
    void ApplySimT(double a,double b,double tx,double ty,cv::Mat &pglobl);
    
  private:
    cv::Mat S_,R_,s_,P_,Px_,Py_,Pz_,R1_,R2_,R3_;
  };
  //===========================================================================
}
#endif
