/* ------------------------------------------------------------
name: "vocal_bp"
Code generated with Faust 2.1.4 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2014 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/

#ifndef __FaustPatch_h__
#define __FaustPatch_h__

#include <cstddef>
#include <string.h>
#include <strings.h>
#include "Patch.h"


#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__


/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /** Global init, calls the following methods :
         * - static class 'classInit' : static table initialisation
         * - 'instanceInit' : constants and instance table initialisation
         *
         * @param samplingRate - the sampling rate in Herz
         */
        virtual void init(int samplingRate) = 0;

        /** Init instance state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceInit(int samplingRate) = 0;

        /** Init instance constant state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceConstants(int samplingRate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value metadata).
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with sucessive in/out audio buffers.
         *
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the nomber of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = 0):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int samplingRate) { fDSP->init(samplingRate); }
        virtual void instanceInit(int samplingRate) { fDSP->instanceInit(samplingRate); }
        virtual void instanceConstants(int samplingRate) { fDSP->instanceConstants(samplingRate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { return fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

class UI
{

    public:

        UI() {}

        virtual ~UI() {}

        // -- widget's layouts

        virtual void openTabBox(const char* label) = 0;
        virtual void openHorizontalBox(const char* label) = 0;
        virtual void openVerticalBox(const char* label) = 0;
        virtual void closeBox() = 0;

        // -- active widgets

        virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

        // -- passive widgets

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

#endif



struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};



/**************************************************************************************

	OwlParameter : object used by OwlUI to ensures the connection between an owl parameter 
	and a faust widget
	
***************************************************************************************/

class OwlParameter
{
  protected:
	Patch* 	fPatch;		// needed to register and read owl parameters
	PatchParameterId	fParameter;		// OWL parameter code : PARAMETER_A,...
	FAUSTFLOAT* 		fZone;			// Faust widget zone
	const char*			fLabel;			// Faust widget label 
	float				fMin;			// Faust widget minimal value
	float				fSpan;			// Faust widget value span (max-min)
	
  public:
	OwlParameter() :
		fPatch(0), fParameter(PARAMETER_A), fZone(0), fLabel(""), fMin(0), fSpan(1) {}
	OwlParameter(const OwlParameter& w) :
		fPatch(w.fPatch), fParameter(w.fParameter), fZone(w.fZone), fLabel(w.fLabel), fMin(w.fMin), fSpan(w.fSpan) {}
	OwlParameter(Patch* pp, PatchParameterId param, FAUSTFLOAT* z, const char* l, float lo, float hi) :
		fPatch(pp), fParameter(param), fZone(z), fLabel(l), fMin(lo), fSpan(hi-lo) {}
	void bind() 	{ fPatch->registerParameter(fParameter, fLabel); }
	void update()	{ *fZone = fMin + fSpan*fPatch->getParameterValue(fParameter); }
	
};

class OwlButton
{
  protected:
	Patch* 	fPatch;		// needed to register and read owl parameters
	PatchButtonId	fButton;		// OWL button id : PUSHBUTTON, ...
	FAUSTFLOAT* 		fZone;			// Faust widget zone
	const char*			fLabel;			// Faust widget label 
  public:
	OwlButton() :
		fPatch(0), fButton(PUSHBUTTON), fZone(0), fLabel("") {}
	OwlButton(const OwlButton& w) :
		fPatch(w.fPatch), fButton(w.fButton), fZone(w.fZone), fLabel(w.fLabel) {}
	OwlButton(Patch* pp, PatchButtonId button, FAUSTFLOAT* z, const char* l) :
		fPatch(pp), fButton(button), fZone(z), fLabel(l) {}
	void bind() 	{  }
	void update()	{ *fZone = fPatch->isButtonPressed(fButton); }
};


/**************************************************************************************

	OwlUI : Faust User Interface builder. Passed to buildUserInterface OwlUI ensures
	the mapping between owl parameters and faust widgets. It relies on specific
	metadata "...[OWL:PARAMETER_X]..." in widget's label for that. For example any 
	faust widget with metadata [OWL:PARAMETER_B] will be controlled by PARAMETER_B 
	(the second knob).
	
***************************************************************************************/

// The maximun number of mappings between owl parameters and faust widgets 
#define MAXOWLPARAMETERS 40
#define MAXOWLBUTTONS    2
#define NO_PARAMETER     ((PatchParameterId)-1)
#define NO_BUTTON        ((PatchButtonId)-1)

class OwlUI : public UI
{
	Patch* 	fPatch;
	PatchParameterId	fParameter;					// current parameter ID, value NO_PARAMETER means not set
	int					fParameterIndex;						// number of OwlParameters collected so far
	OwlParameter			fParameterTable[MAXOWLPARAMETERS];		// kind of static list of OwlParameters
        PatchButtonId fButton;
        int fButtonIndex;
        OwlButton fButtonTable[MAXOWLBUTTONS];
	// check if the widget is an Owl parameter and, if so, add the corresponding OwlParameter
	void addOwlParameter(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
		if ((fParameter >= PARAMETER_A) && (fParameterIndex < MAXOWLPARAMETERS)) {
			fParameterTable[fParameterIndex] = OwlParameter(fPatch, fParameter, zone, label, lo, hi);
			fParameterTable[fParameterIndex].bind();
			fParameterIndex++;
		}
		fParameter = NO_PARAMETER; 		// clear current parameter ID
	}
	void addOwlButton(const char* label, FAUSTFLOAT* zone) {
		if ((fButton >= PUSHBUTTON) && (fButtonIndex < MAXOWLBUTTONS)) {
			fButtonTable[fButtonIndex] = OwlButton(fPatch, fButton, zone, label);
			fButtonTable[fButtonIndex].bind();
			fButtonIndex++;
		}
		fButton = NO_BUTTON; 		// clear current button ID
	}

	// we dont want to create a widget by-ut we clear the current parameter ID just in case
	void skip() {
		fParameter = NO_PARAMETER; 		// clear current parameter ID
		fButton = NO_BUTTON;
	}

 public:

