#include "brains/DataStorageTestCase.hpp"
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DataStorageTestCase );

void DataStorageTestCase::testDataStorage() {
    //test constructor
    DataStorage ds1(10020);
    CPPUNIT_ASSERT_EQUAL(ds1.getStorageLayout(), 0x11111111);
    CPPUNIT_ASSERT_EQUAL(ds1.getSize(), 10020);

    //test resize
    ds1.resize(56);
    CPPUNIT_ASSERT_EQUAL(ds1.getSize(), 56);

    //test reserve
    
    DataStorage ds2(10000, DataStorage::dslForce |DataStorage::dslAmat |DataStorage::dslZAngle);

    CPPUNIT_ASSERT(!(ds2.getStorageLayout() & DataStorage::dslPosition));
    CPPUNIT_ASSERT(!(ds2.getStorageLayout() & DataStorage::dslVelocity));
    CPPUNIT_ASSERT(ds2.getStorageLayout() & DataStorage::dslAmat);
    CPPUNIT_ASSERT(!(ds2.getStorageLayout() & DataStorage::dslAngularMomentum));
    CPPUNIT_ASSERT(!(ds2.getStorageLayout() & DataStorage::dslUnitVector));
    CPPUNIT_ASSERT(ds2.getStorageLayout() & DataStorage::dslZAngle);    
    CPPUNIT_ASSERT(ds2.getStorageLayout() & DataStorage::dslForce);
    CPPUNIT_ASSERT(!(ds2.getStorageLayout() & DataStorage::dslTorque));


    CPPUNIT_ASSERT(ds2.position.size() == 0);
    CPPUNIT_ASSERT(ds2.velocity.size() == 0);
    CPPUNIT_ASSERT(ds2.aMat.size() == 10000);
    CPPUNIT_ASSERT(ds2.angularMomentum.size() == 0);
    CPPUNIT_ASSERT(ds2.unitVector.size() == 0);
    CPPUNIT_ASSERT(ds2.zAngle.size() == 10000);
    CPPUNIT_ASSERT(ds2.force.size() == 10000);
    CPPUNIT_ASSERT(ds2.torque.size() == 0);
    
    //test getArrayPointer

    double* pamat = ds2.getArrayPointer(DataStorage::dslAmat);
    double* pzangle = ds2.getArrayPointer(DataStorage::dslZAngle);
    double* pforce = ds2.getArrayPointer(DataStorage::dslForce);

    pamat[9] = 2.0993;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[1](0,0), 2.0993, OpenMD::NumericConstant::epsilon);

    pzangle[9743] = 8464353.129574;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.zAngle[9743], 8464353.129574, OpenMD::NumericConstant::epsilon);

    pforce[12] = 345546.3413263;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[4][0], 345546.3413263, OpenMD::NumericConstant::epsilon);

    //test copy
    ds2.zAngle[0] = 7.0;
    ds2.zAngle[1] = 3.5;

    ds2.force[0][0]= 6;
    ds2.force[0][1]= 7.0;
    ds2.force[0][2]= 8.0;
    
    ds2.force[1][0]= 9.0;
    ds2.force[1][1]= 10.0;
    ds2.force[1][2]= 11.0;
    
    ds2.aMat[0](0,0) = 1.0;
    ds2.aMat[0](0,1) = 2.0;
    ds2.aMat[0](0,2) = 3.0;
    ds2.aMat[0](1,0) = 4.0;
    ds2.aMat[0](1,1) = 5.0;
    ds2.aMat[0](1,2) = 6.0;
    ds2.aMat[0](2,0) = 7.0;
    ds2.aMat[0](2,1) = 8.0;
    ds2.aMat[0](2,2) = 9.0;

    ds2.aMat[1](0,0) = 11.0;
    ds2.aMat[1](0,1) = 12.0;
    ds2.aMat[1](0,2) = 13.0;
    ds2.aMat[1](1,0) = 14.0;
    ds2.aMat[1](1,1) = 15.0;
    ds2.aMat[1](1,2) = 16.0;
    ds2.aMat[1](2,0) = 17.0;
    ds2.aMat[1](2,1) = 18.0;
    ds2.aMat[1](2,2) = 19.0;

    ds2.copy(0, 2, 10);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.zAngle[10], 7.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.zAngle[11], 3.5, OpenMD::NumericConstant::epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[10][0], 6.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[10][1], 7.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[10][2], 8.0, OpenMD::NumericConstant::epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[11][0], 9.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[11][1], 10.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.force[11][2], 11.0, OpenMD::NumericConstant::epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](0, 0), 1.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](0, 1), 2.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](0, 2), 3.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](1, 0), 4.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](1, 1) , 5.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](1, 2), 6.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](2, 0), 7.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](2, 1), 8.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[10](2, 2), 9.0, OpenMD::NumericConstant::epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](0, 0), 11.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](0, 1), 12.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](0, 2), 13.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](1, 0), 14.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](1, 1) , 15.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](1, 2), 16.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](2, 0), 17.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](2, 1), 18.0, OpenMD::NumericConstant::epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ds2.aMat[11](2, 2), 19.0, OpenMD::NumericConstant::epsilon);

    
}

