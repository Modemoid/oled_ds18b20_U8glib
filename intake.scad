M6hole = 3; //use radius
HoleDist = 45;
BaseHeight = 5;
IntakeHole = 24/2;//use radius
Qw = 10; //render quolity
MainSealOffset = 2;
MainSealDia = 0.6;
BaseHoleBoarder = M6hole + 4;
IntekeBoarder = 7;



difference(){
{//base plate contact zone
hull(){
cylinder(BaseHeight,BaseHoleBoarder,BaseHoleBoarder,$fn = Qw);
{translate([HoleDist,0,0])
	cylinder(BaseHeight,BaseHoleBoarder,BaseHoleBoarder,$fn = Qw);
	}
{	translate([HoleDist/2,0,0])
	cylinder(BaseHeight,IntakeHole+IntekeBoarder,IntakeHole+IntekeBoarder,$fn = Qw);
	}
}
//base plate contact zone
};

translate([0,0,-0.5]){
{cylinder(BaseHeight+1,M6hole,M6hole,$fn = Qw);
 {translate([HoleDist,0,0])
	cylinder(BaseHeight+1,M6hole,M6hole,$fn = Qw);
	}
{	translate([HoleDist/2,0,0])
	cylinder(BaseHeight+1,IntakeHole,IntakeHole,$fn = Qw);
	}
}

}
{translate([HoleDist/2,0,0])
rotate_extrude(convexity = 100)
translate([IntakeHole + MainSealOffset, 0, 0])
circle(r = MainSealDia,$fn = Qw); }

}