        OwlUI(Patch* pp) : fPatch(pp), fParameter(NO_PARAMETER), fParameterIndex(0), fButton(NO_BUTTON), fButtonIndex(0) {}
	
	virtual ~OwlUI() {}
	
	// should be called before compute() to update widget's zones registered as Owl parameters
	void update() {
		for (int i=0; i<fParameterIndex; i++)  fParameterTable[i].update();
		for (int i=0; i<fButtonIndex; i++)  fButtonTable[i].update();
	}

	//---------------------- virtual methods called by buildUserInterface ----------------
	
    // -- widget's layouts

    virtual void openTabBox(const char* label) {}
    virtual void openHorizontalBox(const char* label) {}
    virtual void openVerticalBox(const char* label) {}
    virtual void closeBox() {}

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) 																			{ addOwlButton(label, zone); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 																		{ addOwlButton(label, zone); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlParameter(label, zone, lo, hi); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlParameter(label, zone, lo, hi); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 			{ addOwlParameter(label, zone, lo, hi); }

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }
    virtual void addVerticalBargraph  (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT* z, const char* k, const char* id) {
    	if (strcasecmp(k,"OWL") == 0) {
	  if(strncasecmp(id, "PARAMETER_", 10) == 0)
	    id += 10;
	  if (strcasecmp(id,"A") == 0)  fParameter = PARAMETER_A;
	  else if (strcasecmp(id,"B") == 0)  fParameter = PARAMETER_B;
	  else if (strcasecmp(id,"C") == 0)  fParameter = PARAMETER_C;
	  else if (strcasecmp(id,"D") == 0)  fParameter = PARAMETER_D;
	  else if (strcasecmp(id,"E") == 0)  fParameter = PARAMETER_E;
	  else if (strcasecmp(id,"F") == 0)  fParameter = PARAMETER_F;
	  else if (strcasecmp(id,"G") == 0)  fParameter = PARAMETER_G;
	  else if (strcasecmp(id,"H") == 0)  fParameter = PARAMETER_H;
	  else if (strcasecmp(id,"PUSH") == 0)  fButton = PUSHBUTTON;
	  else if (strcasecmp(id,"BYPASS") == 0)  fButton = BYPASS_BUTTON;
    	}
    }
};

#endif // __FaustCommonInfrastructure__

/**************************BEGIN USER SECTION **************************/


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

#include <math.h>

