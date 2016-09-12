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
#ifndef __CLM_h_
#define __CLM_h_
#include <FaceTracker/PDM.h>
#include <FaceTracker/Patch.h>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A Constrained Local Model
  */
  class CLM{
  public:
    PDM                               _pdm;   /**< 3D Shape model           */
    cv::Mat                           _plocal;/**< local parameters         */
    cv::Mat                           _pglobl;/**< global parameters        */
    cv::Mat                           _refs;  /**< Reference shape          */
    std::vector<cv::Mat>              _cent;  /**< Centers/view (Euler)     */
    std::vector<cv::Mat>              _visi;  /**< Visibility for each view */
    std::vector<std::vector<MPatch> > _patch; /**< Patches/point/view       */
    
    CLM(){;}
    CLM(const char* fname){this->Load(fname);}
    CLM(PDM &s,cv::Mat &r, std::vector<cv::Mat> &c,
	std::vector<cv::Mat> &v,std::vector<std::vector<MPatch> > &p){
      this->Init(s,r,c,v,p);
    }
    CLM& operator=(CLM const&rhs);
    inline int nViews(){return _patch.size();}
    int GetViewIdx();
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    void Init(PDM &s,cv::Mat &r, std::vector<cv::Mat> &c,
	      std::vector<cv::Mat> &v,std::vector<std::vector<MPatch> > &p);
    void Fit(cv::Mat im, std::vector<int> &wSize,
	     int nIter = 10,double clamp = 3.0,double fTol = 0.0);
  private:
    cv::Mat cshape_,bshape_,oshape_,ms_,u_,g_,J_,H_; 
    std::vector<cv::Mat> prob_,pmem_,wmem_;
    void Optimize(int idx,int wSize,int nIter,
		  double fTol,double clamp,bool rigid);
  };
  //===========================================================================
}
#endif
