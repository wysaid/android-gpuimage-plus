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
#ifndef __Tracker_h_
#define __Tracker_h_
#include <FaceTracker/CLM.h>
#include <FaceTracker/FDet.h>
#include <FaceTracker/FCheck.h>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      Face Tracker
  */
  class Tracker{
  public:    
    CLM        _clm;    /**< Constrained Local Model           */
    FDet       _fdet;   /**< Face Detector                     */
    int64      _frame;  /**< Frame number since last detection */    
    MFCheck    _fcheck; /**< Failure checker                   */
    cv::Mat    _shape;  /**< Current shape                     */
    cv::Mat    _rshape; /**< Reference shape                   */
    cv::Rect   _rect;   /**< Detected rectangle                */
    cv::Scalar _simil;  /**< Initialization similarity         */
    
    /** NULL constructor */
    Tracker(){;}
    
    /** Constructor from model file */
    Tracker(const char* fname){this->Load(fname);}

    /** Constructor from components */
    Tracker(CLM &clm,FDet &fdet,MFCheck &fcheck,
	    cv::Mat &rshape,cv::Scalar &simil){
      this->Init(clm,fdet,fcheck,rshape,simil);
    }
    /**
       Track model in current frame
       @param im     Image containing face
       @param wSize  List of search window sizes (set from large to small)
       @param fpd    Number of frames between detections (-1: never)
       @param nIter  Maximum number of optimization steps to perform.
       @param clamp  Shape model parameter clamping factor (in standard dev's)
       @param fTol   Convergence tolerance of optimization
       @param fcheck Check if tracking succeeded?
       @return       -1 on failure, 0 otherwise.
    */
    int Track(cv::Mat im,std::vector<int> &wSize,
	      const int    fpd    =-1,
	      const int    nIter  = 10,
	      const double clamp  = 3.0,
	      const double fTol   = 0.01,
	      const bool   fcheck = true);

    /** Reset frame number (will perform detection in next image) */
    inline void FrameReset(){_frame = -1;}

    /** Load tracker from model file */
    void Load(const char* fname);

    //Add by wysaid.
    void LoadFromData(const char* data);

    /** Save tracker to model file */
    void Save(const char* fname);
    
    /** Write tracker to file stream */
    void Write(std::ofstream &s);

    /** Read tracking from file stream */
    void Read(std::istream &s,bool readType = true);

  private:
    cv::Mat gray_,temp_,ncc_,small_;
    void Init(CLM &clm,FDet &fdet,MFCheck &fcheck,
	      cv::Mat &rshape,cv::Scalar &simil);    
    void InitShape(cv::Rect &r,cv::Mat &shape);
    cv::Rect ReDetect(cv::Mat &im);
    cv::Rect UpdateTemplate(cv::Mat &im,cv::Mat &s,bool rsize);
  };
  //===========================================================================
}
#endif