float mydsp_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	float fRec1[2];
	int fSamplingFreq;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fHslider2;
	float fRec2[2];
	int iRec4[2];
	float fConst2;
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	float fRec5[2];
	float fRec0[3];
	float fRec6[3];
	float fRec7[3];
	float fRec8[3];
	float fRec9[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.0");
		m->declare("name", "vocal_bp");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 0;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(192000.0f, max(1000.0f, float(fSamplingFreq)));
		fConst1 = (1.0f / fConst0);
		fConst2 = (3.14159274f / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.90000000000000002f);
		fHslider1 = FAUSTFLOAT(0.0f);
		fHslider2 = FAUSTFLOAT(0.0f);
		fHslider3 = FAUSTFLOAT(0.0f);
		fHslider4 = FAUSTFLOAT(0.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec1[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			iRec4[l2] = 0;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec5[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
			fRec0[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
			fRec6[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec7[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec8[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec9[l8] = 0.0f;
			
		}
		
	}
	
		virtual void init(int samplingFreq) {
			classInit(samplingFreq);
			instanceInit(samplingFreq);
		}
	virtual void instanceInit(int samplingFreq) {
		instanceConstants(samplingFreq);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSamplingFreq;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("vocal_bp");
		ui_interface->declare(&fHslider2, "OWL", "A");
		ui_interface->addHorizontalSlider("freq", &fHslider2, 0.0f, 0.0f, 5.0f, 0.00999999978f);
		ui_interface->declare(&fHslider1, "OWL", "D");
		ui_interface->addHorizontalSlider("fricative", &fHslider1, 0.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->declare(&fHslider0, "OWL", "E");
		ui_interface->addHorizontalSlider("gain", &fHslider0, 0.899999976f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->declare(&fHslider3, "OWL", "B");
		ui_interface->addHorizontalSlider("voice type", &fHslider3, 0.0f, 0.0f, 4.0f, 1.0f);
		ui_interface->declare(&fHslider4, "OWL", "C");
		ui_interface->addHorizontalSlider("vowel", &fHslider4, 0.0f, 0.0f, 4.0f, 0.00100000005f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = float(fHslider0);
		float fSlow1 = (0.00100000005f * float(fHslider1));
		float fSlow2 = powf(2.0f, float(fHslider2));
		float fSlow3 = (32.7029991f * fSlow2);
		float fSlow4 = max(1.00000001e-07f, fabsf(fSlow3));
		float fSlow5 = (fConst1 * fSlow4);
		float fSlow6 = (1.0f - (fConst0 / fSlow4));
		float fSlow7 = float(fHslider3);
		float fSlow8 = (5.0f * fSlow7);
		float fSlow9 = (0.00100000005f * float(fHslider4));
		float fSlow10 = (5.0f * (1.0f - fSlow7));
		int iSlow11 = (((fSlow7 == 0.0f)?1:((fSlow7 == 3.0f)?1:0)) == 0);
		float fSlow12 = ((0.000839999993f * (1000.0f - fSlow3)) + 0.800000012f);
		float fSlow13 = ((0.00366666657f * (400.0f - fSlow3)) + 3.0f);
		float fSlow14 = (iSlow11?fSlow13:fSlow12);
		int iSlow15 = (fSlow7 != 2.0f);
		int iSlow16 = (fSlow3 >= 200.0f);
		float fSlow17 = ((65.4059982f * fSlow2) + 30.0f);
		float fSlow18 = (0.000952380942f * (fSlow3 + -200.0f));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec1[0] = (fSlow1 + (0.999000013f * fRec1[1]));
			float fTemp0 = (fSlow5 + (fRec2[1] + -1.0f));
			int iTemp1 = (fTemp0 < 0.0f);
			float fTemp2 = (fSlow5 + fRec2[1]);
			fRec2[0] = (iTemp1?fTemp2:fTemp0);
			float fRec3 = (iTemp1?fTemp2:(fSlow5 + (fRec2[1] + (fSlow6 * fTemp0))));
			iRec4[0] = ((1103515245 * iRec4[1]) + 12345);
			float fTemp3 = (fSlow0 * (((1.0f - fRec1[0]) * ((2.0f * fRec3) + -1.0f)) + (4.65661287e-10f * (fRec1[0] * float(iRec4[0])))));
			fRec5[0] = (fSlow9 + (0.999000013f * fRec5[1]));
			float fTemp4 = (fSlow8 + fRec5[0]);
			int iTemp5 = (fTemp4 < 23.0f);
			int iTemp6 = (fTemp4 < 24.0f);
			float fTemp7 = (fSlow8 + (fRec5[0] + -23.0f));
			int iTemp8 = (fTemp4 < 22.0f);
			float fTemp9 = (fSlow8 + (fRec5[0] + -22.0f));
			int iTemp10 = (fTemp4 < 21.0f);
			float fTemp11 = (fSlow8 + (fRec5[0] + -21.0f));
			int iTemp12 = (fTemp4 < 20.0f);
			float fTemp13 = (fSlow8 + (fRec5[0] + -20.0f));
			int iTemp14 = (fTemp4 < 19.0f);
			float fTemp15 = (fSlow8 + (fRec5[0] + -19.0f));
			int iTemp16 = (fTemp4 < 18.0f);
			float fTemp17 = (fSlow8 + (fRec5[0] + -18.0f));
			int iTemp18 = (fTemp4 < 17.0f);
			float fTemp19 = (fSlow8 + (fRec5[0] + -17.0f));
			int iTemp20 = (fTemp4 < 16.0f);
			float fTemp21 = (fSlow8 + (fRec5[0] + -16.0f));
			float fTemp22 = (80.0f * fTemp21);
			int iTemp23 = (fTemp4 < 15.0f);
			float fTemp24 = (fSlow8 + (fRec5[0] + -15.0f));
			int iTemp25 = (fTemp4 < 14.0f);
			float fTemp26 = (fSlow8 + (fRec5[0] + -14.0f));
			int iTemp27 = (fTemp4 < 13.0f);
			float fTemp28 = (fSlow8 + (fRec5[0] + -13.0f));
			int iTemp29 = (fTemp4 < 12.0f);
			float fTemp30 = (fSlow8 + (fRec5[0] + -12.0f));
			int iTemp31 = (fTemp4 < 11.0f);
			float fTemp32 = (fSlow8 + (fRec5[0] + -11.0f));
			int iTemp33 = (fTemp4 < 10.0f);
			float fTemp34 = (fSlow8 + (fRec5[0] + -10.0f));
			int iTemp35 = (fTemp4 < 9.0f);
			float fTemp36 = (fSlow8 + (fRec5[0] + -9.0f));
			int iTemp37 = (fTemp4 < 8.0f);
			float fTemp38 = (fSlow8 + (fRec5[0] + -8.0f));
			float fTemp39 = (50.0f * fTemp38);
			int iTemp40 = (fTemp4 < 7.0f);
			float fTemp41 = (fSlow8 + (fRec5[0] + -7.0f));
			int iTemp42 = (fTemp4 < 6.0f);
			float fTemp43 = (fSlow8 + (fRec5[0] + -6.0f));
			int iTemp44 = (fTemp4 < 5.0f);
			float fTemp45 = (fRec5[0] - fSlow10);
			int iTemp46 = (fTemp4 < 4.0f);
			float fTemp47 = (fSlow8 + (fRec5[0] + -4.0f));
			int iTemp48 = (fTemp4 < 3.0f);
			float fTemp49 = (fSlow8 + (fRec5[0] + -3.0f));
			int iTemp50 = (fTemp4 < 2.0f);
			float fTemp51 = (fSlow8 + (fRec5[0] + -2.0f));
			int iTemp52 = (fTemp4 < 1.0f);
			float fTemp53 = (fSlow8 + (fRec5[0] + -1.0f));
			int iTemp54 = (fTemp4 < 0.0f);
			float fTemp55 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?800.0f:(iTemp52?(800.0f - (400.0f * fTemp4)):400.0f)):(iTemp50?(400.0f - (50.0f * fTemp53)):350.0f)):(iTemp48?((100.0f * fTemp51) + 350.0f):450.0f)):(iTemp46?(450.0f - (125.0f * fTemp49)):325.0f)):(iTemp44?((275.0f * fTemp47) + 325.0f):600.0f)):(iTemp42?(600.0f - (200.0f * fTemp45)):400.0f)):(iTemp40?(400.0f - (150.0f * fTemp43)):250.0f)):(iTemp37?((150.0f * fTemp41) + 250.0f):400.0f)):(iTemp35?(400.0f - fTemp39):350.0f)):(iTemp33?((310.0f * fTemp36) + 350.0f):660.0f)):(iTemp31?(660.0f - (220.0f * fTemp34)):440.0f)):(iTemp29?(440.0f - (170.0f * fTemp32)):270.0f)):(iTemp27?((160.0f * fTemp30) + 270.0f):430.0f)):(iTemp25?(430.0f - (60.0f * fTemp28)):370.0f)):(iTemp23?((430.0f * fTemp26) + 370.0f):800.0f)):(iTemp20?(800.0f - (450.0f * fTemp24)):350.0f)):(iTemp18?(350.0f - fTemp22):270.0f)):(iTemp16?((180.0f * fTemp19) + 270.0f):450.0f)):(iTemp14?(450.0f - (125.0f * fTemp17)):325.0f)):(iTemp12?(325.0f * (fTemp15 + 1.0f)):650.0f)):(iTemp10?(650.0f - (250.0f * fTemp13)):400.0f)):(iTemp8?(400.0f - (110.0f * fTemp11)):290.0f)):(iTemp5?((110.0f * fTemp9) + 290.0f):400.0f)):(iTemp6?(400.0f - (50.0f * fTemp7)):350.0f));
			float fTemp56 = ((fTemp55 <= fSlow3)?fSlow3:fTemp55);
			float fTemp57 = tanf((fConst2 * fTemp56));
			float fTemp58 = (1.0f / fTemp57);
			float fTemp59 = (20.0f * fTemp13);
			float fTemp60 = (20.0f * fTemp24);
			float fTemp61 = (10.0f * fTemp34);
			float fTemp62 = (10.0f * fTemp47);
			float fTemp63 = (20.0f * fTemp49);
			float fTemp64 = (20.0f * fTemp51);
			float fTemp65 = (20.0f * fTemp4);
			float fTemp66 = ((iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?80.0f:(iTemp52?(80.0f - fTemp65):60.0f)):(iTemp50?(60.0f - (10.0f * fTemp53)):50.0f)):(iTemp48?(fTemp64 + 50.0f):70.0f)):(iTemp46?(70.0f - fTemp63):50.0f)):(iTemp44?(fTemp62 + 50.0f):60.0f)):(iTemp42?(60.0f - (20.0f * fTemp45)):40.0f)):(iTemp40?((20.0f * fTemp43) + 40.0f):60.0f)):(iTemp37?(60.0f - (20.0f * fTemp41)):40.0f)):40.0f):(iTemp33?(40.0f * (fTemp36 + 1.0f)):80.0f)):(iTemp31?(80.0f - fTemp61):70.0f)):(iTemp29?(70.0f - (30.0f * fTemp32)):40.0f)):40.0f):40.0f):(iTemp23?(40.0f * (fTemp26 + 1.0f)):80.0f)):(iTemp20?(80.0f - fTemp60):60.0f)):60.0f):(iTemp16?(60.0f - (20.0f * fTemp19)):40.0f)):(iTemp14?((10.0f * fTemp17) + 40.0f):50.0f)):50.0f):(iTemp10?(fTemp59 + 50.0f):70.0f)):(iTemp8?(70.0f - (30.0f * fTemp11)):40.0f)):(iTemp5?((30.0f * fTemp9) + 40.0f):70.0f)):(iTemp6?(70.0f - (30.0f * fTemp7)):40.0f)) / fTemp56);
			float fTemp67 = (((fTemp58 + fTemp66) / fTemp57) + 1.0f);
			fRec0[0] = (fTemp3 - (((fRec0[2] * (((fTemp58 - fTemp66) / fTemp57) + 1.0f)) + (2.0f * (fRec0[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp57)))))) / fTemp67));
			float fTemp68 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?1150.0f:(iTemp52?((450.0f * fTemp4) + 1150.0f):1600.0f)):(iTemp50?((100.0f * fTemp53) + 1600.0f):1700.0f)):(iTemp48?(1700.0f - (900.0f * fTemp51)):800.0f)):(iTemp46?(800.0f - (100.0f * fTemp49)):700.0f)):(iTemp44?((340.0f * fTemp47) + 700.0f):1040.0f)):(iTemp42?((580.0f * fTemp45) + 1040.0f):1620.0f)):(iTemp40?((130.0f * fTemp43) + 1620.0f):1750.0f)):(iTemp37?(1750.0f - (1000.0f * fTemp41)):750.0f)):(iTemp35?(750.0f - (150.0f * fTemp38)):600.0f)):(iTemp33?((520.0f * fTemp36) + 600.0f):1120.0f)):(iTemp31?((680.0f * fTemp34) + 1120.0f):1800.0f)):(iTemp29?((50.0f * fTemp32) + 1800.0f):1850.0f)):(iTemp27?(1850.0f - (1030.0f * fTemp30)):820.0f)):(iTemp25?(820.0f - (190.0f * fTemp28)):630.0f)):(iTemp23?((520.0f * fTemp26) + 630.0f):1150.0f)):(iTemp20?((850.0f * fTemp24) + 1150.0f):2000.0f)):(iTemp18?((140.0f * fTemp21) + 2000.0f):2140.0f)):(iTemp16?(2140.0f - (1340.0f * fTemp19)):800.0f)):(iTemp14?(800.0f - (100.0f * fTemp17)):700.0f)):(iTemp12?((380.0f * fTemp15) + 700.0f):1080.0f)):(iTemp10?((620.0f * fTemp13) + 1080.0f):1700.0f)):(iTemp8?((170.0f * fTemp11) + 1700.0f):1870.0f)):(iTemp5?(1870.0f - (1070.0f * fTemp9)):800.0f)):(iTemp6?(800.0f - (200.0f * fTemp7)):600.0f));
			float fTemp69 = (iSlow15?(((fTemp68 >= 1300.0f) & iSlow16)?(fTemp68 - (fSlow18 * (fTemp68 + -1300.0f))):((fTemp68 <= fSlow17)?fSlow17:fTemp68)):fTemp68);
			float fTemp70 = tanf((fConst2 * fTemp69));
			float fTemp71 = (1.0f / fTemp70);
			float fTemp72 = (10.0f * fTemp9);
			float fTemp73 = (10.0f * fTemp13);
			float fTemp74 = (10.0f * fTemp36);
			float fTemp75 = (10.0f * fTemp45);
			float fTemp76 = ((iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?90.0f:(iTemp52?(90.0f - (10.0f * fTemp4)):80.0f)):(iTemp50?((20.0f * fTemp53) + 80.0f):100.0f)):(iTemp48?(100.0f - fTemp64):80.0f)):(iTemp46?(80.0f - fTemp63):60.0f)):(iTemp44?(fTemp62 + 60.0f):70.0f)):(iTemp42?(fTemp75 + 70.0f):80.0f)):(iTemp40?((10.0f * fTemp43) + 80.0f):90.0f)):(iTemp37?(90.0f - (10.0f * fTemp41)):80.0f)):80.0f):(iTemp33?(fTemp74 + 80.0f):90.0f)):(iTemp31?(90.0f - fTemp61):80.0f)):(iTemp29?((10.0f * fTemp32) + 80.0f):90.0f)):(iTemp27?(90.0f - (10.0f * fTemp30)):80.0f)):(iTemp25?(80.0f - (20.0f * fTemp28)):60.0f)):(iTemp23?((30.0f * fTemp26) + 60.0f):90.0f)):(iTemp20?((10.0f * fTemp24) + 90.0f):100.0f)):(iTemp18?(100.0f - (10.0f * fTemp21)):90.0f)):(iTemp16?(90.0f - (10.0f * fTemp19)):80.0f)):(iTemp14?(80.0f - (20.0f * fTemp17)):60.0f)):(iTemp12?((30.0f * fTemp15) + 60.0f):90.0f)):(iTemp10?(90.0f - fTemp73):80.0f)):(iTemp8?((10.0f * fTemp11) + 80.0f):90.0f)):(iTemp5?(90.0f - fTemp72):80.0f)):(iTemp6?(80.0f - (20.0f * fTemp7)):60.0f)) / fTemp69);
			float fTemp77 = (((fTemp71 + fTemp76) / fTemp70) + 1.0f);
			fRec6[0] = (fTemp3 - (((fRec6[2] * (((fTemp71 - fTemp76) / fTemp70) + 1.0f)) + (2.0f * (fRec6[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp70)))))) / fTemp77));
			float fTemp78 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?0.630957007f:(iTemp52?(0.630957007f - (0.567861021f * fTemp4)):0.0630960017f)):(iTemp50?((0.0369039997f * fTemp53) + 0.0630960017f):0.100000001f)):(iTemp48?((0.254812986f * fTemp51) + 0.100000001f):0.35481301f)):(iTemp46?(0.35481301f - (0.103624001f * fTemp49)):0.251188993f)):(iTemp44?((0.195494995f * fTemp47) + 0.251188993f):0.446684003f)):(iTemp42?(0.446684003f - (0.195494995f * fTemp45)):0.251188993f)):(iTemp40?(0.251188993f - (0.219566002f * fTemp43)):0.0316229984f)):(iTemp37?((0.250214994f * fTemp41) + 0.0316229984f):0.281838f)):(iTemp35?(0.281838f - (0.181838006f * fTemp38)):0.100000001f)):(iTemp33?((0.401187003f * fTemp36) + 0.100000001f):0.501187027f)):(iTemp31?(0.501187027f - (0.301661015f * fTemp34)):0.199525997f)):(iTemp29?(0.199525997f - (0.136429995f * fTemp32)):0.0630960017f)):(iTemp27?((0.253131986f * fTemp30) + 0.0630960017f):0.316228002f)):(iTemp25?(0.316228002f - (0.216227993f * fTemp28)):0.100000001f)):(iTemp23?((0.401187003f * fTemp26) + 0.100000001f):0.501187027f)):(iTemp20?(0.501187027f - (0.401187003f * fTemp24)):0.100000001f)):(iTemp18?((0.151188999f * fTemp21) + 0.100000001f):0.251188993f)):(iTemp16?((0.0306490008f * fTemp19) + 0.251188993f):0.281838f)):(iTemp14?(0.281838f - (0.123349003f * fTemp17)):0.158489004f)):(iTemp12?((0.342698008f * fTemp15) + 0.158489004f):0.501187027f)):(iTemp10?(0.501187027f - (0.301661015f * fTemp13)):0.199525997f)):(iTemp8?(0.199525997f - (0.0216979999f * fTemp11)):0.177827999f)):(iTemp5?((0.138400003f * fTemp9) + 0.177827999f):0.316228002f)):(iTemp6?(0.316228002f - (0.216227993f * fTemp7)):0.100000001f));
			float fTemp79 = (iSlow11?(fSlow13 * fTemp78):(fSlow12 * fTemp78));
			float fTemp80 = (100.0f * fTemp7);
			float fTemp81 = (50.0f * fTemp15);
			float fTemp82 = (50.0f * fTemp34);
			float fTemp83 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?2800.0f:(iTemp52?(2800.0f - (100.0f * fTemp4)):2700.0f)):2700.0f):(iTemp48?((130.0f * fTemp51) + 2700.0f):2830.0f)):(iTemp46?(2830.0f - (300.0f * fTemp49)):2530.0f)):(iTemp44?(2530.0f - (280.0f * fTemp47)):2250.0f)):(iTemp42?((150.0f * fTemp45) + 2250.0f):2400.0f)):(iTemp40?((200.0f * fTemp43) + 2400.0f):2600.0f)):(iTemp37?(2600.0f - (200.0f * fTemp41)):2400.0f)):2400.0f):(iTemp33?((350.0f * fTemp36) + 2400.0f):2750.0f)):(iTemp31?(2750.0f - fTemp82):2700.0f)):(iTemp29?((200.0f * fTemp32) + 2700.0f):2900.0f)):(iTemp27?(2900.0f - (200.0f * fTemp30)):2700.0f)):(iTemp25?((50.0f * fTemp28) + 2700.0f):2750.0f)):(iTemp23?((150.0f * fTemp26) + 2750.0f):2900.0f)):(iTemp20?(2900.0f - (100.0f * fTemp24)):2800.0f)):(iTemp18?((150.0f * fTemp21) + 2800.0f):2950.0f)):(iTemp16?(2950.0f - (120.0f * fTemp19)):2830.0f)):(iTemp14?(2830.0f - (130.0f * fTemp17)):2700.0f)):(iTemp12?(2700.0f - fTemp81):2650.0f)):(iTemp10?(2650.0f - (50.0f * fTemp13)):2600.0f)):(iTemp8?((200.0f * fTemp11) + 2600.0f):2800.0f)):(iTemp5?(2800.0f - (200.0f * fTemp9)):2600.0f)):(iTemp6?(fTemp80 + 2600.0f):2700.0f));
			float fTemp84 = tanf((fConst2 * fTemp83));
			float fTemp85 = (1.0f / fTemp84);
			float fTemp86 = (20.0f * fTemp26);
			float fTemp87 = (20.0f * fTemp34);
			float fTemp88 = (20.0f * fTemp36);
			float fTemp89 = (60.0f * fTemp47);
			float fTemp90 = ((iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?120.0f:(iTemp48?(120.0f - fTemp64):100.0f)):(iTemp46?((70.0f * fTemp49) + 100.0f):170.0f)):(iTemp44?(170.0f - fTemp89):110.0f)):(iTemp42?(110.0f - fTemp75):100.0f)):100.0f):100.0f):100.0f):(iTemp33?(fTemp88 + 100.0f):120.0f)):(iTemp31?(120.0f - fTemp87):100.0f)):100.0f):100.0f):100.0f):(iTemp23?(fTemp86 + 100.0f):120.0f)):120.0f):(iTemp18?(120.0f - (20.0f * fTemp21)):100.0f)):100.0f):(iTemp14?((70.0f * fTemp17) + 100.0f):170.0f)):(iTemp12?(170.0f - fTemp81):120.0f)):(iTemp10?(120.0f - fTemp59):100.0f)):100.0f):100.0f):100.0f) / fTemp83);
			float fTemp91 = (((fTemp85 + fTemp90) / fTemp84) + 1.0f);
			fRec7[0] = (fTemp3 - (((fRec7[2] * (((fTemp85 - fTemp90) / fTemp84) + 1.0f)) + (2.0f * (fRec7[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp84)))))) / fTemp91));
			float fTemp92 = (iTemp16?((0.0293140002f * fTemp19) + 0.0501190014f):0.0794330016f);
			float fTemp93 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?0.100000001f:(iTemp52?(0.100000001f - (0.0683770031f * fTemp4)):0.0316229984f)):0.0316229984f):(iTemp48?((0.126865998f * fTemp51) + 0.0316229984f):0.158489004f)):(iTemp46?(0.158489004f - (0.126865998f * fTemp49)):0.0316229984f)):(iTemp44?((0.323190004f * fTemp47) + 0.0316229984f):0.35481301f)):0.35481301f):(iTemp40?(0.35481301f - (0.196324006f * fTemp43)):0.158489004f)):(iTemp37?(0.158489004f - (0.0693639964f * fTemp41)):0.0891249999f)):(iTemp35?(0.0891249999f - (0.0640060008f * fTemp38)):0.0251189992f)):(iTemp33?((0.0456760004f * fTemp36) + 0.0251189992f):0.0707949996f)):(iTemp31?((0.0550980009f * fTemp34) + 0.0707949996f):0.125892997f)):(iTemp29?(0.125892997f - (0.0627970025f * fTemp32)):0.0630960017f)):(iTemp27?(0.0630960017f - (0.0129770003f * fTemp30)):0.0501190014f)):(iTemp25?((0.020676f * fTemp28) + 0.0501190014f):0.0707949996f)):(iTemp23?(0.0707949996f - (0.0456760004f * fTemp26)):0.0251189992f)):(iTemp20?((0.152709007f * fTemp24) + 0.0251189992f):0.177827999f)):(iTemp18?(0.177827999f - (0.127709001f * fTemp21)):0.0501190014f)):fTemp92):(iTemp14?(0.0794330016f - (0.0616500005f * fTemp17)):0.0177829992f)):(iTemp12?((0.428900987f * fTemp15) + 0.0177829992f):0.446684003f)):(iTemp10?(0.446684003f - (0.195494995f * fTemp13)):0.251188993f)):(iTemp8?(0.251188993f - (0.125295997f * fTemp11)):0.125892997f)):(iTemp5?((0.125295997f * fTemp9) + 0.125892997f):0.251188993f)):(iTemp6?(0.251188993f - (0.109935001f * fTemp7)):0.141253993f));
			float fTemp94 = (iSlow11?(fSlow13 * fTemp93):(fSlow12 * fTemp93));
			float fTemp95 = (350.0f * fTemp45);
			float fTemp96 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?3500.0f:(iTemp52?(3500.0f - (200.0f * fTemp4)):3300.0f)):(iTemp50?((400.0f * fTemp53) + 3300.0f):3700.0f)):(iTemp48?(3700.0f - (200.0f * fTemp51)):3500.0f)):3500.0f):(iTemp44?(3500.0f - (1050.0f * fTemp47)):2450.0f)):(iTemp42?(fTemp95 + 2450.0f):2800.0f)):(iTemp40?((250.0f * fTemp43) + 2800.0f):3050.0f)):(iTemp37?(3050.0f - (450.0f * fTemp41)):2600.0f)):(iTemp35?((75.0f * fTemp38) + 2600.0f):2675.0f)):(iTemp33?((325.0f * fTemp36) + 2675.0f):3000.0f)):3000.0f):(iTemp29?((350.0f * fTemp32) + 3000.0f):3350.0f)):(iTemp27?(3350.0f - (350.0f * fTemp30)):3000.0f)):3000.0f):(iTemp23?((900.0f * fTemp26) + 3000.0f):3900.0f)):(iTemp20?(3900.0f - (300.0f * fTemp24)):3600.0f)):(iTemp18?((300.0f * fTemp21) + 3600.0f):3900.0f)):(iTemp16?(3900.0f - (100.0f * fTemp19)):3800.0f)):3800.0f):(iTemp12?(3800.0f - (900.0f * fTemp15)):2900.0f)):(iTemp10?((300.0f * fTemp13) + 2900.0f):3200.0f)):(iTemp8?((50.0f * fTemp11) + 3200.0f):3250.0f)):(iTemp5?(3250.0f - (450.0f * fTemp9)):2800.0f)):(iTemp6?(fTemp80 + 2800.0f):2900.0f));
			float fTemp97 = tanf((fConst2 * fTemp96));
			float fTemp98 = (1.0f / fTemp97);
			float fTemp99 = ((iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?130.0f:(iTemp52?(fTemp65 + 130.0f):150.0f)):150.0f):(iTemp48?(150.0f - fTemp64):130.0f)):(iTemp46?((50.0f * fTemp49) + 130.0f):180.0f)):(iTemp44?(180.0f - fTemp89):120.0f)):120.0f):120.0f):120.0f):120.0f):(iTemp33?(fTemp74 + 120.0f):130.0f)):(iTemp31?(130.0f - fTemp61):120.0f)):120.0f):120.0f):120.0f):(iTemp23?((10.0f * fTemp26) + 120.0f):130.0f)):(iTemp20?(fTemp60 + 130.0f):150.0f)):(iTemp18?(150.0f - (30.0f * fTemp21)):120.0f)):120.0f):(iTemp14?((60.0f * fTemp17) + 120.0f):180.0f)):(iTemp12?(180.0f - fTemp81):130.0f)):(iTemp10?(130.0f - fTemp73):120.0f)):120.0f):(iTemp5?(fTemp72 + 120.0f):130.0f)):(iTemp6?(130.0f - (10.0f * fTemp7)):120.0f)) / fTemp96);
			float fTemp100 = (((fTemp98 + fTemp99) / fTemp97) + 1.0f);
			fRec8[0] = (fTemp3 - (((fRec8[2] * (((fTemp98 - fTemp99) / fTemp97) + 1.0f)) + (2.0f * (fRec8[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp97)))))) / fTemp100));
			float fTemp101 = (iTemp29?(0.100000001f - (0.0841509998f * fTemp32)):0.0158489998f);
			float fTemp102 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?0.0158489998f:(iTemp52?((0.00193400006f * fTemp4) + 0.0158489998f):0.0177829992f)):(iTemp50?(0.0177829992f - (0.00193400006f * fTemp53)):0.0158489998f)):(iTemp48?((0.0239620004f * fTemp51) + 0.0158489998f):0.0398110002f)):(iTemp46?(0.0398110002f - (0.0298110005f * fTemp49)):0.00999999978f)):(iTemp44?((0.344812989f * fTemp47) + 0.00999999978f):0.35481301f)):(iTemp42?(0.35481301f - (0.103624001f * fTemp45)):0.251188993f)):(iTemp40?(0.251188993f - (0.171755999f * fTemp43)):0.0794330016f)):(iTemp37?((0.0205669999f * fTemp41) + 0.0794330016f):0.100000001f)):(iTemp35?(0.100000001f - (0.0601890013f * fTemp38)):0.0398110002f)):(iTemp33?((0.0232849997f * fTemp36) + 0.0398110002f):0.0630960017f)):(iTemp31?((0.0369039997f * fTemp34) + 0.0630960017f):0.100000001f)):fTemp101):(iTemp27?((0.0635839999f * fTemp30) + 0.0158489998f):0.0794330016f)):(iTemp25?(0.0794330016f - (0.0478099994f * fTemp28)):0.0316229984f)):(iTemp23?((0.0683770031f * fTemp26) + 0.0316229984f):0.100000001f)):(iTemp20?(0.100000001f - (0.0900000036f * fTemp24)):0.00999999978f)):(iTemp18?((0.0401189998f * fTemp21) + 0.00999999978f):0.0501190014f)):fTemp92):(iTemp14?(0.0794330016f - (0.0694330037f * fTemp17)):0.00999999978f)):(iTemp12?((0.388107002f * fTemp15) + 0.00999999978f):0.398106992f)):(iTemp10?(0.398106992f - (0.198580995f * fTemp13)):0.199525997f)):(iTemp8?(0.199525997f - (0.099526003f * fTemp11)):0.100000001f)):(iTemp5?((0.151188999f * fTemp9) + 0.100000001f):0.251188993f)):(iTemp6?(0.251188993f - (0.0516630001f * fTemp7)):0.199525997f));
			float fTemp103 = (iSlow11?(fSlow13 * fTemp102):(fSlow12 * fTemp102));
			float fTemp104 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?4950.0f:(iTemp44?(4950.0f - (2200.0f * fTemp47)):2750.0f)):(iTemp42?(fTemp95 + 2750.0f):3100.0f)):(iTemp40?((240.0f * fTemp43) + 3100.0f):3340.0f)):(iTemp37?(3340.0f - (440.0f * fTemp41)):2900.0f)):(iTemp35?(fTemp39 + 2900.0f):2950.0f)):(iTemp33?((400.0f * fTemp36) + 2950.0f):3350.0f)):(iTemp31?(3350.0f - fTemp82):3300.0f)):(iTemp29?((290.0f * fTemp32) + 3300.0f):3590.0f)):(iTemp27?(3590.0f - (290.0f * fTemp30)):3300.0f)):(iTemp25?((100.0f * fTemp28) + 3300.0f):3400.0f)):(iTemp23?((1550.0f * fTemp26) + 3400.0f):4950.0f)):4950.0f):4950.0f):4950.0f):4950.0f):(iTemp12?(4950.0f - (1700.0f * fTemp15)):3250.0f)):(iTemp10?((330.0f * fTemp13) + 3250.0f):3580.0f)):(iTemp8?(3580.0f - (40.0f * fTemp11)):3540.0f)):(iTemp5?(3540.0f - (540.0f * fTemp9)):3000.0f)):(iTemp6?((300.0f * fTemp7) + 3000.0f):3300.0f));
			float fTemp105 = tanf((fConst2 * fTemp104));
			float fTemp106 = (1.0f / fTemp105);
			float fTemp107 = ((iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?(iTemp52?(iTemp54?140.0f:(iTemp52?((60.0f * fTemp4) + 140.0f):200.0f)):200.0f):(iTemp48?(200.0f - (65.0f * fTemp51)):135.0f)):(iTemp46?((65.0f * fTemp49) + 135.0f):200.0f)):(iTemp44?(200.0f - (70.0f * fTemp47)):130.0f)):(iTemp42?(130.0f - fTemp75):120.0f)):120.0f):120.0f):120.0f):(iTemp33?(fTemp88 + 120.0f):140.0f)):(iTemp31?(140.0f - fTemp87):120.0f)):120.0f):120.0f):120.0f):(iTemp23?(fTemp86 + 120.0f):140.0f)):(iTemp20?((60.0f * fTemp24) + 140.0f):200.0f)):(iTemp18?(200.0f - fTemp22):120.0f)):120.0f):(iTemp14?((80.0f * fTemp17) + 120.0f):200.0f)):(iTemp12?(200.0f - (60.0f * fTemp15)):140.0f)):(iTemp10?(140.0f - fTemp59):120.0f)):120.0f):(iTemp5?((15.0f * fTemp9) + 120.0f):135.0f)):(iTemp6?(135.0f - (15.0f * fTemp7)):120.0f)) / fTemp104);
			float fTemp108 = (((fTemp106 + fTemp107) / fTemp105) + 1.0f);
			fRec9[0] = (fTemp3 - (((fRec9[2] * (((fTemp106 - fTemp107) / fTemp105) + 1.0f)) + (2.0f * (fRec9[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp105)))))) / fTemp108));
			float fTemp109 = (iTemp5?(iTemp8?(iTemp10?(iTemp12?(iTemp14?(iTemp16?(iTemp18?(iTemp20?(iTemp23?(iTemp25?(iTemp27?(iTemp29?(iTemp31?(iTemp33?(iTemp35?(iTemp37?(iTemp40?(iTemp42?(iTemp44?(iTemp46?(iTemp48?(iTemp50?0.00100000005f:(iTemp48?((0.000777999987f * fTemp51) + 0.00100000005f):0.00177800003f)):(iTemp46?(0.00177800003f - (0.001147f * fTemp49)):0.000630999974f)):(iTemp44?((0.0993689969f * fTemp47) + 0.000630999974f):0.100000001f)):(iTemp42?((0.0258930009f * fTemp45) + 0.100000001f):0.125892997f)):(iTemp40?(0.125892997f - (0.0860819966f * fTemp43)):0.0398110002f)):(iTemp37?(0.0398110002f - (0.0298110005f * fTemp41)):0.00999999978f)):(iTemp35?((0.00584900007f * fTemp38) + 0.00999999978f):0.0158489998f)):(iTemp33?(0.0158489998f - (0.00325999991f * fTemp36)):0.0125890002f)):(iTemp31?((0.0874110013f * fTemp34) + 0.0125890002f):0.100000001f)):fTemp101):(iTemp27?((0.00410400005f * fTemp30) + 0.0158489998f):0.0199529994f)):0.0199529994f):(iTemp23?(0.0199529994f - (0.0167909991f * fTemp26)):0.0031620001f)):(iTemp20?(0.0031620001f - (0.00157700002f * fTemp24)):0.00158499996f)):(iTemp18?((0.00472499989f * fTemp21) + 0.00158499996f):0.00631000008f)):(iTemp16?(0.00631000008f - (0.00314799999f * fTemp19)):0.0031620001f)):(iTemp14?(0.0031620001f - (0.00216199993f * fTemp17)):0.00100000005f)):(iTemp12?((0.0784329996f * fTemp15) + 0.00100000005f):0.0794330016f)):(iTemp10?((0.0205669999f * fTemp13) + 0.0794330016f):0.100000001f)):(iTemp8?(0.100000001f - (0.0683770031f * fTemp11)):0.0316229984f)):(iTemp5?((0.0184959993f * fTemp9) + 0.0316229984f):0.0501190014f)):0.0501190014f);
			float fTemp110 = (iSlow11?(fSlow13 * fTemp109):(fSlow12 * fTemp109));
			float fTemp111 = (((((((fRec0[2] * (0.0f - (fSlow14 / fTemp57))) + ((fRec0[0] * fSlow14) / fTemp57)) / fTemp67) + (((fRec6[2] * (0.0f - (fTemp79 / fTemp70))) + ((fRec6[0] * fTemp79) / fTemp70)) / fTemp77)) + (((fRec7[2] * (0.0f - (fTemp94 / fTemp84))) + ((fRec7[0] * fTemp94) / fTemp84)) / fTemp91)) + (((fRec8[2] * (0.0f - (fTemp103 / fTemp97))) + ((fRec8[0] * fTemp103) / fTemp97)) / fTemp100)) + (((fRec9[2] * (0.0f - (fTemp110 / fTemp105))) + ((fRec9[0] * fTemp110) / fTemp105)) / fTemp108));
			output0[i] = FAUSTFLOAT(fTemp111);
			output1[i] = FAUSTFLOAT(fTemp111);
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			iRec4[1] = iRec4[0];
			fRec5[1] = fRec5[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	FaustPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class FaustPatch : public Patch
{
    mydsp   fDSP;
    OwlUI	fUI;
    
public:

    FaustPatch() : fUI(this)
    {
        fDSP.init(int(getSampleRate()));		// Init Faust code with the OWL sampling rate
        fDSP.buildUserInterface(&fUI);			// Maps owl parameters and faust widgets 
    }
    
    void processAudio(AudioBuffer &buffer)
    {
        // Reasonably assume we will not have more than 32 channels
        float*  ins[32];
        float*  outs[32];
        int     n = buffer.getChannels();
        
        if ( (fDSP.getNumInputs() < 32) && (fDSP.getNumOutputs() < 32) ) {
            
            // create the table of input channels
            for(int ch=0; ch<fDSP.getNumInputs(); ++ch) {
                ins[ch] = buffer.getSamples(ch%n);
            }
            
            // create the table of output channels
            for(int ch=0; ch<fDSP.getNumOutputs(); ++ch) {
                outs[ch] = buffer.getSamples(ch%n);
            }
            
            // read OWL parameters and updates corresponding Faust Widgets zones
            fUI.update(); 
            
            // Process the audio samples
            fDSP.compute(buffer.getSize(), ins, outs);
        }
    }

};

extern "C" {
  void doSetButton(uint8_t id, uint16_t state, uint16_t samples);
  int owl_pushbutton(int value){
    static bool state = 0;
    static uint16_t counter = 0;
    value = (bool)value;
    if(state != value){
      state = value;
      doSetButton(PUSHBUTTON, state, counter);
    }
    if(++counter > getProgramVector()->audio_blocksize)
      counter = 0;
    return value;
  }
}

#endif // __FaustPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
