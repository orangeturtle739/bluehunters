$fa = 1;

color("white", 1) {
    union() {
        translate([0,0,-50]) cylinder(h=200, r=400, center=true);
        translate([0,-200,-50]) cube(size=[800,400,200], center=true);
    }

}

color("blue", 1) {
    translate([400 + 50, 0, -100]) {
        rotate([0, 90, 0]) {
            cylinder(h=100, r=200, center=true);
        }
    }
    translate([-(400 + 50), 0, -100]) {
        rotate([0, 90, 0]) {
            cylinder(h=100, r=200, center=true);
        }
    }
}

color("yellow", 1) {
    union() {
        translate([0, -350, -200]) {
            cylinder(h=100, r=50, center=true);
        }
        translate([0, -350, -250]) {
            sphere(r=50, center=true);
        }
    }
}