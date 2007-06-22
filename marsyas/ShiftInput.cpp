/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class ShiftInput
	\ingroup Basic
    \brief Shift next D samples  

    Shift next D samples from the input source to the output source 
    into the right-hand end of the outputSlice. 
    with gain and put them in the output vector. 

	Controls:
	- \b mrs_natural/Interpolation [rw] : dunno
	- \b mrs_natural/WindowSize [rw] : dunno
	- \b mrs_natural/Decimation [rw] : dunno
*/

#include "ShiftInput.h"

using namespace std;
using namespace Marsyas;

ShiftInput::ShiftInput(string name):MarSystem("ShiftInput",name)
{

  PW_ = 0;
  W_ = 0;
  N_ = 0;
  D_ = 0;
  reset_ = false;
  addControls();
}

ShiftInput::~ShiftInput()
{
}

ShiftInput::ShiftInput(const ShiftInput& a):MarSystem(a)
{
  PW_ = 0;
  W_ = 0;
  N_ = 0;
  D_ = 0;
  reset_ = false;
  
  ctrl_reset_ = getctrl("mrs_bool/reset");
  
}


MarSystem* 
ShiftInput::clone() const 
{
  return new ShiftInput(*this);
}




void
ShiftInput::addControls()
{
  addctrl("mrs_natural/Decimation", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES/2);
  addctrl("mrs_natural/WindowSize", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/WindowSize", true);

  addctrl("mrs_bool/reset", true, ctrl_reset_);
  setctrlState("mrs_bool/reset", true);
}

void
ShiftInput::myUpdate(MarControlPtr sender)
{
  reset_ = getctrl("mrs_bool/reset")->toBool();  

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/WindowSize"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  
  setctrl("mrs_natural/Decimation", getctrl("mrs_natural/inSamples"));

  W_ = getctrl("mrs_natural/WindowSize")->toNatural();

  if (PW_ != W_) 
    pout_.stretch(W_);

  PW_ = W_;  
  N_ = getctrl("mrs_natural/onSamples")->toNatural();
  D_ = getctrl("mrs_natural/inSamples")->toNatural();
}

void 
ShiftInput::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  if (reset_) 
    {
      pout_.setval(0.0);
      reset_ = false;
      ctrl_reset_->setValue(false, NOUPDATE);
    }

  for (t = 0; t < N_-D_; t++)
    out(t) = pout_(t+D_);
  for (t=N_-D_; t < N_; t++)
    {
      out(t) = in(t-(N_-D_));
    }
  for (t = 0; t < N_; t++) 
    pout_(t) = out(t);

	/*MATLAB_PUT(in, "ShiftInput_in");
	MATLAB_PUT(out, "ShiftInput_out");
	MATLAB_EVAL("plot([zeros(1,length(ShiftInput_in)) ShiftInput_in]); hold on; plot(ShiftInput_out, 'r'); hold off");*/
}









