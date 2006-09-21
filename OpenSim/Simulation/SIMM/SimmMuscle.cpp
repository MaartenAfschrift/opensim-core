// SimmMuscle.cpp
// Author: Peter Loan
/* Copyright (c) 2005, Stanford University and Peter Loan.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


//=============================================================================
// INCLUDES
//=============================================================================
#include "SimmMuscle.h"
#include "SimmMuscleGroup.h"
#include "SimmMuscleViaPoint.h"
#include "SimmModel.h"
#include "SimmKinematicsEngine.h"

//=============================================================================
// STATICS
//=============================================================================


using namespace OpenSim;
using namespace std;

//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Default constructor.
 */
SimmMuscle::SimmMuscle() :
   Actuator(0, 0, 0),
	_attachmentSetProp(PropertyObj("", SimmMusclePointSet())),
	_attachmentSet((SimmMusclePointSet&)_attachmentSetProp.getValueObj()),
	_maxIsometricForce(_maxIsometricForceProp.getValueDbl()),
	_optimalFiberLength(_optimalFiberLengthProp.getValueDbl()),
	_tendonSlackLength(_tendonSlackLengthProp.getValueDbl()),
	_pennationAngle(_pennationAngleProp.getValueDbl()),
	_maxContractionVelocity(_maxContractionVelocityProp.getValueDbl()),
	_tendonForceLengthCurve((ArrayPtrs<Function>&)_tendonForceLengthCurveProp.getValueObjArray()),
	_activeForceLengthCurve((ArrayPtrs<Function>&)_activeForceLengthCurveProp.getValueObjArray()),
	_passiveForceLengthCurve((ArrayPtrs<Function>&)_passiveForceLengthCurveProp.getValueObjArray()),
	_forceVelocityCurve((ArrayPtrs<Function>&)_forceVelocityCurveProp.getValueObjArray()),
 	_displayerProp(PropertyObj("", VisibleObject())),
   _displayer((VisibleObject&)_displayerProp.getValueObj()),
	_groupNames(_groupNamesProp.getValueStrArray()),
	_groups(NULL),
	_preScaleLength(0.0),
	_kinematicsEngine(NULL)
{
	setNull();

}
//_____________________________________________________________________________
/**
 * Constructor from an XML node
 */
