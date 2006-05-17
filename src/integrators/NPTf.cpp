/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 */
 
#include "brains/SimInfo.hpp"
#include "brains/Thermo.hpp"
#include "integrators/IntegratorCreator.hpp"
#include "integrators/NPTf.hpp"
#include "primitives/Molecule.hpp"
#include "utils/OOPSEConstant.hpp"
#include "utils/simError.h"

namespace oopse {

  // Basic non-isotropic thermostating and barostating via the Melchionna
  // modification of the Hoover algorithm:
  //
  //    Melchionna, S., Ciccotti, G., and Holian, B. L., 1993,
  //       Molec. Phys., 78, 533.
  //
  //           and
  //
  //    Hoover, W. G., 1986, Phys. Rev. A, 34, 2499.

  void NPTf::evolveEtaA() {

    int i, j;

    for(i = 0; i < 3; i ++){
      for(j = 0; j < 3; j++){
	if( i == j) {
	  eta(i, j) += dt2 *  instaVol * (press(i, j) - targetPressure/OOPSEConstant::pressureConvert) / (NkBT*tb2);
	} else {
	  eta(i, j) += dt2 * instaVol * press(i, j) / (NkBT*tb2);
	}
      }
    }
  
    for(i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        oldEta(i, j) = eta(i, j);
      }
    }
  
  }

  void NPTf::evolveEtaB() {

    int i;
    int j;

    for(i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
	prevEta(i, j) = eta(i, j);
      }
    }

    for(i = 0; i < 3; i ++){
      for(j = 0; j < 3; j++){
	if( i == j) {
	  eta(i, j) = oldEta(i, j) + dt2 *  instaVol *
	    (press(i, j) - targetPressure/OOPSEConstant::pressureConvert) / (NkBT*tb2);
	} else {
	  eta(i, j) = oldEta(i, j) + dt2 * instaVol * press(i, j) / (NkBT*tb2);
	}
      }
    }

  
  }

  void NPTf::calcVelScale(){

    for (int i = 0; i < 3; i++ ) {
      for (int j = 0; j < 3; j++ ) {
	vScale(i, j) = eta(i, j);

	if (i == j) {
	  vScale(i, j) += chi;
	}
      }
    }
  }

  void NPTf::getVelScaleA(Vector3d& sc, const Vector3d& vel){
    sc = vScale * vel;
  }

  void NPTf::getVelScaleB(Vector3d& sc, int index ) {
    sc = vScale * oldVel[index];
  }

  void NPTf::getPosScale(const Vector3d& pos, const Vector3d& COM, int index, Vector3d& sc) {

    /**@todo */
    Vector3d rj = (oldPos[index] + pos)/(RealType)2.0 -COM;
    sc = eta * rj;
  }

  void NPTf::scaleSimBox(){

    int i;
    int j;
    int k;
    Mat3x3d scaleMat;
    RealType eta2ij;
    RealType bigScale, smallScale, offDiagMax;
    Mat3x3d hm;
    Mat3x3d hmnew;



    // Scale the box after all the positions have been moved:

    // Use a taylor expansion for eta products:  Hmat = Hmat . exp(dt * etaMat)
    //  Hmat = Hmat . ( Ident + dt * etaMat  + dt^2 * etaMat*etaMat / 2)

    bigScale = 1.0;
    smallScale = 1.0;
    offDiagMax = 0.0;

    for(i=0; i<3; i++){
      for(j=0; j<3; j++){

	// Calculate the matrix Product of the eta array (we only need
	// the ij element right now):

	eta2ij = 0.0;
	for(k=0; k<3; k++){
	  eta2ij += eta(i, k) * eta(k, j);
	}

	scaleMat(i, j) = 0.0;
	// identity matrix (see above):
	if (i == j) scaleMat(i, j) = 1.0;
	// Taylor expansion for the exponential truncated at second order:
	scaleMat(i, j) += dt*eta(i, j)  + 0.5*dt*dt*eta2ij;
      

	if (i != j)
	  if (fabs(scaleMat(i, j)) > offDiagMax)
	    offDiagMax = fabs(scaleMat(i, j));
      }

      if (scaleMat(i, i) > bigScale) bigScale = scaleMat(i, i);
      if (scaleMat(i, i) < smallScale) smallScale = scaleMat(i, i);
    }

    if ((bigScale > 1.01) || (smallScale < 0.99)) {
      sprintf( painCave.errMsg,
	       "NPTf error: Attempting a Box scaling of more than 1 percent.\n"
	       " Check your tauBarostat, as it is probably too small!\n\n"
	       " scaleMat = [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "      eta = [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n",
	       scaleMat(0, 0),scaleMat(0, 1),scaleMat(0, 2),
	       scaleMat(1, 0),scaleMat(1, 1),scaleMat(1, 2),
	       scaleMat(2, 0),scaleMat(2, 1),scaleMat(2, 2),
	       eta(0, 0),eta(0, 1),eta(0, 2),
	       eta(1, 0),eta(1, 1),eta(1, 2),
	       eta(2, 0),eta(2, 1),eta(2, 2));
      painCave.isFatal = 1;
      simError();
    } else if (offDiagMax > 0.01) {
      sprintf( painCave.errMsg,
	       "NPTf error: Attempting an off-diagonal Box scaling of more than 1 percent.\n"
	       " Check your tauBarostat, as it is probably too small!\n\n"
	       " scaleMat = [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "      eta = [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n"
	       "            [%lf\t%lf\t%lf]\n",
	       scaleMat(0, 0),scaleMat(0, 1),scaleMat(0, 2),
	       scaleMat(1, 0),scaleMat(1, 1),scaleMat(1, 2),
	       scaleMat(2, 0),scaleMat(2, 1),scaleMat(2, 2),
	       eta(0, 0),eta(0, 1),eta(0, 2),
	       eta(1, 0),eta(1, 1),eta(1, 2),
	       eta(2, 0),eta(2, 1),eta(2, 2));
      painCave.isFatal = 1;
      simError();
    } else {

      Mat3x3d hmat = currentSnapshot_->getHmat();
      hmat = hmat *scaleMat;
      currentSnapshot_->setHmat(hmat);
        
    }
  }

  bool NPTf::etaConverged() {
    int i;
    RealType diffEta, sumEta;

    sumEta = 0;
    for(i = 0; i < 3; i++) {
      sumEta += pow(prevEta(i, i) - eta(i, i), 2);
    }
    
    diffEta = sqrt( sumEta / 3.0 );

    return ( diffEta <= etaTolerance );
  }

  RealType NPTf::calcConservedQuantity(){

    chi= currentSnapshot_->getChi();
    integralOfChidt = currentSnapshot_->getIntegralOfChiDt();
    loadEta();
    
    // We need NkBT a lot, so just set it here: This is the RAW number
    // of integrableObjects, so no subtraction or addition of constraints or
    // orientational degrees of freedom:
    NkBT = info_->getNGlobalIntegrableObjects()*OOPSEConstant::kB *targetTemp;

    // fkBT is used because the thermostat operates on more degrees of freedom
    // than the barostat (when there are particles with orientational degrees
    // of freedom).  
    fkBT = info_->getNdf()*OOPSEConstant::kB *targetTemp;    
    
    RealType conservedQuantity;
    RealType totalEnergy;
    RealType thermostat_kinetic;
    RealType thermostat_potential;
    RealType barostat_kinetic;
    RealType barostat_potential;
    RealType trEta;

    totalEnergy = thermo.getTotalE();

    thermostat_kinetic = fkBT * tt2 * chi * chi /(2.0 * OOPSEConstant::energyConvert);

    thermostat_potential = fkBT* integralOfChidt / OOPSEConstant::energyConvert;

    SquareMatrix<RealType, 3> tmp = eta.transpose() * eta;
    trEta = tmp.trace();
    
    barostat_kinetic = NkBT * tb2 * trEta /(2.0 * OOPSEConstant::energyConvert);

    barostat_potential = (targetPressure * thermo.getVolume() / OOPSEConstant::pressureConvert) /OOPSEConstant::energyConvert;

    conservedQuantity = totalEnergy + thermostat_kinetic + thermostat_potential +
      barostat_kinetic + barostat_potential;

    return conservedQuantity;

  }

  void NPTf::loadEta() {
    eta= currentSnapshot_->getEta();

    //if (!eta.isDiagonal()) {
    //    sprintf( painCave.errMsg,
    //             "NPTf error: the diagonal elements of eta matrix are not the same or etaMat is not a diagonal matrix");
    //    painCave.isFatal = 1;
    //    simError();
    //}
  }

  void NPTf::saveEta() {
    currentSnapshot_->setEta(eta);
  }

}
