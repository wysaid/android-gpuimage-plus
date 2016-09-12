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
#include <FaceTracker/IO.h>
#include <stdio.h>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
void IO::ReadMat(istream& s,cv::Mat &M)
{
  int r,c,t; s >> r >> c >> t;
  M = cv::Mat(r,c,t);
  switch(M.type()){
  case CV_64FC1: 
    {
      cv::MatIterator_<double> i1 = M.begin<double>(),i2 = M.end<double>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_32FC1:
    {
      cv::MatIterator_<float> i1 = M.begin<float>(),i2 = M.end<float>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_32SC1:
    {
      cv::MatIterator_<int> i1 = M.begin<int>(),i2 = M.end<int>();
      while(i1 != i2)s >> *i1++;
    }break;
  case CV_8UC1:
    {
      cv::MatIterator_<uchar> i1 = M.begin<uchar>(),i2 = M.end<uchar>();
      //modified by wysaid: be compatible with istringstream.
      while(i1 != i2)
      {
          s >> *i1;
          if(*i1 >= '0')
              *i1 -= '0';
          i1++;
      }
    }break;
  default:
    printf("ERROR(%s,%d) : Unsupported Matrix type %d!\n", 
	   __FILE__,__LINE__,M.type()); abort();
  }return;
}
//===========================================================================
void IO::WriteMat(ofstream& s,cv::Mat &M)
{
  s << M.rows << " " << M.cols << " " << M.type() << " ";
  switch(M.type()){
  case CV_64FC1: 
    {
      cv::MatIterator_<double> i1 = M.begin<double>(),i2 = M.end<double>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_32FC1:
    {
      cv::MatIterator_<float> i1 = M.begin<float>(),i2 = M.end<float>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_32SC1:
    {
      cv::MatIterator_<int> i1 = M.begin<int>(),i2 = M.end<int>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  case CV_8UC1:
    {
      cv::MatIterator_<uchar> i1 = M.begin<uchar>(),i2 = M.end<uchar>();
      while(i1 != i2)s << *i1++ << " ";
    }break;
  default:
    printf("ERROR(%s,%d) : Unsupported Matrix type %d!\n", 
	   __FILE__,__LINE__,M.type()); abort();
  }return;
}
//===========================================================================
cv::Mat IO::LoadCon(const char* fname)
{
  int i,n; char str[256]; char c; fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening file %s for reading\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(1){file >> str; if(strncmp(str,"n_connections:",14) == 0)break;}
  file >> n; cv::Mat con(2,n,CV_32S);
  while(1){file >> c; if(c == '{')break;}
  for(i = 0; i < n; i++)file >> con.at<int>(0,i) >> con.at<int>(1,i);
  file.close(); return con;
}
//=============================================================================
cv::Mat IO::LoadTri(const char* fname)
{
  int i,n; char str[256]; char c; fstream file(fname,fstream::in);
  if(!file.is_open()){
    printf("ERROR(%s,%d) : Failed opening file %s for reading\n", 
	   __FILE__,__LINE__,fname); abort();
  }
  while(1){file >> str; if(strncmp(str,"n_tri:",6) == 0)break;}
  file >> n; cv::Mat tri(n,3,CV_32S);
  while(1){file >> c; if(c == '{')break;}
  for(i = 0; i < n; i++)
    file >> tri.at<int>(i,0) >> tri.at<int>(i,1) >> tri.at<int>(i,2);
  file.close(); return tri;
}
//===========================================================================
