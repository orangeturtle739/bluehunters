tol = 1;
screw_extra = 2;
fudge = 0.1;
$fa = 1;
$fs = 0.1;

servo_length = 22.58;
servo_height = 12.28;
flange_thickness = 2.38;
flange_to_back = 17.82 - flange_thickness;
flange_to_front = 4.36;
mounting_hole_diamter = 2.13;
hole_out_dist = 30.28;
hole_center_dist = hole_out_dist - mounting_hole_diamter;
screw_length = 9.42;
buffer_length = 3;

plate_thickness = screw_length + screw_extra;
plate_length = hole_center_dist + mounting_hole_diamter + 2*buffer_length;
plate_height = servo_height + + 2*buffer_length;
inner_length = servo_length + tol*2;
inner_height = servo_height + tol*2;

module servo_mount_plate() {
    difference() {
        cube([plate_length,plate_thickness,plate_height], true);
        translate([0, 0, buffer_length/2]) cube([inner_length,plate_thickness + fudge, inner_height + buffer_length + fudge], true);
        translate([-hole_center_dist/2, (plate_thickness + fudge)/2, 0])
            rotate([90,0,0])
                cylinder(plate_thickness + fudge, d=mounting_hole_diamter, true);
        translate([hole_center_dist/2, (plate_thickness + fudge)/2, 0])
            rotate([90,0,0])
                cylinder(plate_thickness + fudge, d=mounting_hole_diamter, true);
    }
}

base_length = 90;
base_width = 90;
base_thickness = 4;
module base() {
        union() {
        translate([base_length/4 -fudge, 0, 0]) cube([base_length/2, base_width, base_thickness], center=true);
        cylinder(base_thickness, d=base_length, center=true);
        }
}


horn_diameter = 26.66;
horn_center = 11.35;
horn_tip = 4.25;
horn_in = 7.94;
horn_thickness = 2.2;

module servo_horn() {
    union() {
    linear_extrude(horn_thickness,true) offset(r=0.4)
    union() {
        square(horn_in, true);
        for (i = [0 : 90 : 270])
            union() {
            rotate([0, 0, i]) translate([horn_diameter/2 - horn_tip/2, 0, 0])circle(d=horn_tip);
             rotate([0, 0, i]) polygon(points=[[-horn_in/2, horn_in/2],[horn_in/2, horn_in/2],[horn_tip/2, (horn_diameter/2 - horn_tip/2)],[-horn_tip/2, (horn_diameter/2 - horn_tip/2)]]);
            }
    }
    cylinder(h = 100, d=7, center=true);
}
}

wheel_diameter = 60;
wheel_thickness = 5;
wheel_cut_diameter = 12;
module wheel() {
    difference() {
        translate([0, 0, 0]) cylinder(wheel_thickness,d=wheel_diameter, true);
        translate([0, 0, wheel_thickness - horn_thickness + fudge])servo_horn();
        for (i = [0 : 45 : 360-45])
             rotate([0, 0, i]) translate([wheel_diameter/2 - wheel_cut_diameter/2 - buffer_length, 0, -fudge])cylinder(wheel_thickness + 2*fudge,d=wheel_cut_diameter, true);
    }
}

post_diameter = 15;
cone_extenstion = 2;

triangle_width = 20;
triangle_height = 10;
triangle_offset = 5;
triangle_thickness = 100;
module round_triangle() {
    translate([-triangle_height-2*triangle_offset, 0, -triangle_thickness/2]) linear_extrude(triangle_thickness) offset(triangle_offset, chamfer) translate([0, -triangle_width/2, 0])polygon([[0, 0], [0, triangle_width], [triangle_height, triangle_width/2]]);
}

module robot() {
    difference() {
    union() {
        translate([base_length/2 - plate_length/2, base_width / 2 - plate_thickness/2, plate_height/2 + base_thickness/2 - fudge]) servo_mount_plate();
        translate([base_length/2 - plate_length/2, - (base_width / 2 - plate_thickness/2), plate_height/2 + base_thickness/2 - fudge]) servo_mount_plate();
        base();
    }
    translate([-base_length/2 + post_diameter /2 + cone_extenstion, 0, -(base_thickness/2 + fudge)]) cylinder(base_thickness + 2*fudge,d=post_diameter,true);
    round_triangle(); 
    rotate([0, 0, 180]) translate([0, 0, 0]) round_triangle();
    rotate([0, 0, 90]) union(){
        round_triangle(); 
        rotate([0, 0, 180]) translate([0, 0, 0]) round_triangle();
    }
    }
}

module drag() {
    translate([0, 0, post_diameter/2])
    union() {
        offset = plate_height/2 + base_thickness;
        needed = wheel_diameter / 2 - offset - post_diameter/2;
        cylinder(needed,d1=post_diameter,d2=post_diameter+2*cone_extenstion,true);
        translate([0, 0, needed]) cylinder(base_thickness + fudge,d=post_diameter - tol/2 ,true);
        sphere(d=post_diameter);
    }
}
//servo_horn();
//wheel();
//robot();
//base();
//round_triangle();
drag();
//servo_mount_plate();