M6hole = 3; //use radius
HoleDist = 45;
BaseHeight = 5;
IntakeHole = 24/2;//use radius
Qw = 30; //render quolity
MainSealOffset = 2;
MainSealDia = 0.6;
BaseHoleBoarder = M6hole + 4;
IntekeBoarder = 7;

TractBoarder = 2;
TractHeight = 26;
IngBottHeight = 15;

//projection(cut=true){


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

translate([HoleDist/2,0,BaseHeight]){
cylinder(TractHeight+IngBottHeight,IntakeHole+TractBoarder,IntakeHole+TractBoarder,$fn = Qw);
}



translate([HoleDist/2-TractBoarder/2,-(TractHeight+TractBoarder)/2,BaseHeight+TractHeight-IntakeHole]){
cube ([TractHeight+TractBoarder ,(TractHeight+TractBoarder) ,TractHeight+TractBoarder ]);
}    
translate([HoleDist/2+IntakeHole,0,BaseHeight+TractHeight-IntakeHole]){
rotate([90,0,0])
rotate_extrude(convexity = 100)
translate([12, 0, 0])
circle(r = IntakeHole,$fn = Qw); }




/*

translate([0,0,BaseHeight]){
{	
    translate([HoleDist/2,0,0]){
	  difference(){
        union(){ 
        
       
        translate([0-TractBoarder*2,0,TractHeight-TractHeight/2.5]){
        rotate([0,45,0]){
            cylinder(TractHeight,IntakeHole+TractBoarder,IntakeHole+TractBoarder,$fn = Qw);
            }
        }
    }
        

        union(){      
            translate([0,0,-0.1]){
            cylinder(TractHeight+1,IntakeHole,IntakeHole,$fn = Qw);}

        translate([0-TractBoarder*2,0,TractHeight-TractHeight/2.5]){
        rotate([0,45,0]){
       
            cylinder(TractHeight+1,IntakeHole,IntakeHole,$fn = Qw);
            }
        }
    }
}
}
}
      

}

//}