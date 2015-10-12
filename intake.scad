M6hole = 3; //use radius
HoleDist = 45;
BaseHeight = 15;
IntakeHole = 25/2;//use radius
cylinder(BaseHeight,M6hole,M6hole);
 {translate([HoleDist,0,0])
	cylinder(BaseHeight,M6hole,M6hole);
	}
{	translate([HoleDist/2,0,0])
	cylinder(BaseHeight,IntakeHole,IntakeHole);
	}
