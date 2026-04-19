const ifj = @import("ifj24.zig");

pub fn main() void {
    var a = ifj.string("");
    var b = ifj.string("");
    var c = ifj.concat(a, b);
    ifj.write(c);
    ifj.write("\n");
}

pub fn add(a: i32, b: i32) i32 {
    return a + b;
}

const ifj = @import("ifj24.zig");

pub fn main() void {
    var a = ifj.string("1");
    var b = ifj.string("2");
    if (strcmp(a, b)) {
        ifj.write("1");
        ifj.write("\n");
    } else {
        ifj.write("0");
        ifj.write("\n");
    }
}

pub fn add(a: i32, b: i32) i32 {
    return a + b;
}

const ifj = @import("ifj24.zig");

pub fn main() void {
    var a = 1;
    while (a == 1) {
        ifj.write("1");
        ifj.write("\n");
        a = 0;
    } else {
        ifj.write("0");
        ifj.write("\n");
    }
}

pub fn add(a: i32, b: i32) i32 {
    return a + b;
}
