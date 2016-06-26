
epsilon = 0.1;

module mx_cutout(){  // origin at center, bottom view
    translate([-7,-7,-1]){
        union(){
            cube([14,14,5]);
            translate([0,-1.5,1+1.4]){
                cube([14,14+3,5]);
            }
        }
    }
}

module top_plate(){
    difference(){
        translate([-19,-19,0])
            cube([38,38,2.4]);
        translate([-9.5,-9.5,0])
            mx_cutout();
        translate([-9.5,+9.5,0])
            mx_cutout();
        translate([+9.5,+9.5,0])
            mx_cutout();
        translate([+9.5,-9.5,0])
            mx_cutout();
    }
}


module body(){
    difference(){
        cube([42,42,16]);  // main body
        translate([4,4,4])  // large cutout
            cube([34,34,16]);
        translate([2-epsilon,2-epsilon,16-2.4-epsilon])  // lip
            cube([38+2*epsilon,38+2*epsilon, 6]);
        translate([42/2 - 11.3/2, 2, 2])  // PCB
            cube([11.3, 22.3, 5]);
        translate([42/2 - 10/2, -1, 2+1.2])  // USB hole
            cube([10,6,3.7]);
    }
}



body();
translate([19+2, 19+2, 16]) scale([1,1,-1]) top_plate();

// body();
// top_plate();    
        