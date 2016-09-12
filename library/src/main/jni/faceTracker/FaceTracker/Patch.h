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
#ifndef __Patch_h_
#define __Patch_h_
#include <FaceTracker/IO.h>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A Patch Expert
  */
  class Patch{
  public:
    int     _t; /**< Type of patch (0=raw,1=grad,2=lbp) */
    double  _a; /**< scaling                            */
    double  _b; /**< bias                               */
    cv::Mat _W; /**< Gain                               */
    
    Patch(){;}
    Patch(const char* fname){this->Load(fname);}
    Patch(int t,double a,double b,cv::Mat &W){this->Init(t,a,b,W);}
    Patch& operator=(Patch const&rhs);
    inline int w(){return _W.cols;}
    inline int h(){return _W.rows;}
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    void Init(int t, double a, double b, cv::Mat &W);
    void Response(cv::Mat &im,cv::Mat &resp);    

  private:
    cv::Mat im_,res_;
  };
  //===========================================================================
  /**
     A Multi-patch Expert
  */
  class MPatch{
  public:
    int _w,_h;             /**< Width and height of patch */
    std::vector<Patch> _p; /**< List of patches           */
    
    MPatch(){;}
    MPatch(const char* fname){this->Load(fname);}
    MPatch(std::vector<Patch> &p){this->Init(p);}
    MPatch& operator=(MPatch const&rhs);
    inline int nPatch(){return _p.size();}
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    void Init(std::vector<Patch> &p);
    void Response(cv::Mat &im,cv::Mat &resp);    

  private:
    cv::Mat res_;
  };
  //===========================================================================
}
#endif
