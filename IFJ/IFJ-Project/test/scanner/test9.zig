const ifj = @import("ifj24.zig");

pub fn main() void {
    var x = 1.0 + ifj.i2f(tes());
    ifj.write(x);
    // x = ifj.readi32();
    // if (x == 2) {
    //     var o = 3 + 6;
    //     var b = o + 2;
    // } else {
    //     var t = x + x;
    //     ifj.write(t);
    //     ifj.write("\n");
    // }
}

pub fn tes() i32 {
    return 1;
}
