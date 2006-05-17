/* Copyright (c) 2006 The University of Notre Dame. All Rights Reserved.
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
 *
 *
 *  sphericalLattice.cpp
 *
 *  Created by Charles F. Vardeman II on 17 Feb 2006.
 *  @author  Charles F. Vardeman II
 *  @version $Id: shapedLattice.cpp,v 1.4 2006-05-17 21:51:42 tim Exp $
 *
 */


#include "lattice/shapedLattice.hpp"
#include "math/Vector3.hpp"
#include "brains/Register.hpp"
#include "lattice/LatticeFactory.hpp"

namespace oopse{
	shapedLattice::shapedLattice(RealType latticeConstant, 
                               std::string latticeType) {
		latticeConstant_ = latticeConstant;
		latticeType_ = latticeType;
		registerLattice();
		simpleLattice_ = LatticeFactory::getInstance()->createLattice(latticeType);
		if (simpleLattice_ == NULL){
			std::cerr << "shapedLattice:: Error creating lattice" << std::endl;
			exit(1);
		}
		
		//Set the lattice constant
    std::vector<RealType> lc;
    lc.push_back(latticeConstant_);
		simpleLattice_->setLatticeConstant(lc);
	}
	
  
  
  void shapedLattice::setGridDimension(Vector3d dimension){
    dimension_ = dimension;
		// Find	number of unit cells in each direction
		beginNx_ = -(int) ceil(0.5*dimension_[0]/latticeConstant_) ;
		beginNy_ = -(int) ceil(0.5*dimension_[1]/latticeConstant_) ;
		beginNz_ = -(int) ceil(0.5*dimension_[2]/latticeConstant_) ;    
    endNx_ = (int) ceil(0.5*dimension_[0]/latticeConstant_);
		endNy_ = (int) ceil(0.5*dimension_[1]/latticeConstant_);
		endNz_ = (int) ceil(0.5*dimension_[2]/latticeConstant_);    
  }
  
  
  
    std::vector<Vector3d> shapedLattice::getPoints(){
			std::vector<Vector3d> latticePos;
			
      
      std::vector<Vector3d> pointsOrt =  simpleLattice_->getLatticePointsOrt();
      int numMolPerCell = simpleLattice_->getNumSitesPerCell();	
      for(int i = beginNx_; i < endNx_; i++) {     
        for(int j = beginNy_; j < endNy_; j++) {       
          for(int k = beginNz_; k < endNz_; k++) {
            //get the position of the cell sites
            simpleLattice_->getLatticePointsPos(latticePos, i, j, k);
            
            for(int l = 0; l < numMolPerCell; l++) {
              
              
              if (isInterior(latticePos[l])){
              	Vector3d myPoint = latticePos[l];
                Vector3d myOrt = pointsOrt[l];
                coords_.push_back(myPoint);
                coordsOrt_.push_back(myOrt);
              }
            }
          }
        }
      }
      
      return coords_;
      
		}	
}
