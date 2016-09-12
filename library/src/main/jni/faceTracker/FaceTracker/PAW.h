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
#ifndef __PAW_h_
#define __PAW_h_
#include <FaceTracker/IO.h>
namespace FACETRACKER
{
  //===========================================================================
  /** 
      A Piecewise Affine Warp
  */
  class PAW{
  public:    
    int     _nPix;   /**< Number of pixels                   */
    double  _xmin;   /**< Minimum x-coord for src            */
    double  _ymin;   /**< Minimum y-coord for src            */
    cv::Mat _src;    /**< Source points                      */
    cv::Mat _dst;    /**< destination points                 */
    cv::Mat _tri;    /**< Triangulation                      */
    cv::Mat _tridx;  /**< Triangle for each valid pixel      */
    cv::Mat _mask;   /**< Valid region mask                  */
    cv::Mat _coeff;  /**< affine coeffs for all triangles    */
    cv::Mat _alpha;  /**< matrix of (c,x,y) coeffs for alpha */
    cv::Mat _beta;   /**< matrix of (c,x,y) coeffs for alpha */
    cv::Mat _mapx;   /**< x-destination of warped points     */
    cv::Mat _mapy;   /**< y-destination of warped points     */

    PAW(){;}
    PAW(const char* fname){this->Load(fname);}
    PAW(cv::Mat &src,cv::Mat &tri){this->Init(src,tri);}
    PAW& operator=(PAW const&rhs);
    inline int nPoints(){return _src.rows/2;}
    inline int nTri(){return _tri.rows;}
    inline int Width(){return _mask.cols;}
    inline int Height(){return _mask.rows;}
    void Load(const char* fname);
    void Save(const char* fname);
    void Write(std::ofstream &s);
    void Read(std::istream &s,bool readType = true);
    void Init(cv::Mat &src,cv::Mat &tri);
    void Crop(cv::Mat &src, cv::Mat &dst,cv::Mat &s);

  private:    
    void CalcCoeff();
    void WarpRegion(cv::Mat &mapx,cv::Mat &mapy);
  };
  //===========================================================================
}
#endif