SimmMuscle::SimmMuscle(DOMElement *aElement) :
   Actuator(0, 0, 0, aElement),
	_attachmentSetProp(PropertyObj("", SimmMusclePointSet())),
	_attachmentSet((SimmMusclePointSet&)_attachmentSetProp.getValueObj()),
	_maxIsometricForce(_maxIsometricForceProp.getValueDbl()),
	_optimalFiberLength(_optimalFiberLengthProp.getValueDbl()),
	_tendonSlackLength(_tendonSlackLengthProp.getValueDbl()),
	_pennationAngle(_pennationAngleProp.getValueDbl()),
	_maxContractionVelocity(_maxContractionVelocityProp.getValueDbl()),
	_tendonForceLengthCurve((ArrayPtrs<Function>&)_tendonForceLengthCurveProp.getValueObjArray()),
	_activeForceLengthCurve((ArrayPtrs<Function>&)_activeForceLengthCurveProp.getValueObjArray()),
	_passiveForceLengthCurve((ArrayPtrs<Function>&)_passiveForceLengthCurveProp.getValueObjArray()),
	_forceVelocityCurve((ArrayPtrs<Function>&)_forceVelocityCurveProp.getValueObjArray()),
	_displayerProp(PropertyObj("", VisibleObject())),
   _displayer((VisibleObject&)_displayerProp.getValueObj()),
	_groupNames(_groupNamesProp.getValueStrArray()),
	_groups(NULL),
	_preScaleLength(0.0),
	_kinematicsEngine(NULL)
{
	setNull();

	updateFromXMLNode();
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
SimmMuscle::~SimmMuscle()
{
}

//_____________________________________________________________________________
/**
 * Copy constructor.
 *
 * @param aMuscle SimmMuscle to be copied.
 */
SimmMuscle::SimmMuscle(const SimmMuscle &aMuscle) :
   Actuator(aMuscle),
	_attachmentSetProp(PropertyObj("", SimmMusclePointSet())),
	_attachmentSet((SimmMusclePointSet&)_attachmentSetProp.getValueObj()),
	_maxIsometricForce(_maxIsometricForceProp.getValueDbl()),
	_optimalFiberLength(_optimalFiberLengthProp.getValueDbl()),
	_tendonSlackLength(_tendonSlackLengthProp.getValueDbl()),
	_pennationAngle(_pennationAngleProp.getValueDbl()),
	_maxContractionVelocity(_maxContractionVelocityProp.getValueDbl()),
	_tendonForceLengthCurve((ArrayPtrs<Function>&)_tendonForceLengthCurveProp.getValueObjArray()),
	_activeForceLengthCurve((ArrayPtrs<Function>&)_activeForceLengthCurveProp.getValueObjArray()),
	_passiveForceLengthCurve((ArrayPtrs<Function>&)_passiveForceLengthCurveProp.getValueObjArray()),
	_forceVelocityCurve((ArrayPtrs<Function>&)_forceVelocityCurveProp.getValueObjArray()),
	_displayerProp(PropertyObj("", VisibleObject())),
   _displayer((VisibleObject&)_displayerProp.getValueObj()),
	_groupNames(_groupNamesProp.getValueStrArray()),
	_groups(NULL),
	_preScaleLength(0.0),
	_kinematicsEngine(NULL)
{
	setupProperties();
	copyData(aMuscle);
}
//_____________________________________________________________________________
/**
 * Copy this muscle point and return a pointer to the copy.
 * The copy constructor for this class is used.
 *
 * @return Pointer to a copy of this SimmMuscle.
 */
Object* SimmMuscle::copy() const
{
	SimmMuscle *musc = new SimmMuscle(*this);
	return(musc);
}
//_____________________________________________________________________________
/**
 * Copy this SimmMuscle and modify the copy so that it is consistent
 * with a specified XML element node.
 *
 * The copy is constructed by first using
 * SimmMuscle::SimmMuscle(DOMElement*) in order to establish the
 * relationship of the SimmMuscle object with the XML node. Then, the
 * assignment operator is used to set all data members of the copy to the
 * values of this SimmMuscle object. Finally, the data members of the copy are
 * updated using SimmMuscle::updateFromXMLNode().
 *
 * @param aElement XML element. 
 * @return Pointer to a copy of this SimmMuscle.
 */
Object* SimmMuscle::copy(DOMElement *aElement) const
{
	SimmMuscle *pt = new SimmMuscle(aElement);
	*pt = *this;
	pt->updateFromXMLNode();
	return(pt);
}

void SimmMuscle::copyData(const SimmMuscle &aMuscle)
{
	_attachmentSet = aMuscle.getAttachmentSet();
	_maxIsometricForce = aMuscle._maxIsometricForce;
	_optimalFiberLength = aMuscle._optimalFiberLength;
	_tendonSlackLength = aMuscle._tendonSlackLength;
	_pennationAngle = aMuscle._pennationAngle;
	_maxContractionVelocity = aMuscle._maxContractionVelocity;
	_tendonForceLengthCurve = aMuscle._tendonForceLengthCurve;
	_activeForceLengthCurve = aMuscle._activeForceLengthCurve;
	_passiveForceLengthCurve = aMuscle._passiveForceLengthCurve;
	_forceVelocityCurve = aMuscle._forceVelocityCurve;
	_groupNames = aMuscle._groupNames;
	_groups = aMuscle._groups;
	_displayer = aMuscle._displayer;
	_preScaleLength = 0.0;
	_kinematicsEngine = aMuscle._kinematicsEngine;
}


//=============================================================================
// CONSTRUCTION
//=============================================================================
//_____________________________________________________________________________
/**
 * Set the data members of this SimmMuscle to their null values.
 */
void SimmMuscle::setNull()
{
	setupProperties();
	setType("SimmMuscle");
	setName("");
}
//_____________________________________________________________________________
/**
 * Connect properties to local pointers.
 */
void SimmMuscle::setupProperties()
{
	_attachmentSetProp.setName("SimmMusclePointSet");
	_propertySet.append(&_attachmentSetProp);

	_maxIsometricForceProp.setName("max_isometric_force");
	_maxIsometricForceProp.setValue(0.0);
	_propertySet.append(&_maxIsometricForceProp);

	_optimalFiberLengthProp.setName("optimal_fiber_length");
	_optimalFiberLengthProp.setValue(0.0);
	_propertySet.append(&_optimalFiberLengthProp);

	_tendonSlackLengthProp.setName("tendon_slack_length");
	_tendonSlackLengthProp.setValue(0.0);
	_propertySet.append(&_tendonSlackLengthProp);

	_pennationAngleProp.setName("pennation_angle");
	_pennationAngleProp.setValue(0.0);
	_propertySet.append(&_pennationAngleProp);

	_maxContractionVelocityProp.setName("max_contraction_velocity");
	_maxContractionVelocityProp.setValue(0.0);
	_propertySet.append(&_maxContractionVelocityProp);

	ArrayPtrs<Object> func;

	_tendonForceLengthCurveProp.setName("tendon_force_length_curves");
	_tendonForceLengthCurveProp.setValue(func);
	_propertySet.append(&_tendonForceLengthCurveProp);

	_activeForceLengthCurveProp.setName("active_force_length_curves");
	_activeForceLengthCurveProp.setValue(func);
	_propertySet.append(&_activeForceLengthCurveProp);

	_passiveForceLengthCurveProp.setName("passive_force_length_curves");
	_passiveForceLengthCurveProp.setValue(func);
	_propertySet.append(&_passiveForceLengthCurveProp);

	_forceVelocityCurveProp.setName("force_velocity_curves");
	_forceVelocityCurveProp.setValue(func);
	_propertySet.append(&_forceVelocityCurveProp);

	_displayerProp.setName("display");
	_propertySet.append(&_displayerProp);

	_groupNamesProp.setName("groups");
	_propertySet.append(&_groupNamesProp);
}

SimmMuscle& SimmMuscle::operator=(const SimmMuscle &aMuscle)
{
	// BASE CLASS
	Actuator::operator=(aMuscle);

	copyData(aMuscle);

	return(*this);
}

void SimmMuscle::registerTypes()
{
	Object::RegisterType(SimmMusclePoint());
	Object::RegisterType(SimmMuscleViaPoint());
}

void SimmMuscle::preScale(const ScaleSet& aScaleSet)
{
	_preScaleLength = getLength();
}

void SimmMuscle::scale(const ScaleSet& aScaleSet)
{
	for (int i = 0; i < _attachmentSet.getSize(); i++)
	{
		const string& bodyName = _attachmentSet.get(i)->getBodyName();
		for (int j = 0; j < aScaleSet.getSize(); j++)
		{
			Scale *aScale = aScaleSet.get(j);
			if (bodyName == aScale->getSegmentName())
			{
				Array<double> scaleFactors(1.0, 3);
				aScale->getScaleFactors(scaleFactors);
				_attachmentSet.get(i)->scale(scaleFactors);
			}
		}
	}
}

void SimmMuscle::postScale(const ScaleSet& aScaleSet)
{
	if (_preScaleLength > 0.0)
	{
		double scaleFactor = getLength() / _preScaleLength;

		_optimalFiberLength *= scaleFactor;
		_tendonSlackLength *= scaleFactor;
		//_maxIsometricForce *= scaleFactor;

		_preScaleLength = 0.0;
	}
}

/* Perform some set up functions that happen after the
 * object has been deserialized or copied.
 */
void SimmMuscle::setup(SimmModel* model, SimmKinematicsEngine* ke)
{
	int i;

	_kinematicsEngine = ke;

	for (i = 0; i < _attachmentSet.getSize(); i++)
		_attachmentSet.get(i)->setup(model, ke);

	_groups.setSize(0);
	for (i = 0; i < _groupNames.getSize(); i++)
		_groups.append(model->enterGroup(_groupNames[i]));
	_displayer.setOwner(this);
}

double SimmMuscle::getLength() const
{
	double length = 0.0;
	SimmMusclePoint *start, *end;

	//check wrapping??

   for (int i = 0; i < _attachmentSet.getSize() - 1; i++)
   {
		start = _attachmentSet.get(i);
      end = _attachmentSet.get(i+1);
#if 0
      /* If both points are wrap points on the same wrap object, then this muscle
       * segment wraps over the surface of a wrap object, so just add in the
       * pre-calculated distance.
       */
      if (dynamic_cast<SimmMuscleWrapPoint&>start &&
			 dynamic_cast<SimmMuscleWrapPoint&>end &&
			 start.getWrapObject() == end.getWrapObject())
      {
         length += end.getWrapDistance();
      }
      else
      {
         length += _kinematicsEngine->calcDistance(start->getAttachment(), start->getBody(), end->getAttachment(), end->getBody());
      }
#else
		length += _kinematicsEngine->calcDistance(start->getAttachment(), start->getBody(), end->getAttachment(), end->getBody());
#endif
   }

	return length;
}

void SimmMuscle::writeSIMM(ofstream& out) const
{
	out << "beginmuscle " << getName() << endl;

	out << "beginpoints" << endl;
	for (int i = 0; i < _attachmentSet.getSize(); i++)
		_attachmentSet.get(i)->writeSIMM(out);
	out << "endpoints" << endl;

	if (_groupNames.getSize() > 0)
	{
		out << "begingroups" << endl;
		for (int i = 0; i < _groupNames.getSize(); i++)
			out << " " << _groupNames[i];
		out << endl << "endgroups" << endl;
	}

	out << "max_force " << _maxIsometricForce << endl;
	out << "optimal_fiber_length " << _optimalFiberLength << endl;
	out << "tendon_slack_length " << _tendonSlackLength << endl;
	out << "pennation_angle " << _pennationAngle << endl;
	out << "max_contraction_velocity " << _maxContractionVelocity << endl;

	NatCubicSpline* ncs;

	if (_tendonForceLengthCurve.getSize() > 0)
		if (ncs = dynamic_cast<NatCubicSpline*>(_tendonForceLengthCurve[0]))
			ncs->writeSIMM(out, string("tendonforcelengthcurve"));

	if (_activeForceLengthCurve.getSize() > 0)
		if (ncs = dynamic_cast<NatCubicSpline*>(_activeForceLengthCurve[0]))
			ncs->writeSIMM(out, string("activeforcelengthcurve"));

	if (_passiveForceLengthCurve.getSize() > 0)
		if (ncs = dynamic_cast<NatCubicSpline*>(_passiveForceLengthCurve[0]))
			ncs->writeSIMM(out, string("passiveforcelengthcurve"));

	if (_forceVelocityCurve.getSize() > 0)
		if (ncs = dynamic_cast<NatCubicSpline*>(_forceVelocityCurve[0]))
			ncs->writeSIMM(out, string("forcevelocitycurve"));

	out << "endmuscle" << endl << endl;
}

void SimmMuscle::peteTest(SimmKinematicsEngine* ke) const
{
	int i;

	cout << "Muscle: " << getName() << endl;
	for (i = 0; i < _attachmentSet.getSize(); i++)
		_attachmentSet.get(i)->peteTest();
	cout << "   groups: ";
	for (i = 0; i < _groupNames.getSize(); i++)
		cout << _groupNames[i] << " ";
	cout << endl;
	cout << "   maxIsometricForce: " << _maxIsometricForce << endl;
	cout << "   optimalFiberLength: " << _optimalFiberLength << endl;
	cout << "   tendonSlackLength: " << _tendonSlackLength << endl;
	cout << "   pennationAngle: " << _pennationAngle << endl;
	cout << "   maxContractionVelocity: " << _maxContractionVelocity << endl;
	if (_tendonForceLengthCurve.getSize() > 0)
		cout << "   tendonForceLengthCurve: " << *(_tendonForceLengthCurve[0]) << endl;
	if (_activeForceLengthCurve.getSize() > 0)
		cout << "   activeForceLengthCurve: " << *(_activeForceLengthCurve[0]) << endl;
	if (_passiveForceLengthCurve.getSize() > 0)
		cout << "   passiveForceLengthCurve: " << *(_passiveForceLengthCurve[0]) << endl;
	if (_forceVelocityCurve.getSize() > 0)
		cout << "   forceVelocityCurve: " << *(_forceVelocityCurve[0]) << endl;
	cout << "   current length: " << getLength() << endl;
}
