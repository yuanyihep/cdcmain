//---------------------------------------------------------------------------
//      BOOST --- BESIII Object_Oriented Simulation Tool                     
//---------------------------------------------------------------------------/
//Description: Main part for construct MDC geometry 
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: 
//---------------------------------------------------------------------------/

#include "BesMdcConstruction.hh"
#include "ReadBoostRoot.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Hype.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4TwistedTubs.hh"
#include "G4SDManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4Geo/MdcG4Geo.h"

#include "MyMdcGeomSvc.hh"
#include "G4Cons.hh"

BesMdcConstruction::BesMdcConstruction()
 : mdc_log(0),mdc_phys(0){
   mdc=BesMdcGeoParameter::GetGeo();
}

BesMdcConstruction::~BesMdcConstruction(){
}
 
//**************************************************************************

void BesMdcConstruction::Construct(G4LogicalVolume* logicBes){
  //------------------------------------------------ 
  // Sensitive detectors
  //------------------------------------------------ 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String mdcSDname = "BesMdcSD";
  aTrackerSD = new BesMdcSD( mdcSDname);
  SDman->AddNewDetector( aTrackerSD );

  if(ReadBoostRoot::GetTuning())return; //No construct when tuning
  //Construct 
  G4LogicalVolume *logicalMdc = 0;
  G4LogicalVolume *lv         = 0;
  
  if(ReadBoostRoot::GetMdc()==2){
  }else {
    //construct with previous code. 
    if(ReadBoostRoot::GetMdc()==3)G4cout<<"Nowire in Mdc Construct"<<G4endl;   

    //--------- Materials  ---------
    G4double a, z, density,abundance,fractionmass;
    G4double temperature, pressure;
    G4int iz,n,nel,ncomponents,natoms;
    G4String name,symbol;

    G4Material* Vacuum = G4Material::GetMaterial("Galactic"); 

    G4Material* Al = G4Material::GetMaterial("Aluminium");
    G4Element *C  =G4Element::GetElement("Carbon");
    G4Element *H  =G4Element::GetElement("Hydrogen");
    G4Element *O  =G4Element::GetElement("Oxygen");

    // Mdcgas He/C3H8 (60:40)
    G4Material* MdcGas = G4Material::GetMaterial("Mdcgas");
    G4Material* He = G4Material::GetMaterial("Hegas");

    density = 0.002489*g/cm3;
    G4Material* Butance = new G4Material(name="Butance",density,nel=2);
    Butance->AddElement(C, natoms=4);
    Butance->AddElement(H, natoms=10);

    density = 0.000403321*g/cm3;
    G4Material* Cdcgas = new G4Material(name="Cdcgas",density,nel=2);
    Cdcgas->AddMaterial(He, fractionmass = 0.3713);
    Cdcgas->AddMaterial(Butance, fractionmass = 0.6287);

    density = 19.3*g/cm3;
    a = 183.84*g/mole;
    G4Material* W = new G4Material("Tungsten", 74., a, density);

    density=19.32*g/cm3;
    a = 196.967*g/mole;
    G4Material* Au= new G4Material("Au",79,a,density);

    density=1.57*g/cm3;
    G4int nElement=3;
    G4Material* CarbonFiber=new G4Material("CarbonFiber",density,nElement);
    CarbonFiber->AddElement(C,0.697);
    CarbonFiber->AddElement(H,0.0061);
    CarbonFiber->AddElement(O,0.2969);

    density = 1.42*g/cm3;
    G4Material* Kapton = new G4Material("Kapton", density, 3);
    Kapton->AddElement(O,2);
    Kapton->AddElement(C,5);
    Kapton->AddElement(H,4);

    //Begin construct
    G4int i,j,k;
    G4double outR, innerR, length;
    G4double startAngle, spanAngle;
    G4double posX, posY, posZ;
    G4VisAttributes* visAtt;
    //string name; 
    //=======Mdc container

    outR=800*mm;
    innerR=0*mm;
    length=751*mm; //In Geant4, soild has central-symmetry 

    startAngle=0.*deg;
    spanAngle=360.*deg;
    posX  = 0.*m;
    posY  = 0.*m;
    posZ  = 0.*m;

    G4Tubs* mdc_tube=new G4Tubs("solidCdc",innerR,outR,length,startAngle,spanAngle);
    mdc_log=new G4LogicalVolume(mdc_tube, Cdcgas,"logicalCdc",0,0,0);
    mdc_log->SetVisAttributes(G4VisAttributes::Invisible);

    //limits of step length
    //    G4double maxStep = 20.*mm;


    mdc_phys= new G4PVPlacement(0,
	G4ThreeVector(posX ,posY ,posZ),
	mdc_log,"physicalCdc",logicBes,false,0);


    //======Tube and Endplane

    outR=502*mm;
    innerR=501.6*mm;
    length=750*mm; 
    startAngle=0*deg;
    spanAngle=360*deg; 
    G4Tubs* innerWall_tube=new G4Tubs("solidInnerWall",innerR,outR,length,startAngle,spanAngle);
    G4LogicalVolume* innerWall_log=new G4LogicalVolume(innerWall_tube,CarbonFiber,"logicalInnerWall",0,0,0);
    G4PVPlacement* innerWall_phys=new G4PVPlacement(0,
	G4ThreeVector(posX ,posY ,posZ),
	innerWall_log,"physicalInnerWall",mdc_log,false,0);

    outR=501.6*mm;
    innerR=0*mm;
    length=750*mm;                            
    startAngle=0*deg;
    spanAngle=360*deg;
    G4Tubs* vacuum_tube=new G4Tubs("solidVacuum",innerR,outR,length,startAngle,spanAngle);
    G4LogicalVolume* vacuum_log=new G4LogicalVolume(vacuum_tube,Vacuum,"logicalVacuum",0,0,0);
    G4PVPlacement* vacuum_phys=new G4PVPlacement(0,
	G4ThreeVector(posX ,posY ,posZ),
	innerWall_log,"physicalVacuum",mdc_log,false,0);


    //========Layer  
    G4int signalLayer,firstWire;
    G4int replicaNo;
    G4double offset;

    G4double signalWireR=mdc->SignalWireR()*micrometer;
    G4double fieldWireR=mdc->FieldWireR()*micrometer;
    std::cout<<__FILE__<<"   "<<__LINE__<<" signalWireR  "<<signalWireR<<" fieldWireR "<<fieldWireR<<std::endl;
    //G4double signalWireR=15.*micrometer;
    //G4double fieldWireR=63.*micrometer;
    G4double thickOfAu=0.7*micrometer; // All wires are goldplated.

    G4double innerLength,innerTwistAngle,innerTan,midInnerR,innerStereo;
    G4double outLength,outTwistAngle,outTwistAngleFixed,outTan,midOutR,outStereo,outRFixed,outTanFixed; 

    //----Stereo layers 21-36

    for(i=17; i>-1; i--){
      signalLayer=mdc->Signal2Global(i);

      innerR=mdc->Layer(signalLayer-1).R()*mm-fieldWireR;
      outR=mdc->Layer(signalLayer+1).R()*mm-fieldWireR;
      if(i==17)outR=mdc->Layer(signalLayer+1).R()*mm+fieldWireR;

      innerLength=(mdc->Layer(signalLayer-1).Length())/2.*mm;
      outLength=(mdc->Layer(signalLayer+1).Length())/2.*mm;

      innerTwistAngle=mdc->Layer(signalLayer-1).RotateAngle()*rad;
      outTwistAngle=mdc->Layer(signalLayer+1).RotateAngle()*rad;

      innerTan=innerR/innerLength*sin(innerTwistAngle);

      midInnerR=innerR*cos(innerTwistAngle);
      innerStereo=atan(innerTan);

      outTan=outR/outLength*sin(outTwistAngle);
      outTwistAngleFixed=atan(innerLength/outLength*tan(outTwistAngle));

      if(abs(outTwistAngleFixed) >= abs(innerTwistAngle)){
	midOutR=outR*cos(outTwistAngle);
	outRFixed=midOutR/cos(innerTwistAngle);
	outR=outRFixed;

	outTanFixed=midOutR/innerLength*tan(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }else{	
	outRFixed=sqrt(outR*outR+(innerLength*innerLength-outLength*outLength)*outTan*outTan);
	outR=outRFixed;
	midOutR=outR*cos(innerTwistAngle);

	outTanFixed=outRFixed/innerLength*sin(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }
      //Layer
      std::ostringstream osnameLayerSolid;
      osnameLayerSolid <<"solid"<< "CdcStereoLayer"<<i;  

      G4Hype* stereoLayer_hype=new G4Hype(osnameLayerSolid.str(),midInnerR, midOutR,innerStereo,
	  outStereo,innerLength);

      std::ostringstream osnameLayerLogical;
      osnameLayerLogical <<"logical"<< "CdcStereoLayer"<<i;
      G4LogicalVolume* stereoLayer_log=new G4LogicalVolume(stereoLayer_hype,Cdcgas,
	  osnameLayerLogical.str(),0,0,0);
      stereoLayer_log->SetVisAttributes(G4VisAttributes::Invisible);

      replicaNo=mdc->Layer(signalLayer).WireNo()/2;
      spanAngle=360./replicaNo*deg;
      firstWire=mdc->Layer(signalLayer).FirstWire();

      G4RotationMatrix* layerRot=new G4RotationMatrix();
      layerRot->rotateZ(-(spanAngle*(1-firstWire)/2.+innerTwistAngle));

      std::ostringstream osnameLayerPhys;
      osnameLayerPhys<<"physical"<< "CdcStereoLayer"<<i;
      G4VPhysicalVolume* stereoLayer_phys;
      stereoLayer_phys=new G4PVPlacement(layerRot,0,stereoLayer_log,osnameLayerPhys.str(),
	  mdc_log,false,i);
      //Cell
      std::ostringstream osnameCellSolid;
      osnameCellSolid<<"solid"<< "CdcStereoLayer"<<i<<"Cell";

      G4TwistedTubs* twistTub=new G4TwistedTubs(osnameCellSolid.str(), -innerTwistAngle*2, innerR,outR-1.0*micrometer,innerLength,spanAngle);

      std::ostringstream osnameCellLogical;
      osnameCellLogical<<"logical"<< "CdcStereoLayer"<<i<<"Cell";
      G4LogicalVolume* twistTub_log=new G4LogicalVolume(twistTub, Cdcgas,osnameCellLogical.str(),0,0,0);

      twistTub_log->SetVisAttributes(G4VisAttributes::Invisible);
      twistTub_log->SetSensitiveDetector( aTrackerSD );
      //       twistTub_log->SetUserLimits(new G4UserLimits(maxStep));       

      //Wire
      if(ReadBoostRoot::GetMdc()==1){
	std::ostringstream osnameFieldWireSolid;
	osnameFieldWireSolid<<"solid"<< "StereoLayer"<<i<<"FieldWire";
	G4Tubs* fieldWire_tube=new G4Tubs(osnameFieldWireSolid.str(),0.,fieldWireR,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireLogical;
	osnameFieldWireLogical<<"logical"<< "StereoLayer"<<i<<"FieldWire";
	G4LogicalVolume* fieldWire_log=new G4LogicalVolume(fieldWire_tube,Au,osnameFieldWireLogical.str(),0,0,0);

	std::ostringstream osnameFieldWireSubSolid;
	osnameFieldWireSubSolid<<"solid"<< "StereoLayer"<<i<<"FieldWireSub";
	G4Tubs* fieldWireSub_tube=new G4Tubs(osnameFieldWireSubSolid.str(),0.,fieldWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireSubLogical;
	osnameFieldWireSubLogical<<"logical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4LogicalVolume* fieldWireSub_log=new G4LogicalVolume(fieldWireSub_tube,Al,osnameFieldWireSubLogical.str(),0,0,0);
	std::ostringstream osnameFieldWireSubPhys;
	osnameFieldWireSubPhys<<"physical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4VPhysicalVolume* fieldWireSub_phys;
	fieldWireSub_phys=new G4PVPlacement(0,0,fieldWireSub_log,
	    osnameFieldWireSubPhys.str(),fieldWire_log,false,0);

	std::ostringstream osnameSignalWireSolid;
	osnameSignalWireSolid<<"solid"<< "StereoLayer"<<i<<"SignalWire";
	G4Tubs* signalWire_tube=new G4Tubs(osnameSignalWireSolid.str(),0.,signalWireR,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireLogical;
	osnameSignalWireLogical<<"logical"<< "StereoLayer"<<i<<"SignalWire";
	G4LogicalVolume* signalWire_log=new G4LogicalVolume(signalWire_tube,Au,osnameSignalWireLogical.str(),0,0,0);

	std::ostringstream osnameSignalWireSubSolid;
	osnameSignalWireSubSolid<<"solid"<< "StereoLayer"<<i<<"SignalWireSub";
	G4Tubs* signalWireSub_tube=new G4Tubs(osnameSignalWireSubSolid.str(),0.,signalWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireSubLogical;
	osnameSignalWireSubLogical<<"logical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4LogicalVolume* signalWireSub_log=new G4LogicalVolume(signalWireSub_tube,W,osnameSignalWireSubLogical.str(),0,0,0);
	std::ostringstream osnameSignalWireSubPhys;
	osnameSignalWireSubPhys<<"physical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4VPhysicalVolume* signalWireSub_phys;
	signalWireSub_phys=new G4PVPlacement(0,0,signalWireSub_log,
	    osnameSignalWireSubPhys.str(),signalWire_log,false,0);

	//phi  <------------------->-phi 
	//      |      F5     F2|  There are 1 signal wire S4,3 full field  
	//      |               |  wire F0,1,3 in each cell except Layer 17, 
	//      |      S4     F1|  2 more full field wire F2,5.
	//      |               |  In stereo cell we can't put half wire at edge because of
	//      |      F3     F0|  overlap, so all wires are full wire,Positon of field   
	//      ----------------|  wire is not at cell edge but move into cell avoid overlap
	G4double shiftR,midR,eastX,eastY,westX,westY;
	shiftR=fieldWireR+1*micrometer;
	midR=mdc->Layer(signalLayer).R()*mm;

	std::ostringstream osnameFieldWire0Phys;
	osnameFieldWire0Phys<<"physicalStereoLayer"<<i<<"FieldWire0";

	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);

	G4ThreeVector east0(eastX,eastY,innerLength);
	G4ThreeVector west0(westX,westY,-innerLength);
	east0.rotateZ(-spanAngle/2.);
	west0.rotateZ(-spanAngle/2.);

	posX=(east0.x()+west0.x())/2.;
	posY=(east0.y()+west0.y())/2.;
	G4ThreeVector line0=east0-west0;
	G4RotationMatrix* wireRot0=new G4RotationMatrix();
	wireRot0->rotateZ(-line0.phi());
	wireRot0->rotateY(-line0.theta());
	G4VPhysicalVolume* fieldWire0_phys;
	fieldWire0_phys=new G4PVPlacement(wireRot0,G4ThreeVector(posX,posY,0),fieldWire_log,
	    osnameFieldWire0Phys.str(),twistTub_log,false,0);

	std::ostringstream osnameFieldWire1Phys;
	osnameFieldWire1Phys<<"physicalStereoLayer"<<i<<"FieldWire1";
	eastX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=midR*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	G4ThreeVector east1(eastX,eastY,innerLength);
	G4ThreeVector west1(westX,westY,-innerLength);
	east1.rotateZ(-spanAngle/2.);
	west1.rotateZ(-spanAngle/2.);
	posX=(east1.x()+west1.x())/2.;
	posY=(east1.y()+west1.y())/2.;
	G4ThreeVector line1=east1-west1;
	G4RotationMatrix* wireRot1=new G4RotationMatrix();
	wireRot1->rotateZ(-line1.phi());
	wireRot1->rotateY(-line1.theta());
	G4VPhysicalVolume* fieldWire1_phys;
	fieldWire1_phys=new G4PVPlacement(wireRot1,G4ThreeVector(posX,posY,0),fieldWire_log,
	    osnameFieldWire1Phys.str(),twistTub_log,false,1);

	std::ostringstream osnameFieldWire3Phys;
	osnameFieldWire3Phys<<"physicalStereoLayer"<<i<<"FieldWire3";

	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);

	G4ThreeVector east3(eastX,eastY,innerLength);
	G4ThreeVector west3(westX,westY,-innerLength);

	posX=(east3.x()+west3.x())/2.;
	posY=(east3.y()+west3.y())/2.;
	G4ThreeVector line3=east3-west3;
	G4RotationMatrix* wireRot3=new G4RotationMatrix();
	wireRot3->rotateZ(-line3.phi());
	wireRot3->rotateY(-line3.theta());
	G4VPhysicalVolume* fieldWire3_phys;
	fieldWire3_phys=new G4PVPlacement(wireRot3,G4ThreeVector(posX,posY,0),fieldWire_log,
	    osnameFieldWire3Phys.str(),twistTub_log,false,3);

	std::ostringstream osnameSignalWire4Phys;
	osnameSignalWire4Phys<<"physicalStereoLayer"<<i<<"SignalWire4";

	eastX=midR*cos(innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle);
	westX=midR*cos(innerTwistAngle);
	westY=midR*sin(innerTwistAngle);

	G4ThreeVector east4(eastX,eastY,innerLength);
	G4ThreeVector west4(westX,westY,-innerLength);

	posX=(east4.x()+west4.x())/2.;
	posY=(east4.y()+west4.y())/2.;
	G4ThreeVector line4=east4-west4;
	G4RotationMatrix* wireRot4=new G4RotationMatrix();
	wireRot4->rotateZ(-line4.phi());
	wireRot4->rotateY(-line4.theta());
	G4VPhysicalVolume* signalWire4_phys;
	signalWire4_phys=new G4PVPlacement(wireRot4,G4ThreeVector(posX,posY,0),signalWire_log,
	    osnameSignalWire4Phys.str(),twistTub_log,false,4);

	if(i==17){
	  std::ostringstream osnameFieldWire2Phys;
	  osnameFieldWire2Phys<<"physicalStereoLayer"<<i<<"FieldWire2";

	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);

	  G4ThreeVector east2(eastX,eastY,innerLength);
	  G4ThreeVector west2(westX,westY,-innerLength);
	  east2.rotateZ(-spanAngle/2.);
	  west2.rotateZ(-spanAngle/2.);

	  posX=(east2.x()+west2.x())/2.;
	  posY=(east2.y()+west2.y())/2.;
	  G4ThreeVector line2=east2-west2;
	  G4RotationMatrix* wireRot2=new G4RotationMatrix();
	  wireRot2->rotateZ(-line2.phi());
	  wireRot2->rotateY(-line2.theta());
	  G4VPhysicalVolume* fieldWire2_phys;
	  fieldWire2_phys=new G4PVPlacement(wireRot2,G4ThreeVector(posX,posY,0),fieldWire_log,
	      osnameFieldWire2Phys.str(),twistTub_log,false,2);

	  std::ostringstream osnameFieldWire5Phys;
	  osnameFieldWire5Phys<<"physicalStereoLayer"<<i<<"FieldWire5";

	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);

	  G4ThreeVector east5(eastX,eastY,innerLength);
	  G4ThreeVector west5(westX,westY,-innerLength);

	  posX=(east5.x()+west5.x())/2.;
	  posY=(east5.y()+west5.y())/2.;
	  G4ThreeVector line5=east5-west5;
	  G4RotationMatrix* wireRot5=new G4RotationMatrix();
	  wireRot5->rotateZ(-line5.phi());
	  wireRot5->rotateY(-line5.theta());
	  G4VPhysicalVolume* fieldWire5_phys;
	  fieldWire5_phys=new G4PVPlacement(wireRot5,G4ThreeVector(posX,posY,0),fieldWire_log,
	      osnameFieldWire5Phys.str(),twistTub_log,false,5);
	}
      }

      //Put cells into layer
      for(j=replicaNo-1;j>-1;j--){
	G4RotationMatrix* cellRot=new G4RotationMatrix();
	cellRot->rotateZ(-spanAngle*j);

	std::ostringstream osnameCellPhys;
	osnameCellPhys<<"physical"<< "MdcStereoLayer"<<i<<"Cell"<<j;
	G4VPhysicalVolume* twistTub_phys;
	twistTub_phys=new G4PVPlacement(cellRot,0,twistTub_log,
	    osnameCellPhys.str(),stereoLayer_log,false,j);
      }
    }


  }
}
