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
#ifndef __FCheck_h_
#define __FCheck_h_
#include <FaceTracker/PAW.h>
#include <vector>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      Checks for Tracking Failure
  */
  class FCheck{
  public:    
    PAW     _paw; /**< Piecewise affine warp */
    double  _b;   /**< SVM bias              */
    cv::Mat _w;   /**< SVM gain              */

    FCheck(){;}
    FCheck(const char* fname){this->Load(fname);}
    FCheck(double b, cv::Mat &w, PAW &paw){this->Init(b,w,paw);}
    FCheck& operator=(FCheck const&rhs);
    void Init(double b, cv::Mat &w, PAW &paw);
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    bool Check(cv::Mat &im,cv::Mat &s);
    
  private:
    cv::Mat crop_,vec_;
  };
  //===========================================================================
  /** 
      Checks for Multiview Tracking Failure
  */
  class MFCheck{
  public:    
    std::vector<FCheck> _fcheck; /**< FCheck for each view */
    
    MFCheck(){;}
    MFCheck(const char* fname){this->Load(fname);}
    MFCheck(std::vector<FCheck> &fcheck){this->Init(fcheck);}
    MFCheck& operator=(MFCheck const&rhs){      
      this->_fcheck = rhs._fcheck; return *this;
    }
    void Init(std::vector<FCheck> &fcheck){_fcheck = fcheck;}
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    bool Check(int idx,cv::Mat &im,cv::Mat &s);
  };
  //===========================================================================
}
#endif
