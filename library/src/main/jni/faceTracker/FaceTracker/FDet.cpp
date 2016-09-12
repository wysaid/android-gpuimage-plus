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
#include <FaceTracker/FDet.h>
using namespace FACETRACKER;
using namespace std;
//===========================================================================
FDet::~FDet()
{  
  if(storage_ != NULL)cvReleaseMemStorage(&storage_);
  if(_cascade != NULL)cvReleaseHaarClassifierCascade(&_cascade);
}
//===========================================================================
FDet& FDet::operator= (FDet const& rhs)
{
  this->_min_neighbours = rhs._min_neighbours;
  this->_min_size = rhs._min_size;
  this->_img_scale = rhs._img_scale;
  this->_scale_factor = rhs._scale_factor;
  if(storage_ != NULL)cvReleaseMemStorage(&storage_);
  storage_ = cvCreateMemStorage(0);
  this->_cascade = rhs._cascade;
  this->small_img_ = rhs.small_img_.clone(); return *this;
}
//===========================================================================
void FDet::Init(const char* fname,
		const double img_scale,
		const double scale_factor,
		const int    min_neighbours,
		const int    min_size)
{
  if(!(_cascade = (CvHaarClassifierCascade*)cvLoad(fname,0,0,0))){
    printf("ERROR(%s,%d) : Failed loading classifier cascade!\n",
	   __FILE__,__LINE__); abort();
  }
  storage_        = cvCreateMemStorage(0);
  _img_scale      = img_scale;
  _scale_factor   = scale_factor;
  _min_neighbours = min_neighbours;
  _min_size       = min_size; return;
}
//===========================================================================
cv::Rect FDet::Detect(cv::Mat im)
{
  assert(im.type() == CV_8U);
  cv::Mat gray; int i,maxv; cv::Rect R;
  int w = cvRound(im.cols/_img_scale);
  int h = cvRound(im.rows/_img_scale);
  if((small_img_.rows!=h) || (small_img_.cols!=w))small_img_.create(h,w,CV_8U);
  if(im.channels() == 1)gray = im;
  else{gray=cv::Mat(im.rows,im.cols,CV_8U);cv::cvtColor(im,gray,CV_BGR2GRAY);}
  cv::resize(gray,small_img_,cv::Size(w,h),0,0,CV_INTER_LINEAR);
  cv::equalizeHist(small_img_,small_img_);
  cvClearMemStorage(storage_); IplImage simg = small_img_;
  CvSeq* obj = cvHaarDetectObjects(&simg,_cascade,storage_,
				   _scale_factor,_min_neighbours,0,
				   cv::Size(_min_size,_min_size));
  if(obj->total == 0)return cv::Rect(0,0,0,0);
  for(i = 0,maxv = 0; i < obj->total; i++){
    CvRect* r = (CvRect*)cvGetSeqElem(obj,i);
    if(i == 0 || maxv < r->width*r->height){
      maxv = r->width*r->height; R.x = r->x*_img_scale; R.y = r->y*_img_scale;
      R.width  = r->width*_img_scale; R.height = r->height*_img_scale;
    }
  }
  cvRelease((void**)(&obj)); return R;
}
//===========================================================================
void FDet::Load(const char* fname)
{
  ifstream s(fname); assert(s.is_open()); this->Read(s); s.close(); return;
}
//===========================================================================
void FDet::Save(const char* fname)
{
  ofstream s(fname); assert(s.is_open()); this->Write(s);s.close(); return;
}
//===========================================================================
void FDet::Write(ofstream &s)
{
  int i,j,k,l;
  s << IO::FDET                          << " "
    << _min_neighbours                   << " " 
    << _min_size                         << " "
    << _img_scale                        << " "
    << _scale_factor                     << " "
    << _cascade->count                   << " "
    << _cascade->orig_window_size.width  << " " 
    << _cascade->orig_window_size.height << " ";
  for(i = 0; i < _cascade->count; i++){
    s << _cascade->stage_classifier[i].parent    << " "
      << _cascade->stage_classifier[i].next      << " "
      << _cascade->stage_classifier[i].child     << " "
      << _cascade->stage_classifier[i].threshold << " "
      << _cascade->stage_classifier[i].count     << " "; 
    for(j = 0; j < _cascade->stage_classifier[i].count; j++){
      CvHaarClassifier* classifier = 
	&_cascade->stage_classifier[i].classifier[j];
      s << classifier->count << " ";
      for(k = 0; k < classifier->count; k++){
	s << classifier->threshold[k]           << " "
	  << classifier->left[k]                << " "
	  << classifier->right[k]               << " "
	  << classifier->alpha[k]               << " "
	  << classifier->haar_feature[k].tilted << " ";
	for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	  s << classifier->haar_feature[k].rect[l].weight   << " "
	    << classifier->haar_feature[k].rect[l].r.x      << " "
	    << classifier->haar_feature[k].rect[l].r.y      << " "
	    << classifier->haar_feature[k].rect[l].r.width  << " "
	    << classifier->haar_feature[k].rect[l].r.height << " ";
	}
      }
      s << classifier->alpha[classifier->count] << " ";
    }
  }return;
}
//===========================================================================
void FDet::Read(istream &s,bool readType)
{ 
  int i,j,k,l,n,m;
  if(readType){int type; s >> type; assert(type == IO::FDET);}
  s >> _min_neighbours >> _min_size >> _img_scale >> _scale_factor >> n;
  m = sizeof(CvHaarClassifierCascade)+n*sizeof(CvHaarStageClassifier);
  storage_ = cvCreateMemStorage(0);
  _cascade = (CvHaarClassifierCascade*)cvAlloc(m);
  memset(_cascade,0,m);
  _cascade->stage_classifier = (CvHaarStageClassifier*)(_cascade + 1);
  _cascade->flags = CV_HAAR_MAGIC_VAL;
  _cascade->count = n;
  s >> _cascade->orig_window_size.width >> _cascade->orig_window_size.height;
  for(i = 0; i < n; i++){
    s >> _cascade->stage_classifier[i].parent
      >> _cascade->stage_classifier[i].next
      >> _cascade->stage_classifier[i].child
      >> _cascade->stage_classifier[i].threshold
      >> _cascade->stage_classifier[i].count;
    _cascade->stage_classifier[i].classifier =
      (CvHaarClassifier*)cvAlloc(_cascade->stage_classifier[i].count*
				 sizeof(CvHaarClassifier));    
    for(j = 0; j < _cascade->stage_classifier[i].count; j++){
      CvHaarClassifier* classifier = 
	&_cascade->stage_classifier[i].classifier[j];
      s >> classifier->count;
      classifier->haar_feature = (CvHaarFeature*) 
	cvAlloc(classifier->count*(sizeof(CvHaarFeature) +
				   sizeof(float) + sizeof(int) + sizeof(int))+ 
		(classifier->count+1)*sizeof(float));
      classifier->threshold = 
	(float*)(classifier->haar_feature+classifier->count);
      classifier->left = (int*)(classifier->threshold + classifier->count);
      classifier->right = (int*)(classifier->left + classifier->count);
      classifier->alpha = (float*)(classifier->right + classifier->count);
      for(k = 0; k < classifier->count; k++){
	s >> classifier->threshold[k]
	  >> classifier->left[k]
	  >> classifier->right[k]
	  >> classifier->alpha[k]
	  >> classifier->haar_feature[k].tilted;
	for(l = 0; l < CV_HAAR_FEATURE_MAX; l++){
	  s >> classifier->haar_feature[k].rect[l].weight
	    >> classifier->haar_feature[k].rect[l].r.x
	    >> classifier->haar_feature[k].rect[l].r.y
	    >> classifier->haar_feature[k].rect[l].r.width
	    >> classifier->haar_feature[k].rect[l].r.height;
	}
      }
      s >> classifier->alpha[classifier->count];
    }
  }return;
}
//===========================================================================
